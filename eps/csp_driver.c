// csp_driver.c
// Created by Simeon Shaffar on 11/17/2023
// This c file is the csp interface layer of the EPS software

#include "csp_driver.h"


// This is the main driver function to be used for arbitrary commands within the service
// You give it a name of the parameter you want, and it returns the value of that parameter,
// converted into a string since this would ultimately be read through the command logs
const char* pull_parameter_string(const char* name, bool mission, int offset) {
	param_t* param = find_parameter(name, mission);
		if (param == NULL) return "";
		if (offset < -1) return "";
		if (offset >= param->array_size) return "";

	int result = pull_parameter(param, NULL);
		if (result < 0) return NULL;
	
	const char* value = param_value_string(param, offset);

	return value;
} // pull_parameter_string


// This function pulls a parameter and copies its contents into the given bucket, 
// it is ideal for when you want to fill data into a numeric type
int pull_parameter(param_t* param, void* bucket) {
	if (param == NULL) return -1;

	csp_timestamp_t zeroTimestamp = {0, 0};
	*param->timestamp = zeroTimestamp; //Set the timestamp to 0 otherwise it bugs out sometimes
	int result = param_pull_single(param, INDEX_ALL, CSP_PRIO_NORM, VERBOSE, *param->node, TIMEOUT, CSP_VERSION);

	if (bucket != NULL)
		memcpy(bucket, param->addr, param->array_size * param->array_step);

	return result;
} // pull_parameter


// Setter function for arbitrary parameters
// Essentially it is just a wrapper of param_push_single
int push_parameter(param_t* param, void* new_value, int offset) {
	csp_timestamp_t zeroTimestamp = {0, 0};
	*param->timestamp = zeroTimestamp;
	int result = param_push_single(param, offset, CSP_PRIO_NORM, new_value, 0, 
									*param->node, TIMEOUT, CSP_VERSION, ACK_WITH_PULL);

	// printf("\nRan push_parameter on %s[%d], val: %s\n", param->name, offset, param_value_string(param, offset));

	return result;
} // push_parameter


// This function takes in a parameter, offset, and new value, all in pure strings
// it is designed to be accessed from the service, where we recieve strings from the UDP commands
// 
// Output values: 
//	0 - Successs
// -1 - Network error from param_push_single
// -2 - No valid parameter found for the given name
// -3 - Offset casting error 
// -4 - New value casting error
//
int push_parameter_string(const char* name, bool mission, const char* offset_str, const char* new_value_str) {
	// Get the parameter from the name
	param_t* param = find_parameter(name, mission);
		if (param == NULL) { return -2; }

	// This is like a globally defined value deep in C libraries.
	// The strol function will change it if it has any issues.
	errno = 0;

	// Convert the offset into an integer
	char* offset_endptr;
    long int offset_long = strtol(offset_str, &offset_endptr, 10); // Base 10 for decimal
		if (errno != 0) return -3;
		else if (*offset_endptr != '\0') return -3;
		else if (offset_endptr == offset_str) return -3;
		else if (offset_long >= param->array_size) return -3;
		else if (offset_long == -1 && param->array_size == 1) {}	// -1 ok for scalars only
		else if (offset_long < 0) return -3;
	// At this point offset_long is guaranteed to be within the array bounds and above zero
	int offset = offset_long;


	// If it is some form of integer
	if (param->type <= PARAM_TYPE_XINT64) {
		// Cast into a signed long int, we will then convert down into other types
		char* endptr;
		long long int long_val = strtoll(new_value_str, &endptr, 10);	// Base 10 for decimal
			if (errno != 0) return -4;
			else if (*endptr != '\0') { return -4; }
			else if (endptr == new_value_str) { return -4; }
		// At this point long_val is guaranteed to be the value put in, no garbage casting conversions


		switch (param->type) {
			case PARAM_TYPE_UINT8:
			case PARAM_TYPE_XINT8:
			{
				if (long_val < 0 || UINT8_MAX < long_val) { return -4; }
				uint8_t val = long_val;
				return push_parameter(param, &val, offset);
			}
			case PARAM_TYPE_UINT16:
			case PARAM_TYPE_XINT16:
			{
				if (long_val < 0 || UINT16_MAX < long_val) { return -4; }
				uint16_t val = long_val;
				return push_parameter(param, &val, offset);
			}
			case PARAM_TYPE_UINT32:
			case PARAM_TYPE_XINT32:
			{
				if (long_val < 0 || UINT32_MAX < long_val) { return -4; }
				uint32_t val = long_val;
				return push_parameter(param, &val, offset);
			}
			case PARAM_TYPE_UINT64:
			case PARAM_TYPE_XINT64:
			{
				if (long_val < 0 || UINT64_MAX < long_val) { return -4; }
				uint64_t val = long_val;
				return push_parameter(param, &val, offset);
			}

			case PARAM_TYPE_INT8:
			{
				if (long_val < INT8_MIN || INT8_MAX < long_val) { return -4; }
				int8_t val = long_val;
				return push_parameter(param, &val, offset);
			}
			case PARAM_TYPE_INT16:
			{
				if (long_val < INT16_MIN || UINT16_MAX < long_val) { return -4; }
				int16_t val = long_val;
				return push_parameter(param, &val, offset);
			}
			case PARAM_TYPE_INT32:
			{
				if (long_val < INT32_MIN || UINT32_MAX < long_val) { return -4; }
				int32_t val = long_val;
				return push_parameter(param, &val, offset);
			}
			case PARAM_TYPE_INT64:
			{
				if (long_val < INT64_MIN || UINT64_MAX < long_val) { return -4; }
				int64_t val = long_val;
				return push_parameter(param, &val, offset);
			}

			default:
				break;
		}
	} // If it is some sort of integer

	else if (param->type == PARAM_TYPE_DOUBLE) {
		char* endptr;
		double val = strtod(new_value_str, &endptr);
			if (errno != 0) return -4;
			else if (*endptr != '\0') return -4;
			else if (endptr == new_value_str) return -4;

		return push_parameter(param, &val, offset);
	} // If it is a double

	else if (param->type == PARAM_TYPE_FLOAT) {
		char* endptr;
		float val = strtof(new_value_str, &endptr);
			if (errno != 0) return -4;
			else if (*endptr != '\0') return -4;
			else if (endptr == new_value_str) return -4;

		return push_parameter(param, &val, offset);
	} // If it is a float

	else if (param->type == PARAM_TYPE_DATA || param->type == PARAM_TYPE_STRING) {
		// Copy the new value string, add 1 for the null character
		char val[strlen(new_value_str) + 1];
		strcpy(val, new_value_str);
		
		return push_parameter(param, val, offset);
	}
	
	return -2;
} // push_parameter_string


// This function takes in a name and returns the parameter, if any, corresponding to it
// This function looks over all the parameters in the impress_params_a array
// trying to find any parameter that matches the given name. It returns 
// the index if it does find one, otherwise it returns -1.

param_t* find_parameter(const char* name, bool mission) {
	if (name == NULL) return NULL;

	int idx = -1;

	// Find the parameter from the given name
	if (mission == IMPRESS) {
		for(int i = 0; i < IMPRESS_PARAMS_COUNT; i++) {
			if (strcmp(impress_params_a[i]->name, name) == 0) {
				idx = i;
				break;
			}
		};
		if (idx < 0) { return NULL; }

		return impress_params_a[idx];
	} 
	
	else { // If the mission is EXACT
		for(int i = 0; i < PDUP4_PARAMS_COUNT; i++) {
			if (strcmp(pdup4_params_a[i]->name, name) == 0) {
				idx = i;
				break;
			}
		};
		if (idx < 0) { return NULL; }

		return pdup4_params_a[idx];
	}
} // find_parameter



// This is the default "dumb" way to build the health packet
// We simply run our pull_parameter function on each element in it
// It takes roughly 10us to build it due to 5 CSP packets being sent accross the CAN
int build_impress_health(eps_pdup3_health_t* hp) {
	param_t* pcdu_temp  = impress_params_a[PCDU_TEMP];
	param_t* pcdu_vbatt = impress_params_a[PCDU_VBATT];
	param_t* pcdu_ch_on = impress_params_a[PCDU_CH_ON];
	param_t* pcdu_i_cur = impress_params_a[PCDU_I_CUR];
	param_t* pcdu_v_cur = impress_params_a[PCDU_V_CUR];

	param_t* batt_heater_on = impress_params_a[BATT_HEATER_ON];
    param_t* batt_balance_cell = impress_params_a[BATT_BALANCE_CELL];
    param_t* batt_temp = impress_params_a[BATT_TEMP];
    param_t* batt_vin = impress_params_a[BATT_VIN];
    param_t* batt_vout = impress_params_a[BATT_VOUT];
    param_t* batt_iin = impress_params_a[BATT_IIN];
    param_t* batt_iout = impress_params_a[BATT_IOUT];

    param_t* mppt_temp = impress_params_a[MPPT_TEMP];
    param_t* mppt_iin = impress_params_a[MPPT_IIN];
    param_t* mppt_vin = impress_params_a[MPPT_VIN];

	int result = 0;

	result  = pull_parameter(pcdu_temp,  &hp->pcdu_temp);
		if (result < 0) return result;
	result  = pull_parameter(pcdu_vbatt, &hp->pcdu_vbatt);
		if (result < 0) return result;
	result  = pull_parameter(pcdu_ch_on, hp->pcdu_ch_on);
		if (result < 0) return result;
	result  = pull_parameter(pcdu_i_cur, hp->pcdu_icur);
		if (result < 0) return result;
	result  = pull_parameter(pcdu_v_cur, hp->pcdu_vcur);
		if (result < 0) return result;

	result  = pull_parameter(batt_heater_on, &hp->batt_heater_on);
		if (result < 0) return result;
	result  = pull_parameter(batt_balance_cell, &hp->batt_balance_cell);
		if (result < 0) return result;
	result  = pull_parameter(batt_temp, &hp->batt_temp);
		if (result < 0) return result;
	result  = pull_parameter(batt_vin, &hp->batt_vin);
		if (result < 0) return result;
	result  = pull_parameter(batt_vout, &hp->batt_vout);
		if (result < 0) return result;
	result  = pull_parameter(batt_iin, &hp->batt_iin);
		if (result < 0) return result;
	result  = pull_parameter(batt_iout, &hp->batt_iout);
		if (result < 0) return result;

	result  = pull_parameter(mppt_temp, &hp->mppt_temp);
		if (result < 0) return result;
	result  = pull_parameter(mppt_iin, hp->mppt_iin);
		if (result < 0) return result;
	result  = pull_parameter(mppt_vin, hp->mppt_vin);
		if (result < 0) return result;

	return 0;
} // build_impress_health


// Theoretically this is the better way to build the health packet, takes only ~2us
//
// It builds a queue that requests all the health data at the same time in one CSP packet
// I ran into a problem where if you do param_pull_single on any of the params in the health packet
// it leads to all the following parameters in this queue just not pulling. Hard to understand.
//
// It actually returns 0 for success too, which is even more hard to understand.
int build_impress_health_queue(eps_pdup3_health_t* hp) {
	// Here we grab the pointers to the parameters we will use
	param_t* pcdu_temp  = impress_params_a[PCDU_TEMP];
	param_t* pcdu_vbatt = impress_params_a[PCDU_VBATT];
	param_t* pcdu_ch_on = impress_params_a[PCDU_CH_ON];
	param_t* pcdu_i_cur = impress_params_a[PCDU_I_CUR];
	param_t* pcdu_v_cur = impress_params_a[PCDU_V_CUR];

	param_t* batt_heater_on = impress_params_a[BATT_HEATER_ON];
    param_t* batt_balance_cell = impress_params_a[BATT_BALANCE_CELL];
    param_t* batt_temp = impress_params_a[BATT_TEMP];
    param_t* batt_vin = impress_params_a[BATT_VIN];
    param_t* batt_vout = impress_params_a[BATT_VOUT];
    param_t* batt_iin = impress_params_a[BATT_IIN];
    param_t* batt_iout = impress_params_a[BATT_IOUT];

    param_t* mppt_temp = impress_params_a[MPPT_TEMP];
    param_t* mppt_iin = impress_params_a[MPPT_IIN];
    param_t* mppt_vin = impress_params_a[MPPT_VIN];

	csp_timestamp_t zeroTimestamp = {0, 0};

	// Reset timestamps
	*pcdu_temp->timestamp = zeroTimestamp;
	*pcdu_vbatt->timestamp = zeroTimestamp;
	*pcdu_ch_on->timestamp = zeroTimestamp;
	*pcdu_i_cur->timestamp = zeroTimestamp;
	*pcdu_v_cur->timestamp = zeroTimestamp;

	*batt_heater_on->timestamp = zeroTimestamp;
	*batt_balance_cell->timestamp = zeroTimestamp;
	*batt_temp->timestamp = zeroTimestamp;
	*batt_vin->timestamp = zeroTimestamp;
	*batt_vout->timestamp = zeroTimestamp;
	*batt_iin->timestamp = zeroTimestamp;
	*batt_iout->timestamp = zeroTimestamp;

	*mppt_temp->timestamp = zeroTimestamp;
	*mppt_iin->timestamp = zeroTimestamp;
	*mppt_vin->timestamp = zeroTimestamp;

	
	// We then build the actual main health packet queue
	param_queue_t pcdu_queue;
	uint8_t pcdu_queue_buf[PARAM_SERVER_MTU-2];
	param_queue_init(&pcdu_queue, pcdu_queue_buf, PARAM_SERVER_MTU-2, 0, PARAM_QUEUE_TYPE_GET, CSP_VERSION);

	param_queue_add(&pcdu_queue, pcdu_temp, INDEX_ALL, NULL);
	param_queue_add(&pcdu_queue, pcdu_vbatt, INDEX_ALL, NULL);
	param_queue_add(&pcdu_queue, pcdu_ch_on, INDEX_ALL, NULL);
	param_queue_add(&pcdu_queue, pcdu_i_cur, INDEX_ALL, NULL);
	param_queue_add(&pcdu_queue, pcdu_v_cur, INDEX_ALL, NULL);


	param_queue_t batt_queue;
	uint8_t batt_queue_buf[PARAM_SERVER_MTU-2];
	param_queue_init(&batt_queue, batt_queue_buf, PARAM_SERVER_MTU-2, 0, PARAM_QUEUE_TYPE_GET, CSP_VERSION);

	param_queue_add(&batt_queue, batt_heater_on, INDEX_ALL, NULL);
	param_queue_add(&batt_queue, batt_balance_cell, INDEX_ALL, NULL);
	param_queue_add(&batt_queue, batt_temp, INDEX_ALL, NULL);
	param_queue_add(&batt_queue, batt_vin, INDEX_ALL, NULL);
	param_queue_add(&batt_queue, batt_vout, INDEX_ALL, NULL);
	param_queue_add(&batt_queue, batt_iin, INDEX_ALL, NULL);
	param_queue_add(&batt_queue, batt_iout, INDEX_ALL, NULL);


	param_queue_t mppt_queue;
	uint8_t mppt_queue_buf[PARAM_SERVER_MTU-2];
	param_queue_init(&mppt_queue, mppt_queue_buf, PARAM_SERVER_MTU-2, 0, PARAM_QUEUE_TYPE_GET, CSP_VERSION);

	param_queue_add(&mppt_queue, mppt_temp, INDEX_ALL, NULL);
	param_queue_add(&mppt_queue, mppt_iin, INDEX_ALL, NULL);
	param_queue_add(&mppt_queue, mppt_vin, INDEX_ALL, NULL);


	// int batt_node[1] = {BATT};
	// param_print(batt_temp, -1, batt_node, 1, 10, *batt_temp->timestamp);
	// param_print(batt_vin, -1, batt_node, 1, 10, *batt_vin->timestamp);
	// param_print(batt_vout, -1, batt_node, 1, 10, *batt_vout->timestamp);
	// param_print(batt_iin, -1, batt_node, 1, 10, *batt_iin->timestamp);
	// param_print(batt_iout, -1, batt_node, 1, 10, *batt_iout->timestamp);

	// int mppt_node[1] = {MPPT};
	// param_print(mppt_temp, -1, mppt_node, 1, 10, *mppt_temp->timestamp);
	// param_print(mppt_iin, -1, mppt_node, 1, 10, *mppt_iin->timestamp);
	// param_print(mppt_vin, -1, mppt_node, 1, 10, *mppt_vin->timestamp);
	

	// This is the function that actually builds the packet and sends it to the hardware device, 
	// then retrieving the values
	int pcdu_result = param_pull_queue(&pcdu_queue, CSP_PRIO_NORM, VERBOSE, PCDU, TIMEOUT);
	int batt_result = param_pull_queue(&batt_queue, CSP_PRIO_NORM, VERBOSE, BATT, TIMEOUT);
	int mppt_result = param_pull_queue(&mppt_queue, CSP_PRIO_NORM, VERBOSE, MPPT, TIMEOUT);


	// We then copy all the values we just grabbed into the new health packet struct
	if (pcdu_result == 0) {
		memcpy(&hp->pcdu_temp, pcdu_temp->addr, sizeof(hp->pcdu_temp));
		memcpy(&hp->pcdu_vbatt, pcdu_vbatt->addr, sizeof(hp->pcdu_vbatt));
		memcpy(&hp->pcdu_ch_on, pcdu_ch_on->addr, sizeof(hp->pcdu_ch_on));
		memcpy(&hp->pcdu_icur, pcdu_i_cur->addr, sizeof(hp->pcdu_icur));
		memcpy(&hp->pcdu_vcur, pcdu_v_cur->addr, sizeof(hp->pcdu_vcur));
	}

	if (batt_result == 0) {
		memcpy(&hp->batt_heater_on, batt_heater_on->addr, sizeof(hp->batt_heater_on));
		memcpy(&hp->batt_balance_cell, batt_balance_cell->addr, sizeof(hp->batt_balance_cell));
		memcpy(&hp->batt_temp, batt_temp->addr, sizeof(hp->batt_temp));
		memcpy(&hp->batt_vin, batt_vin->addr, sizeof(hp->batt_vin));
		memcpy(&hp->batt_vout, batt_vout->addr, sizeof(hp->batt_vout));
		memcpy(&hp->batt_iin, batt_iin->addr, sizeof(hp->batt_iin));
		memcpy(&hp->batt_iout, batt_iout->addr, sizeof(hp->batt_iout));
	}

	if (mppt_result == 0) {
		memcpy(&hp->mppt_temp, mppt_temp->addr, sizeof(hp->mppt_temp));
		memcpy(&hp->mppt_iin, mppt_iin->addr, sizeof(hp->mppt_iin));
		memcpy(&hp->mppt_vin, mppt_vin->addr, sizeof(hp->mppt_vin));
	}

	return pcdu_result + batt_result + mppt_result;
} // build_impress_health_queue


int build_exact_health(eps_pdup4_health_t* hp) {
	param_t* pdu_ch_on  = pdup4_params_a[P4_CH_ON];
	param_t* pdu_bus_voltage  = pdup4_params_a[P4_BUS_VOLTAGE];
	param_t* pdu_ch_current  = pdup4_params_a[P4_CH_CURRENT];
	param_t* pdu_ch_voltage  = pdup4_params_a[P4_CH_VOLTAGE];
	param_t* pdu_ch_temp  = pdup4_params_a[P4_CH_TEMP];
	param_t* pdu_mcu_temp  = pdup4_params_a[P4_MCU_TEMP];

	int result = 0;

	result  = pull_parameter(pdu_ch_on,  &hp->pdu_ch_on);
		if (result < 0) return result;
	result  = pull_parameter(pdu_bus_voltage,  &hp->pdu_bus_voltage);
		if (result < 0) return result;
	result  = pull_parameter(pdu_ch_current,  &hp->pdu_ch_current);
		if (result < 0) return result;
	result  = pull_parameter(pdu_ch_voltage,  &hp->pdu_ch_voltage);
		if (result < 0) return result;
	result  = pull_parameter(pdu_ch_temp,  &hp->pdu_ch_temp);
		if (result < 0) return result;
	result  = pull_parameter(pdu_mcu_temp,  &hp->pdu_mcu_temp);
		if (result < 0) return result;

	return 0;
}

int build_exact_health_queue(eps_pdup4_health_t* hp) {
	param_t* pdu_mcu_temp  = pdup4_params_a[P4_MCU_TEMP];
	param_t* pdu_ch_on  = pdup4_params_a[P4_CH_ON];
	param_t* pdu_ch_voltage  = pdup4_params_a[P4_CH_VOLTAGE];
	param_t* pdu_ch_current  = pdup4_params_a[P4_CH_CURRENT];
	param_t* pdu_ch_temp  = pdup4_params_a[P4_CH_TEMP];
	param_t* pdu_bus_voltage  = pdup4_params_a[P4_BUS_VOLTAGE];

	csp_timestamp_t zeroTimestamp = {0, 0};
	
	*pdu_mcu_temp->timestamp = zeroTimestamp;
	*pdu_ch_on->timestamp = zeroTimestamp;
	*pdu_ch_voltage->timestamp = zeroTimestamp;
	*pdu_ch_current->timestamp = zeroTimestamp;
	*pdu_ch_temp->timestamp = zeroTimestamp;
	*pdu_bus_voltage->timestamp = zeroTimestamp;

	param_queue_t pdu_queue;
	uint8_t pdu_queue_buf[PARAM_SERVER_MTU-2];
	param_queue_init(&pdu_queue, pdu_queue_buf, PARAM_SERVER_MTU-2, 0, PARAM_QUEUE_TYPE_GET, CSP_VERSION);

	param_queue_add(&pdu_queue, pdu_mcu_temp, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_on, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_voltage, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_current, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_temp, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_bus_voltage, INDEX_ALL, NULL);
	
	int pdu_result = param_pull_queue(&pdu_queue, CSP_PRIO_HIGH, VERBOSE, PDU, TIMEOUT);

	if (pdu_result == 0) {
		memcpy(&hp->pdu_mcu_temp, pdu_mcu_temp->addr, sizeof(hp->pdu_mcu_temp));	
		memcpy(&hp->pdu_ch_on, pdu_ch_on->addr, sizeof(hp->pdu_ch_on));
		memcpy(&hp->pdu_ch_voltage, pdu_ch_voltage->addr, sizeof(hp->pdu_ch_voltage));
		memcpy(&hp->pdu_ch_temp, pdu_ch_temp->addr, sizeof(hp->pdu_ch_temp));
		memcpy(&hp->pdu_bus_voltage, pdu_bus_voltage->addr, sizeof(hp->pdu_bus_voltage));

		// The PDU firmware updated the ch_cur parameter from a uint16_t to int32_t. We don't care about the extra precision and don't want to update existing decoders, so we translate.
		int32_t intermediate_ch_current[PDUP4_CH_CNT];
		memcpy(&intermediate_ch_current, pdu_ch_current->addr, sizeof(int32_t) * PDUP4_CH_CNT);
		for (int i = 0; i < PDUP4_CH_CNT; i++) { hp->pdu_ch_current[i] = (uint16_t)intermediate_ch_current[i]; }
	}

	return pdu_result;
}

int pdup4_get_settings(eps_pdup4_settings_t* settings)
{
	param_t* pdu_ch_on_init = pdup4_params_a[P4_CH_ON_INIT];
	param_t* pdu_ch_type = pdup4_params_a[P4_CH_TYPE];
	param_t* pdu_ch_curlim = pdup4_params_a[P4_CH_CURLIM];
	param_t* pdu_window_uv = pdup4_params_a[P4_WINDOW_OV];
	param_t* pdu_window_ov = pdup4_params_a[P4_WINDOW_OV];
	param_t* pdu_window_hyst = pdup4_params_a[P4_WINDOW_HYST];


	csp_timestamp_t zeroTimestamp = {0, 0};

	*pdu_ch_on_init->timestamp = zeroTimestamp;
	*pdu_ch_type->timestamp = zeroTimestamp;
	*pdu_ch_curlim->timestamp = zeroTimestamp;
	*pdu_window_uv->timestamp = zeroTimestamp;
	*pdu_window_ov->timestamp = zeroTimestamp;
	*pdu_window_hyst->timestamp = zeroTimestamp;

	param_queue_t pdu_queue;
	uint8_t pdu_queue_buf[PARAM_SERVER_MTU-2];
	param_queue_init(&pdu_queue, pdu_queue_buf, PARAM_SERVER_MTU-2, 0, PARAM_QUEUE_TYPE_GET, CSP_VERSION);

	param_queue_add(&pdu_queue, pdu_ch_on_init, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_type, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_curlim, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_window_uv, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_window_ov, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_window_hyst, INDEX_ALL, NULL);
	
	int pdu_result = param_pull_queue(&pdu_queue, CSP_PRIO_HIGH, VERBOSE, PDU, TIMEOUT);

	if (pdu_result == 0)
	{
		memcpy(&settings->ch_on_init, pdu_ch_on_init->addr, sizeof(settings->ch_on_init));
		memcpy(&settings->ch_type, pdu_ch_type->addr, sizeof(settings->ch_type));
		memcpy(&settings->ch_curlim, pdu_ch_curlim->addr, sizeof(settings->ch_curlim));
		memcpy(&settings->window_ov, pdu_window_uv->addr, sizeof(settings->window_ov));
		memcpy(&settings->window_uv, pdu_window_ov->addr, sizeof(settings->window_uv));
		memcpy(&settings->window_hyst, pdu_window_hyst->addr, sizeof(settings->window_hyst));

		return 1;
	}
	else { return 0; }
}

int pdup4_get_info(eps_pdup4_info_t* info)
{
	param_t* pdu_gndwdt = pdup4_params_a[P4_GNDWDT];
	param_t* pdu_boot_cnt = pdup4_params_a[P4_BOOT_CNT];
	param_t* pdu_ch_on = pdup4_params_a[P4_CH_ON];
	param_t* pdu_ch_on_actual = pdup4_params_a[P4_CH_ON_ACTUAL];
	param_t* pdu_ch_uptime = pdup4_params_a[P4_CH_UPTIME];
	param_t* pdu_ch_downtime = pdup4_params_a[P4_CH_DOWNTIME];

	csp_timestamp_t zeroTimestamp = {0, 0};

	*pdu_gndwdt->timestamp = zeroTimestamp;
	*pdu_boot_cnt->timestamp = zeroTimestamp;
	*pdu_ch_on->timestamp = zeroTimestamp;
	*pdu_ch_on_actual->timestamp = zeroTimestamp;
	*pdu_ch_uptime->timestamp = zeroTimestamp;
	*pdu_ch_downtime->timestamp = zeroTimestamp;

	param_queue_t pdu_queue;
	uint8_t pdu_queue_buf[PARAM_SERVER_MTU-2];
	param_queue_init(&pdu_queue, pdu_queue_buf, PARAM_SERVER_MTU-2, 0, PARAM_QUEUE_TYPE_GET, CSP_VERSION);

	param_queue_add(&pdu_queue, pdu_gndwdt, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_boot_cnt, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_on, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_on_actual, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_uptime, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_downtime, INDEX_ALL, NULL);
	
	int pdu_result = param_pull_queue(&pdu_queue, CSP_PRIO_HIGH, VERBOSE, PDU, TIMEOUT);

	if (pdu_result == 0)
	{
		memcpy(&info->gndwdt, pdu_gndwdt->addr, sizeof(info->gndwdt));
		memcpy(&info->boot_cnt, pdu_boot_cnt->addr, sizeof(info->boot_cnt));
		memcpy(&info->ch_on, pdu_ch_on->addr, sizeof(info->ch_on));
		memcpy(&info->ch_on_actual, pdu_ch_on_actual->addr, sizeof(info->ch_on_actual));
		memcpy(&info->ch_uptime, pdu_ch_uptime->addr, sizeof(info->ch_uptime));
		memcpy(&info->ch_downtime, pdu_ch_downtime->addr, sizeof(info->ch_downtime));

		return 1;
	}
	else { return 0; }
}

int try_pull_too_many_params()
{
	param_t* pdu_ch_on_init = pdup4_params_a[P4_CH_ON_INIT];
	param_t* pdu_ch_uptime = pdup4_params_a[P4_CH_UPTIME];
	param_t* pdu_ch_downtime = pdup4_params_a[P4_CH_DOWNTIME];
	param_t* pdu_ch_type = pdup4_params_a[P4_CH_TYPE];
	param_t* pdu_ch_on = pdup4_params_a[P4_CH_ON];
	param_t* pdu_ch_on_actual = pdup4_params_a[P4_CH_ON_ACTUAL];
	param_t* pdu_ch_curlim = pdup4_params_a[P4_CH_CURLIM];
	param_t* pdu_bus_voltage = pdup4_params_a[P4_BUS_VOLTAGE];
	param_t* pdu_gndwdt = pdup4_params_a[P4_GNDWDT];
	param_t* pdu_ch_wdt_initial = pdup4_params_a[P4_CH_WDT_INITIAL];
	param_t* pdu_wdt_on_timer_cur = pdup4_params_a[P4_CH_WDT_ON_TIMER_CUR];
	param_t* pdu_ch_wdt_in = pdup4_params_a[P4_CH_WDT_IN];

	csp_timestamp_t zeroTimestamp = {0, 0};

	*pdu_ch_on_init->timestamp = zeroTimestamp;
	*pdu_ch_uptime->timestamp = zeroTimestamp;
	*pdu_ch_downtime->timestamp = zeroTimestamp;
	*pdu_ch_type->timestamp = zeroTimestamp;
	*pdu_ch_on->timestamp = zeroTimestamp;
	*pdu_ch_on_actual->timestamp = zeroTimestamp;
	*pdu_ch_curlim->timestamp = zeroTimestamp;
	*pdu_bus_voltage->timestamp = zeroTimestamp;
	*pdu_gndwdt->timestamp = zeroTimestamp;
	*pdu_ch_wdt_initial->timestamp = zeroTimestamp;
	*pdu_wdt_on_timer_cur->timestamp = zeroTimestamp;
	*pdu_ch_wdt_in->timestamp = zeroTimestamp;

	param_queue_t pdu_queue;
	uint8_t pdu_queue_buf[PARAM_SERVER_MTU-2];
	param_queue_init(&pdu_queue, pdu_queue_buf, PARAM_SERVER_MTU-2, 0, PARAM_QUEUE_TYPE_GET, CSP_VERSION);

	param_queue_add(&pdu_queue, pdu_ch_on_init, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_uptime, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_downtime, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_type, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_on, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_on_actual, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_curlim, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_bus_voltage, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_gndwdt, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_wdt_initial, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_wdt_on_timer_cur, INDEX_ALL, NULL);
	param_queue_add(&pdu_queue, pdu_ch_wdt_in, INDEX_ALL, NULL);
	
	int pdu_result = param_pull_queue(&pdu_queue, CSP_PRIO_HIGH, VERBOSE, PDU, TIMEOUT);

	return pdu_result == 0 ? 1 : 0;
}

// This function was shamelessly adapted from print_param_value inside param_string.c
// It converts the values inside a parameter into a string that can be printed or sent via a UDP socket
const char* param_value_string(param_t * param, int offset) {
	if (param == NULL) { return NULL; }


	int count = (param->array_size > 0) ? param->array_size : 1;

	/* Treat data and strings as single parameters */
	if (param->type == PARAM_TYPE_DATA || param->type == PARAM_TYPE_STRING)
		count = 1;

	/* If offset is set, adjust count to only display one value */
	if (offset >= 0) { count = 1; }

	/* If offset is unset, start at zero and display all values */
	if (offset < 0) { offset = 0; }

	static char value_str[1024];
    memset(value_str, 0, sizeof(value_str));


	if (count > 1) { strcat(value_str, "["); }

	for(int i = offset; i < offset + count; i++) {
		char value[sizeof(value_str)];

		if((param->timestamp->tv_nsec > 0 && param->timestamp->tv_sec > 0) || *param->node == 0){
			param_value_str(param, i, value, sizeof(value));
			strcat(value_str, value);
		}
		else {
			strcat(value_str, "-");
		}
		if (i + 1 < count) {
			strcat(value_str, " ");
		}
	} //for loop

	if (count > 1) { strcat(value_str, "]"); }


	if (param->unit != NULL && strlen(param->unit)) {
		strcat(value_str, " ");
		strcat(value_str, param->unit);
	}

	int remain = 20 - strlen(value_str);
	while(remain-- > 0) {
		strcat(value_str, " ");
	}

	return value_str;
} // param_value_string