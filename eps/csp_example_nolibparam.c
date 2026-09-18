// csp_example_nolibparam.c
// Simeon Shaffar - April 25, 2025
// This is meant to be a test to see how much the code expands when you cut out the usage of libparam
// Any function or type libparam was using has been thrown into this executable to be all in one place
// 
// It is mostly a learning exercise to see what the minimum amount of code is to pull
// and print it without libparam's abstraction (still using libcsp though)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>

#include <csp/csp.h>
#include <csp/csp_yaml.h>
#include <mpack/mpack.h>

#define PCDU 3
#define BATT 2
#define MPPT 1
#define CSP_VERSION 2
#define VERBOSE 0
#define ARRAY_SIZE 12
#define ARRAY_STEP 0
#define CAN_BITRATE 1000000
#define TIMEOUT 10
#define PRIORITY 1
#define ACK_WITH_PULL 1

/**
 * Global parameter mask
 */
#define PM_READONLY             (1 << 0) //! r: Readonly by any
#define PM_REMOTE               (1 << 1) //! R: Remote parameter
#define PM_CONF                 (1 << 2) //! c: Actual settings, to be modified by a human (excluding network config)
#define PM_TELEM                (1 << 3) //! t: Ready-to-use telemetry, converted to human readable.
#define PM_HWREG                (1 << 4) //! h: Raw-bit-values in external chips
#define PM_ERRCNT               (1 << 5) //! e: Rarely updated error counters (hopefully)
#define PM_SYSINFO              (1 << 6) //! i: Boot information, time
#define PM_SYSCONF              (1 << 7) //! C: Network and time configuration
#define PM_WDT                  (1 << 8) //! w: Crictical watchdog
#define PM_DEBUG                (1 << 9) //! d: Debug flag (enables uart output)
#define PM_CALIB               (1 << 10) //! q: Calibration gains and offsets

#define PM_ATOMIC_WRITE        (1 << 11) //! o: Parameter must be written atomically.
#define PM_PRIO1               (1 << 12) //! q: Priority of parameter for telemetry logging (two bits)
#define PM_PRIO2               (2 << 12) //! q: Priority of parameter for logging and retrieval (two bits)
#define PM_PRIO3               (3 << 12) //! q: Priority of parameter for logging and retrieval (two bits)
#define PM_PRIO_MASK           (3 << 12) //! q: Priority of parameter for logging and retrieval (two bits)

#define PARAM_REMOTE_NODE_IGNORE 16382
#define PARAM_SERVER_MTU 200
#define PARAM_PORT_SERVER 10
#define PARAM_PORT_LIST	12
#define PARAM_FLAG_END (1 << 7) // Second byte on all packets contains flags

#define	SLIST_EMPTY(head)	((head)->slh_first == NULL)
#define	SLIST_FIRST(head)	((head)->slh_first)
#define	SLIST_NEXT(elm, field)	((elm)->field.sle_next)
#define	SLIST_HEAD(name, type)						\
struct name {								\
	struct type *slh_first;	/* first element */			\
}


#ifdef PARAM_HAVE_SYS_QUEUE
static SLIST_HEAD(param_list_head_s, param_s) param_list_head = {};
#endif


typedef enum {
	PARAM_TYPE_UINT8,
	PARAM_TYPE_UINT16,
	PARAM_TYPE_UINT32,
	PARAM_TYPE_UINT64,
	PARAM_TYPE_INT8,
	PARAM_TYPE_INT16,
	PARAM_TYPE_INT32,
	PARAM_TYPE_INT64,
	PARAM_TYPE_XINT8,
	PARAM_TYPE_XINT16,
	PARAM_TYPE_XINT32,
	PARAM_TYPE_XINT64,
	PARAM_TYPE_FLOAT,
	PARAM_TYPE_DOUBLE,
	PARAM_TYPE_STRING,
	PARAM_TYPE_DATA,
	PARAM_TYPE_INVALID,
} param_type_e;

typedef struct vmem_s {
	int type;
	void (*read)(struct vmem_s * vmem, uint32_t addr, void * dataout, int len);
	void (*write)(struct vmem_s * vmem, uint32_t addr, void * datain, int len);
	int (*backup)(struct vmem_s * vmem);
	int (*restore)(struct vmem_s * vmem);
	void * vaddr;
	int size;
	const char *name;
	int big_endian;
	void * driver;
} vmem_t;

typedef struct param_s {

	/* Parameter declaration */
	uint16_t id;
	uint16_t node;
	param_type_e type;
	uint32_t mask;
	char *name;
	char *unit;
	char *docstr;

	/* Storage */
	void * addr;
	struct vmem_s * vmem;
	int array_size;
	int array_step;

	/* Local info */
	void (*callback)(struct param_s * param, int offset);
	uint32_t * timestamp;

#ifdef PARAM_HAVE_SYS_QUEUE
	/* single linked list:
	 * The weird definition format comes from sys/queue.h SLINST_ENTRY() macro */
	struct { struct param_s *sle_next; } next;
#endif


} param_t;

#ifndef PARAM_STORAGE_SIZE
static param_t param_size_set[2] __attribute__((aligned(1)));
#define PARAM_STORAGE_SIZE ((intptr_t) &param_size_set[1] - (intptr_t) &param_size_set[0])
#endif

#define PARAM_DEFINE_REMOTE(_name, _node, _id, _type, _array_size, _array_step, _flags, _physaddr, _docstr) \
	; /* Catch const param defines */ \
	uint32_t _timestamp_##_name = 0; \
	__attribute__((section("param"))) \
	__attribute__((aligned(1))) \
	__attribute__((used)) \
	param_t _name = { \
		.node = _node, \
		.id = _id, \
		.type = _type, \
		.array_size = _array_size, \
		.array_step = _array_step, \
		.name = (char *) #_name, \
		.mask = _flags, \
		.addr = _physaddr, \
		.timestamp = &_timestamp_##_name, \
		.docstr = _docstr, \
	};

void param_get_data(param_t * param, void * outbuf, int len)
{
	if (param->vmem) {
		param->vmem->read(param->vmem, (uint32_t) (intptr_t) param->addr, outbuf, len);
	} else {
		memcpy(outbuf, param->addr, len);
	}
}
	
#define PARAM_GET(_type, _name, _swapfct) \
	_type param_get_##_name##_array(param_t * param, unsigned int i) { \
		if (i > (unsigned int) param->array_size) { \
			return 0; \
		} \
		if (param->vmem) { \
			_type data = 0; \
			param->vmem->read(param->vmem, (uint32_t) (intptr_t) param->addr + i * param->array_step, &data, sizeof(data)); \
			if (param->vmem->big_endian == 1) { \
				data = _swapfct(data); \
			} \
			return data; \
		} else { \
			return *(_type *)(param->addr + i * param->array_step); \
		} \
	} \
	_type param_get_##_name(param_t * param) { \
		return param_get_##_name##_array(param, 0); \
	}

PARAM_GET(uint8_t, uint8, )
PARAM_GET(uint16_t, uint16, be16toh)
PARAM_GET(uint32_t, uint32, be32toh)
PARAM_GET(uint64_t, uint64, be64toh)
PARAM_GET(int8_t, int8, )
PARAM_GET(int16_t, int16, be16toh)
PARAM_GET(int32_t, int32, be32toh)
PARAM_GET(int64_t, int64, be64toh)
PARAM_GET(float, float, )
PARAM_GET(double, double, )

#undef PARAM_GET

void param_get(param_t * param, unsigned int offset, void * value) {
	switch(param->type) {

#define PARAM_GET(casename, name, type) \
	case casename: \
		*(type *) value = param_get_##name##_array(param, offset); \
		break; \

	PARAM_GET(PARAM_TYPE_UINT8, uint8, uint8_t)
	PARAM_GET(PARAM_TYPE_UINT16, uint16, uint16_t)
	PARAM_GET(PARAM_TYPE_UINT32, uint32, uint32_t)
	PARAM_GET(PARAM_TYPE_UINT64, uint64, uint64_t)
	PARAM_GET(PARAM_TYPE_INT8, int8, int8_t)
	PARAM_GET(PARAM_TYPE_INT16, int16, int16_t)
	PARAM_GET(PARAM_TYPE_INT32, int32, int32_t)
	PARAM_GET(PARAM_TYPE_INT64, int64, int64_t)
	PARAM_GET(PARAM_TYPE_XINT8, uint8, uint8_t)
	PARAM_GET(PARAM_TYPE_XINT16, uint16, uint16_t)
	PARAM_GET(PARAM_TYPE_XINT32, uint32, uint32_t)
	PARAM_GET(PARAM_TYPE_XINT64, uint64, uint64_t)
	PARAM_GET(PARAM_TYPE_FLOAT, float, float)
	PARAM_GET(PARAM_TYPE_DOUBLE, double, double)
	case PARAM_TYPE_STRING:
	case PARAM_TYPE_DATA:
		param_get_data(param, value, param->array_size);
		break;
    case PARAM_TYPE_INVALID:
        break;     
	}
    
}

void param_set_data_nocallback(param_t * param, void * inbuf, int len) {
	if (param->vmem) {
		param->vmem->write(param->vmem, (uint32_t) (intptr_t) param->addr, inbuf, len);
	} else {
		memcpy(param->addr, inbuf, len);
	}
}

void param_set_data(param_t * param, void * inbuf, int len) {
	param_set_data_nocallback(param, inbuf, len);
	/* Callback */
	if (param->callback) {
		param->callback(param, 0);
	}
}

void param_set_string(param_t * param, void * inbuf, int len) {
	param_set_data(param, inbuf, len);
	/* Termination */
	if (param->vmem) {
		param->vmem->write(param->vmem, (uint32_t) (intptr_t) param->addr + len, "", 1);
	} else {
		memcpy(param->addr + len , "", 1);
	}
	/* Callback */
	if (param->callback) {
		param->callback(param, 0);
	}
}

#define PARAM_SET(type, name) \
	void param_set_##name(param_t * param, type value); \
	void param_set_##name##_nocallback(param_t * param, type value); \
	void param_set_##name##_array(param_t * param, unsigned int i, type value); \
	void param_set_##name##_array_nocallback(param_t * param, unsigned int i, type value);
PARAM_SET(uint8_t, uint8)
PARAM_SET(uint16_t, uint16)
PARAM_SET(uint32_t, uint32)
PARAM_SET(uint64_t, uint64)
PARAM_SET(int8_t, int8)
PARAM_SET(int16_t, int16)
PARAM_SET(int32_t, int32)
PARAM_SET(int64_t, int64)
PARAM_SET(float, float)
PARAM_SET(double, double)
#undef PARAM_SET

#define PARAM_SET(_type, name_in, _swapfct) \
	void __param_set_##name_in(param_t * param, _type value, bool do_callback, unsigned int i) { \
		if (i > (unsigned int) param->array_size) { \
			return; \
		} \
		if (param->vmem) { \
			if (param->vmem->big_endian == 1) \
				value = _swapfct(value); \
			param->vmem->write(param->vmem, (uint32_t) (intptr_t) param->addr + i * param->array_step, &value, sizeof(_type)); \
		} else { \
			/* Aligned access directly to RAM */ \
			*(_type*)(param->addr + i * param->array_step) = value; \
		} \
		/* Callback */ \
		if ((do_callback == true) && (param->callback)) { \
			param->callback(param, i); \
		} \
	} \
	inline void param_set_##name_in(param_t * param, _type value) \
	{ \
		__param_set_##name_in(param, value, true, 0); \
	} \
	inline void param_set_##name_in##_nocallback(param_t * param, _type value) \
	{ \
		__param_set_##name_in(param, value, false, 0); \
	} \
	inline void param_set_##name_in##_array(param_t * param, unsigned int i, _type value) \
	{ \
		__param_set_##name_in(param, value, true, i); \
	} \
	inline void param_set_##name_in##_array_nocallback(param_t * param, unsigned int i, _type value) \
	{ \
		__param_set_##name_in(param, value, false, i); \
	}

PARAM_SET(uint8_t, uint8, )
PARAM_SET(uint16_t, uint16, htobe16)
PARAM_SET(uint32_t, uint32, htobe32)
PARAM_SET(uint64_t, uint64, htobe64)
PARAM_SET(int8_t, int8, )
PARAM_SET(int16_t, int16, htobe16)
PARAM_SET(int32_t, int32, htobe32)
PARAM_SET(int64_t, int64, htobe64)
PARAM_SET(float, float, )
PARAM_SET(double, double, )

#undef PARAM_SET

void param_set(param_t * param, unsigned int offset, void * value) {
	switch(param->type) {

#define PARAM_SET(casename, name, type) \
	case casename: \
		param_set_##name##_array(param, offset, *(type *) value); \
		break; \

	PARAM_SET(PARAM_TYPE_UINT8, uint8, uint8_t)
	PARAM_SET(PARAM_TYPE_UINT16, uint16, uint16_t)
	PARAM_SET(PARAM_TYPE_UINT32, uint32, uint32_t)
	PARAM_SET(PARAM_TYPE_UINT64, uint64, uint64_t)
	PARAM_SET(PARAM_TYPE_INT8, int8, int8_t)
	PARAM_SET(PARAM_TYPE_INT16, int16, int16_t)
	PARAM_SET(PARAM_TYPE_INT32, int32, int32_t)
	PARAM_SET(PARAM_TYPE_INT64, int64, int64_t)
	PARAM_SET(PARAM_TYPE_XINT8, uint8, uint8_t)
	PARAM_SET(PARAM_TYPE_XINT16, uint16, uint16_t)
	PARAM_SET(PARAM_TYPE_XINT32, uint32, uint32_t)
	PARAM_SET(PARAM_TYPE_XINT64, uint64, uint64_t)
	PARAM_SET(PARAM_TYPE_FLOAT, float, float)
	PARAM_SET(PARAM_TYPE_DOUBLE, double, double)
	case PARAM_TYPE_STRING:
		param_set_data(param, value, strlen(value) + 1);
		break;
	case PARAM_TYPE_DATA:
		param_set_data(param, value, param->array_size);
		break;
    case PARAM_TYPE_INVALID:
        break;
	}
}


typedef enum {

  /* V1 */
  PARAM_PULL_REQUEST 	= 0,
  PARAM_PULL_RESPONSE = 1,
  PARAM_PUSH_REQUEST 	= 2,
  PARAM_PUSH_RESPONSE = 3,        // Push response is an ack, and is same for v1 and v2.
  PARAM_PULL_ALL_REQUEST = 4,     // Pull all request is same for v1 and v2.

  /* V2 */
  PARAM_PULL_REQUEST_V2  = 5,
  PARAM_PULL_RESPONSE_V2 = 6,
  PARAM_PUSH_REQUEST_V2  = 7,
  PARAM_PULL_ALL_REQUEST_V2 = 8,
  PARAM_SCHEDULE_PUSH = 9,
  //PARAM_SCHEDULE_PULL = 10,
  PARAM_SCHEDULE_ADD_RESPONSE = 11, // same response for adding push or pull schedule
  PARAM_SCHEDULE_SHOW_REQUEST = 12,
  PARAM_SCHEDULE_SHOW_RESPONSE = 13,
  PARAM_SCHEDULE_LIST_REQUEST = 14,
  PARAM_SCHEDULE_LIST_RESPONSE = 15,
  PARAM_SCHEDULE_RM_ALL_REQUEST = 16,
  PARAM_SCHEDULE_RM_REQUEST = 17,
  PARAM_SCHEDULE_RM_RESPONSE = 18,
  PARAM_SCHEDULE_RESET_REQUEST = 19,
  PARAM_SCHEDULE_RESET_RESPONSE = 20,
  PARAM_COMMAND_ADD_REQUEST = 21,
  PARAM_COMMAND_ADD_RESPONSE = 22,
  PARAM_COMMAND_SHOW_REQUEST = 23,
  PARAM_COMMAND_SHOW_RESPONSE = 24,
  PARAM_COMMAND_LIST_REQUEST = 25,
  PARAM_COMMAND_LIST_RESPONSE = 26,
  PARAM_COMMAND_RM_REQUEST = 27,
  PARAM_COMMAND_RM_ALL_REQUEST = 28,
  PARAM_COMMAND_RM_RESPONSE = 29,
  PARAM_COMMAND_EXEC_REQUEST = 30,
  PARAM_COMMAND_EXEC_RESPONSE = 31,
  PARAM_SCHEDULE_COMMAND_REQUEST = 32,
  PARAM_PUSH_REQUEST_V2_HWID = 33,

} param_packet_type_e;


typedef enum {
	PARAM_QUEUE_TYPE_GET,
	PARAM_QUEUE_TYPE_SET,
	PARAM_QUEUE_TYPE_EMPTY,
} param_queue_type_e;

typedef struct param_queue_s {
	char *buffer;
	uint16_t buffer_size;
	uint16_t used;
	uint8_t version;
	uint8_t type;
	char name[20];

	/* State used by serializer */
	uint16_t last_node;
	uint32_t last_timestamp;
} param_queue_t;


// Define some symbols for libparam just to get the program to compile
void * router_task(void *) {
  while(1) {
    csp_route_work();
  }
}

void param_queue_init(param_queue_t *queue, void *buffer, int buffer_size, int used, param_queue_type_e type, int version) {
	queue->buffer = buffer;
	queue->buffer_size = buffer_size;
	queue->type = type;
	queue->used = used;
	queue->version = version;
	queue->last_timestamp = 0;
}

typedef struct param_list_iterator_s {
	int phase;							// Hybrid iterator has multiple phases (0 == Static, 1 == Dynamic List)
	param_t * element;
} param_list_iterator;

param_t * param_list_iterate(param_list_iterator * iterator) {

	/**
	 * GNU Linker symbols. These will be autogenerate by GCC when using
	 * __attribute__((section("param"))
	 */
	__attribute__((weak)) extern param_t __start_param;
	__attribute__((weak)) extern param_t __stop_param;

	/* First element */
	if (iterator->element == NULL) {

		/* Static */
		if ((&__start_param != NULL) && (&__start_param != &__stop_param)) {
			iterator->phase = 0;
			iterator->element = &__start_param;
		} else {
			iterator->phase = 1;
#ifdef PARAM_HAVE_SYS_QUEUE
			iterator->element = SLIST_FIRST(&param_list_head);
#endif
		}

		return iterator->element;
	}

	/* Static phase */
	if (iterator->phase == 0) {

		/* Increment in static memory */
		iterator->element = (param_t *)(intptr_t)((char *)iterator->element + PARAM_STORAGE_SIZE);

		/* Check if we are still within the bounds of the static memory area */
		if (iterator->element < &__stop_param)
			return iterator->element;

		/* Otherwise, switch to dynamic phase */
		iterator->phase = 1;
#ifdef PARAM_HAVE_SYS_QUEUE
		iterator->element = SLIST_FIRST(&param_list_head);
		return iterator->element;
#else
		return NULL;
#endif
	}

#ifdef PARAM_HAVE_SYS_QUEUE
	/* Dynamic phase */
	if (iterator->phase == 1) {

		iterator->element = SLIST_NEXT(iterator->element, next);
		return iterator->element;
	}
#endif

	return NULL;

}

param_t * param_list_find_id(int node, int id) {
	
	if (node < 0)
		node = 0;

	param_t * found = NULL;
	param_t * param;
	param_list_iterator i = {};

	while ((param = param_list_iterate(&i)) != NULL) {

		if (param->node != node)
			continue;

		if (param->id == id) {
			found = param;
			break;
		}

		continue;
	}

	return found;
}

static inline uint16_t param_get_short_id(param_t * param, unsigned int isarray, unsigned int reserved) {
	uint16_t node = param->node;
	return (node << 11) | ((isarray & 0x1) << 10) | ((reserved & 0x1) << 2) | ((param->id) & 0x1FF);
}
static inline uint8_t param_parse_short_id_flag_isarray(uint16_t short_id) {
	return (short_id >> 10) & 0x1;
}

static inline uint8_t param_parse_short_id_node(uint16_t short_id) {
	return (short_id >> 11) & 0x1F;
}

static inline uint16_t param_parse_short_id_paramid(uint16_t short_id) {
	return short_id & 0x1FF;
}

void param_serialize_id(mpack_writer_t *writer, param_t *param, int offset, param_queue_t *queue) {

	if (queue->version == 1) {

		if (offset >= 0) {
			mpack_write_u16(writer, param_get_short_id(param, 1, 0));
			char _offset = offset;
			mpack_write_bytes(writer, &_offset, 1);
		} else {
			mpack_write_u16(writer, param_get_short_id(param, 0, 0));
		}

	} else {

		int node = param->node;
		uint32_t timestamp = *param->timestamp;
		int array_flag = (offset >= 0) ? 1 : 0;
		int node_flag = (queue->last_node != node) ? 1 : 0;
		int timestamp_flag = (queue->last_timestamp != timestamp) ? 1 : 0;
		int extendedid_flag = (param->id > 0x3ff) ? 1 : 0;

		uint16_t header = array_flag << 15 | node_flag << 14 | timestamp_flag << 13 | extendedid_flag << 12 | (param->id & 0x3ff);
		header = htobe16(header);
		mpack_write_u16(writer, header);

		if (array_flag) {
			char _offset = offset;
			mpack_write_bytes(writer, &_offset, 1);
		}

		if (node_flag) {
			queue->last_node = node;
			uint16_t _node = htobe16(node);
			mpack_write_bytes(writer, (char*) &_node, 2);
		}

		if (timestamp_flag) {
			queue->last_timestamp = timestamp;
			uint32_t _timestamp = htobe32(timestamp);
			mpack_write_bytes(writer, (char*) &_timestamp, 4);
		}

		if (extendedid_flag) {
			char _extendedid = (param->id&0xfc00) >> 8;
			mpack_write_bytes(writer, &_extendedid, 1);
		}

	}

}

void param_deserialize_id(mpack_reader_t *reader, int *id, int *node, long unsigned int *timestamp, int *offset, param_queue_t *queue) {

	if (queue->version == 1) {

		uint16_t short_id = mpack_expect_u16(reader);

		if (mpack_reader_error(reader) != mpack_ok)
			return;

		if (param_parse_short_id_flag_isarray(short_id)) {
			char _offset;
			mpack_read_bytes(reader, &_offset, 1);
			*offset = _offset;
		}

		*id = param_parse_short_id_paramid(short_id);
		*node = param_parse_short_id_node(short_id);

	} else {

		uint16_t header = mpack_expect_u16(reader);
		if (mpack_reader_error(reader) != mpack_ok)
			return;

		header = be16toh(header);
		int array_flag = header & 0x8000;
		int node_flag = header & 0x4000;
		int timestamp_flag = header & 0x2000;
		int extendedid_flag = header & 0x1000;
		*id = header & 0x3ff;

		if (array_flag) {
			char _offset;
			mpack_read_bytes(reader, &_offset, 1);
			*offset = _offset;
		}

		if (node_flag) {
			uint16_t _node;
			mpack_read_bytes(reader, (char*) &_node, 2);
			_node = be16toh(_node);
			*node = _node;
			queue->last_node = _node;
		} else {
			*node = queue->last_node;
		}


		if (timestamp_flag) {
			uint32_t _timestamp;
			mpack_read_bytes(reader, (char*) &_timestamp, 4);
			_timestamp = be32toh(_timestamp);
			*timestamp = _timestamp;
			queue->last_timestamp = _timestamp;
		} else {
			*timestamp = queue->last_timestamp;
		}

		if (extendedid_flag) {
			char _extendedid;
			mpack_read_bytes(reader, &_extendedid, 1);
			*id |= ((uint16_t)_extendedid << 8)&0xFFFF;
		}

	}

}

void param_deserialize_from_mpack_to_param(void * context, void * queue, param_t * param, int offset, mpack_reader_t * reader) {

	if (offset < 0)
		offset = 0;

	int count = 1;

	/* Inspect for array */
	mpack_tag_t tag = mpack_peek_tag(reader);
	if (tag.type == mpack_type_array) {
		count = mpack_expect_array(reader);
	}

	for (int i = offset; i < offset + count; i++) {

		switch (param->type) {
		case PARAM_TYPE_UINT8:
		case PARAM_TYPE_XINT8:
			param_set_uint8_array(param, i, (uint8_t) mpack_expect_uint(reader)); break;
		case PARAM_TYPE_UINT16:
		case PARAM_TYPE_XINT16:
			param_set_uint16_array(param, i, (uint16_t) mpack_expect_uint(reader)); break;
		case PARAM_TYPE_UINT32:
		case PARAM_TYPE_XINT32:
			param_set_uint32_array(param, i, (uint32_t) mpack_expect_uint(reader)); break;
		case PARAM_TYPE_UINT64:
		case PARAM_TYPE_XINT64:
			param_set_uint64_array(param, i, mpack_expect_u64(reader)); break;
		case PARAM_TYPE_INT8:
			param_set_int8_array(param, i, (int8_t) mpack_expect_int(reader)); break;
		case PARAM_TYPE_INT16:
			param_set_int16_array(param, i, (int16_t) mpack_expect_int(reader)); break;
		case PARAM_TYPE_INT32:
			param_set_int32_array(param, i, (int32_t) mpack_expect_int(reader)); break;
		case PARAM_TYPE_INT64:
			param_set_int64_array(param, i, mpack_expect_i64(reader)); break;
#if MPACK_FLOAT
		case PARAM_TYPE_FLOAT:
			param_set_float_array(param, i, mpack_expect_float(reader)); break;
		case PARAM_TYPE_DOUBLE:
			param_set_double_array(param, i, mpack_expect_double(reader)); break;
#endif
		case PARAM_TYPE_STRING: {
			int len = mpack_expect_str(reader);
			if (len == 0) {
				param_set_string(param, "", 1);
			} else {
				param_set_string(param, (void *) reader->data, len);
			}
			reader->data += len;
			mpack_done_str(reader);
			break;
		}
		case PARAM_TYPE_DATA: {
			int len = mpack_expect_bin(reader);
			param_set_data(param, (void *) reader->data, len);
			reader->data += len;
			mpack_done_bin(reader);
			break;
		}

		default:
			mpack_discard(reader);
			break;
		}

		if (mpack_reader_error(reader) != mpack_ok) {
			return;
		}

	}

}

/* External hooks to get atomic writes */
extern __attribute__((weak)) void param_enter_critical(void);
extern __attribute__((weak)) void param_exit_critical(void);

int param_queue_apply(param_queue_t *queue, int apply_local, int from) {
	int return_code = 0;
	int atomic_write = 0;

	mpack_reader_t reader;
	mpack_reader_init_data(&reader, queue->buffer, queue->used);
	while(reader.data < reader.end) {
		int id, node, offset = -1;
		long unsigned int timestamp = 0;
		param_deserialize_id(&reader, &id, &node, &timestamp, &offset, queue);

		/* If the from address is set, and the nodeid is 0, substitue with the source address */
		if (node == 0)
			node = from;

		/* First we search on the specified node in the request or response */
		param_t * param = param_list_find_id(node, id);

		if (!param) {

			/* If the apply_local flag is set (true for push messages) and the parameter not found,
			   try a search in the local parameters: This enables the re-use of packets from one node
			   to another. But be very carefull with this. */
			if (apply_local == 1) {
				node = 0;
			}

			param = param_list_find_id(node, id);
		}

		if (param) {
			if ((param->mask & PM_ATOMIC_WRITE) && (atomic_write == 0)) {
				atomic_write = 1;
				if (param_enter_critical)
					param_enter_critical();
			}

			*param->timestamp = timestamp;

			param_deserialize_from_mpack_to_param(NULL, queue, param, offset, &reader);
		} else {
			// We couldn't find all parameters. Skip this one.
			return_code = -1;

			mpack_tag_t tag = mpack_read_tag(&reader);
			if (mpack_reader_error(&reader) != mpack_ok) {
				break;
			}

			// TODO: Skip content
			bool valid = true;

			switch (tag.type) {
    		case mpack_type_str:
    		case mpack_type_bin:
    			if (reader.end - reader.data >= tag.v.l) {
	    			mpack_skip_bytes(&reader, tag.v.l);
	    		} else {
    				valid = false;
	    			break;
	    		}

	    		if (tag.type == mpack_type_str) {
	    			mpack_done_str(&reader);
	    		} else if (tag.type == mpack_type_bin) {
	    			mpack_done_bin(&reader);
	    		}
    			break;
    		case mpack_type_array:
    			for (int i = 0; i < tag.v.n; i++) {
					mpack_read_tag(&reader);
					if (mpack_reader_error(&reader) != mpack_ok) {
						valid = false;
						break;
					}
    			}
    			if (valid) {
	    			mpack_done_array(&reader);
    			}
    			break;
    		case mpack_type_map:
    			break;
    		default:
    			break;
			}

		}
	}

	if (atomic_write) {
		if (param_exit_critical)
			param_exit_critical();
	}

	return return_code;
}

static void param_transaction_callback_pull(csp_packet_t *response, int verbose, int version, void * context) {

	int from = response->id.src;

	param_queue_t queue;
	param_queue_init(&queue, &response->data[2], response->length - 2, response->length - 2, PARAM_QUEUE_TYPE_SET, version);
	queue.last_node = response->id.src;

  if (verbose) { csp_print("param transaction verbose, context: %d", context) }

	/* Write data to local memory */
	param_queue_apply(&queue, 0, from);

	csp_buffer_free(response);
}

typedef void (*param_transaction_callback_f)(csp_packet_t *response, int verbose, int version, void * context);
int param_transaction(csp_packet_t *packet, int host, int timeout, param_transaction_callback_f callback, int verbose, int version, void * context) {

	//csp_hex_dump("transaction", packet->data, packet->length);

	/* Parameters can be setup with a special nodeid, which caused all transaction to be ignored
	   and return failure immediately */
	if (host == PARAM_REMOTE_NODE_IGNORE) {
		csp_buffer_free(packet);
		return -1;
	}

	csp_conn_t * conn = csp_connect(CSP_PRIO_HIGH, host, PARAM_PORT_SERVER, 0, CSP_O_CRC32);
	if (conn == NULL) {
		printf("param transaction failure\n");
		csp_buffer_free(packet);
		return -1;
	}

	csp_send(conn, packet);

	if (timeout == -1) {
		printf("param transaction failure\n");
		csp_close(conn);
		return -1;
	}

	int result = -1;
	while((packet = csp_read(conn, timeout)) != NULL) {

		int end = (packet->data[1] == PARAM_FLAG_END);

		//csp_hex_dump("response", packet->data, packet->length);

		if (callback) {
			callback(packet, verbose, version, context);
		} else {
			csp_buffer_free(packet);
		}

		if (end) {
			result = 0;
			break;
		}

	}

	//printf("Successful param transaction, result: %d\n", result);
	csp_close(conn);
	return result;
}

int param_serialize_to_mpack(param_t * param, int offset, mpack_writer_t * writer, void * value, param_queue_t * queue) {

	/* Remember the initial position if we need to abort later due to buffer full */
	char * init_pos = writer->position;

	param_serialize_id(writer, param, offset, queue);

	if (mpack_writer_error(writer) != mpack_ok)
		return -1;


	int count = (param->array_size > 0) ? param->array_size : 1;

	/* Treat data and strings as single parameters */
	if (param->type == PARAM_TYPE_DATA || param->type == PARAM_TYPE_STRING)
		count = 1;

	/* If offset is set, adjust count to only display one value */
	if (offset >= 0) {
		count = 1;
	}

	/* If offset is unset, start at zero and display all values */
	if (offset < 0) {
		offset = 0;
	}

	if (count > 1) {
		mpack_start_array(writer, count);
	}

	for(int i = offset; i < offset + count; i++) {

		switch (param->type) {
		case PARAM_TYPE_UINT8:
		case PARAM_TYPE_XINT8:
			if (value) {
				mpack_write_uint(writer, *(uint8_t *) value);
			} else {
				mpack_write_uint(writer, param_get_uint8_array(param, i));
			}
			break;
		case PARAM_TYPE_UINT16:
		case PARAM_TYPE_XINT16:
			if (value) {
				mpack_write_uint(writer, *(uint16_t *) value);
			} else {
				mpack_write_uint(writer, param_get_uint16_array(param, i));
			}
			break;
		case PARAM_TYPE_UINT32:
		case PARAM_TYPE_XINT32:
			if (value) {
				mpack_write_uint(writer, *(uint32_t *) value);
			} else {
				mpack_write_uint(writer, param_get_uint32_array(param, i));
			}
			break;
		case PARAM_TYPE_UINT64:
		case PARAM_TYPE_XINT64:
			if (value) {
				mpack_write_uint(writer, *(uint64_t *) value);
			} else {
				mpack_write_uint(writer, param_get_uint64_array(param, i));
			}
			break;
		case PARAM_TYPE_INT8:
			if (value) {
				mpack_write_int(writer, *(int8_t *) value);
			} else {
				mpack_write_int(writer, param_get_int8_array(param, i));
			}
			break;
		case PARAM_TYPE_INT16:
			if (value) {
				mpack_write_int(writer, *(int16_t *) value);
			} else {
				mpack_write_int(writer, param_get_int16_array(param, i));
			}
			break;
		case PARAM_TYPE_INT32:
			if (value) {
				mpack_write_int(writer, *(int32_t *) value);
			} else {
				mpack_write_int(writer, param_get_int32_array(param, i));
			}
			break;
		case PARAM_TYPE_INT64:
			if (value) {
				mpack_write_int(writer, *(int64_t *) value);
			} else {
				mpack_write_int(writer, param_get_int64_array(param, i));
			}
			break;
#if MPACK_FLOAT
		case PARAM_TYPE_FLOAT:
			if (value) {
				mpack_write_float(writer, *(float *) value);
			} else {
				mpack_write_float(writer, param_get_float_array(param, i));
			}
			break;
		case PARAM_TYPE_DOUBLE:
			if (value) {
				mpack_write_double(writer, *(double *) value);
			} else {
				mpack_write_double(writer, param_get_double_array(param, i));
			}
			break;
#endif
		case PARAM_TYPE_STRING: {
			size_t len;
			if (value) {
				len = strnlen(value, param->array_size);

				mpack_start_str(writer, len);

				if (writer->position + len > writer->end) {
					writer->error = mpack_error_too_big;
					break;
				}

				memcpy(writer->position, (char *) value, len);

			} else {
				char tmp[param->array_size];
				param_get_data(param, tmp, param->array_size);
				len = strnlen(tmp, param->array_size);

				mpack_start_str(writer, len);

				if (writer->position + len > writer->end) {
					writer->error = mpack_error_too_big;
					break;
				}

				memcpy(writer->position, tmp, len);

			}
			writer->position += len;
			mpack_finish_str(writer);
			break;
		}

		case PARAM_TYPE_DATA:

			mpack_start_bin(writer, param->array_size);

			unsigned int size = (param->array_size > 0) ? param->array_size : 1;
			if (writer->position + size > writer->end) {
				writer->error = mpack_error_too_big;
				break;
			}

			if (value) {
				memcpy(writer->position, value, size);
			} else {
				param_get_data(param, writer->position, size);
			}
			writer->position += param->array_size;
			mpack_finish_bin(writer);
			break;

		default:
			break;
		}

		if (mpack_writer_error(writer) != mpack_ok) {
			writer->position = init_pos;
			return -1;
		}

	}

	if (count > 1) {
		mpack_finish_array(writer);
	}

	return 0;

}

int param_queue_add(param_queue_t *queue, param_t *param, int offset, void *value) {

	/* Ensure we always send nodeid on the first element of the queue */
	if (queue->used == 0) {
		queue->last_node = UINT16_MAX;
	}

	if ((queue->type == PARAM_QUEUE_TYPE_GET) && (value != NULL)) {
		printf("Cannot mix GET/SET commands\n");
		printf("Queue type %u value %p\n", queue->type, value);
		return -1;
	}

	mpack_writer_t writer;
	mpack_writer_init(&writer, queue->buffer, queue->buffer_size);
	writer.position = queue->buffer + queue->used;
	if (queue->type == PARAM_QUEUE_TYPE_SET) {
		param_serialize_to_mpack(param, offset, &writer, value, queue);
	} else {
		param_serialize_id(&writer, param, offset, queue);
	}
	if (mpack_writer_error(&writer) != mpack_ok) {
		return -1;
	}
	queue->used = mpack_writer_buffer_used(&writer);
	return 0;
}

void init_csp_barebones() {
  // Generalized csp init function from libcsp
  csp_init();
  printf("initialized csp\n");

  // Use the yaml file to initialize the CAN connection as CSP's interface
  char * yamlname = "/etc/SMALLSAT/eps/can.yaml";
	unsigned int dfl_addr = 0;
	csp_yaml_init(yamlname, &dfl_addr);
  printf("Initialized the yaml\n");

  // Set up a pthread for the router
  // CSP basically always requires that you intialize a second thread to do routing
  // They require that a user does it manually in their code and not in csp_init() for some reason
  static pthread_t router_handle;
	pthread_create(&router_handle, NULL, &router_task, NULL);

  printf("Started the pthread for the router, csp initialized\n");
}

static uint16_t _pcdu_temp = 0;
PARAM_DEFINE_REMOTE(pcdu_temp, PCDU, 30, PARAM_TYPE_UINT16, 1, ARRAY_STEP, PM_CONF, &_pcdu_temp, "");
static uint8_t _pcdu_dfl_on[12] = {0};
PARAM_DEFINE_REMOTE(pcdu_dfl_on, PCDU, 230, PARAM_TYPE_UINT8, 12, 1, PM_CONF, &_pcdu_dfl_on, "");


int main() {
  init_csp_barebones();

  //Spams all telemetry
  while(1) {
    csp_packet_t * packet = csp_buffer_get(PARAM_SERVER_MTU);
    if (packet == NULL)
      return -1;

    packet->data[0] = PARAM_PULL_REQUEST_V2;
    packet->data[1] = 0;

    param_queue_t queue;
    param_queue_init(&queue, &packet->data[2], PARAM_SERVER_MTU - 2, 0, PARAM_QUEUE_TYPE_GET, CSP_VERSION);
    param_queue_add(&queue, &pcdu_temp, -1, NULL);

    packet->length = queue.used + 2;
	param_transaction(packet, PCDU, TIMEOUT, param_transaction_callback_pull, VERBOSE, CSP_VERSION, NULL);


    printf("pcdu_temp: %d\n", _pcdu_temp);

    sleep(1);
  }

  return 0;
}