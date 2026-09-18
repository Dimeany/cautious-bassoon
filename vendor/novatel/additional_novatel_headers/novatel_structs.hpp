#ifndef SMALLSAT_NOVATEL_MESSAGE_DEFINITIONS_HPP
#define SMALLSAT_NOVATEL_MESSAGE_DEFINITIONS_HPP

#ifdef PASSTHROUGH
   #undef PASSTHROUGH // Fix name collision in wingdi.h (included by spdlog)
#endif

#include <cstdint>
#include "udp.h"

namespace gps {

#pragma pack(1)

struct PASSAUX
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct FILETRANSFER
{
	int32_t file_transfer_operation_enum;
	uint8_t file_name[128];
};

struct TRANSFERPORTSTATUS
{
	int32_t usb_detection_type;
	int32_t usb_mode;
};

struct LOG
{
	int32_t log_port_address;
	uint32_t message_id;
	int32_t trigger;
	double on_time;
	double offset;
	int32_t hold;
};

struct INTERFACEMODE
{
	int32_t port;
	int32_t receive_interface_mode;
	int32_t transmit_interface_mode;
	int32_t responses;
};

struct COM
{
	int32_t port;
	uint32_t baud_rate;
	int32_t parity;
	uint32_t data_bits;
	uint32_t stop_bits;
	int32_t hand_shaking;
	int32_t echo;
	int32_t breaks;
};

struct LOGLIST_log_list
{
	int32_t log_port_address;
	uint32_t message_id;
	int32_t trigger;
	double on_time;
	double offset;
	int32_t hold;
};

struct LOGLIST
{
	uint32_t log_list_arraylength;
	LOGLIST_log_list log_list[80];
};

struct RESET
{
	uint32_t delay;
};

struct SAVECONFIG
{
};

struct FRESET
{
	int32_t type;
};

struct MODEL
{
	uint8_t model_name[16];
};

struct UNLOG
{
	int32_t log_port_address;
	uint32_t message_id;
};

struct VERSION_versions
{
	int32_t component_type;
	uint8_t model_name[16];
	uint8_t psn[16];
	uint8_t hardware_version[16];
	uint8_t software_version[16];
	uint8_t boot_version[16];
	uint8_t compile_date[12];
	uint8_t compile_time[12];
};

struct VERSION
{
	uint32_t versions_arraylength;
	VERSION_versions versions[20];
};

struct UNLOGALL
{
	int32_t port;
	bool remove_held;
};

struct AUTH
{
	int32_t auth_state;
	uint8_t auth_data1[16];
	uint8_t auth_data2[16];
	uint8_t auth_data3[16];
	uint8_t auth_data4[16];
	uint8_t auth_data5[16];
	uint8_t model_name[16];
	uint8_t expiry_date[7];
};

struct PORTSTATS_port_statistics
{
	int32_t port;
	uint32_t rx_chars;
	uint32_t tx_chars;
	uint32_t good_rx_chars;
	uint32_t dropped_chars;
	uint32_t interrupts;
	uint32_t breaks;
	uint32_t parity_errors;
	uint32_t framing_errors;
	uint32_t over_runs;
};

struct PORTSTATS
{
	uint32_t port_statistics_arraylength;
	PORTSTATS_port_statistics port_statistics[33];
};

struct RXSTATUS_status_words
{
	uint32_t status_word;
	uint32_t priority_mask;
	uint32_t event_set_mask;
	uint32_t event_clear_mask;
};

struct RXSTATUS
{
	uint32_t rx_error;
	uint32_t status_words_arraylength;
	RXSTATUS_status_words status_words[5];
};

struct RXSTATUSEVENT
{
	int32_t word;
	uint32_t bit_position;
	int32_t event;
	uint8_t description[32];
};

struct STATUSCONFIG
{
	int32_t type;
	int32_t word;
	uint32_t mask;
};

struct ANTENNAPOWER
{
	int32_t antenna_power;
};

struct RXCONFIG
{
	uint32_t message_id;
	uint32_t message_length;
	uint8_t message_buffer[450];
};

struct SEND
{
	int32_t port_address;
	uint8_t text[2000];
};

struct SENDHEX
{
	int32_t port_address;
	uint32_t text_arraylength;
	uint8_t text[2000];
};

struct NVMRESTORE
{
};

struct VALIDMODELS_models
{
	uint8_t model[16];
	uint32_t expiry_year;
	uint32_t expiry_month;
	uint32_t expiry_day;
};

struct VALIDMODELS
{
	uint32_t models_arraylength;
	VALIDMODELS_models models[24];
};

struct PASSCOM1
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSCOM2
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSCOM3
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct COMCONFIG_com_config
{
	int32_t port;
	uint32_t baud;
	int32_t parity;
	uint32_t data_bits;
	uint32_t stop_bits;
	int32_t handshake;
	int32_t echo;
	int32_t breaks;
	int32_t rx_type;
	int32_t tx_type;
	int32_t responses;
};

struct COMCONFIG
{
	uint32_t com_config_arraylength;
	COMCONFIG_com_config com_config[53];
};

struct PASSXCOM1
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSXCOM2
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct COMCONTROL
{
	int32_t port;
	int32_t signal;
	int32_t control;
};

struct SOFTLOADCOMMIT
{
	int32_t src;
};

struct SOFTLOADRESET
{
	int32_t src;
};

struct SOFTLOADSREC
{
	uint8_t srec[515];
	int32_t src;
};

struct PASSUSB1
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSUSB2
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSUSB3
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSXCOM3
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct NMEATALKER
{
	int32_t talker_id;
};

struct CANCONFIG
{
	int32_t can_port;
	int32_t on_off;
	int32_t bitrate;
};

struct TUNNELESCAPE
{
	int32_t enable;
	uint32_t tunnel_escape_arraylength;
	uint8_t tunnel_escape[8];
};

struct HWMONITOR_measurements
{
	float value;
	uint32_t status;
};

struct HWMONITOR
{
	uint32_t measurements_arraylength;
	HWMONITOR_measurements measurements[32];
};

struct CONFIGCODE
{
	int32_t config_state;
	uint8_t data1[15];
	uint8_t data2[15];
	uint8_t data3[15];
	uint8_t data4[15];
	uint8_t data5[15];
	uint8_t description[15];
};

struct CHANCONFIGLIST_chan_cfg_list_array_chan_cfg_list_sys_array
{
	uint32_t num_chans;
	int32_t signal_type;
};

struct CHANCONFIGLIST_chan_cfg_list_array
{
	uint32_t chan_cfg_list_sys_array_arraylength;
	CHANCONFIGLIST_chan_cfg_list_array_chan_cfg_list_sys_array chan_cfg_list_sys_array[16];
};

struct CHANCONFIGLIST
{
	uint32_t set_in_use;
	uint32_t chan_cfg_list_array_arraylength;
	CHANCONFIGLIST_chan_cfg_list_array chan_cfg_list_array[20];
};

struct SELECTCHANCONFIG
{
	uint32_t chan_config_setting;
};

struct SOFTLOADDATA
{
	uint32_t offset;
	uint32_t data_arraylength;
	uint8_t data[4096];
};

struct SOFTLOADSETUP
{
	int32_t setup_type;
	uint8_t setup_string[512];
};

struct SOFTLOADSTATUS
{
	int32_t status;
};

struct IPCONFIG
{
	int32_t interface;
	int32_t mode;
	uint8_t ip_address[16];
	uint8_t netmask[16];
	uint8_t gateway[16];
};

struct DNSCONFIG_dns_server
{
	uint8_t ip_address[16];
};

struct DNSCONFIG
{
	uint32_t dns_server_arraylength;
	DNSCONFIG_dns_server dns_server[1];
};

struct ETHCONFIG
{
	int32_t interface;
	int32_t speed;
	int32_t duplex;
	int32_t crossover;
	int32_t power_mode;
};

struct SERIALCONFIG
{
	int32_t port;
	uint32_t baud_rate;
	int32_t parity;
	uint32_t data_bits;
	uint32_t stop_bits;
	int32_t hand_shaking;
	int32_t breaks;
};

// Nick commented out, causing compile issues
// struct ECHO
// {
// 	int32_t port;
// 	int32_t echo;
// };

struct ICOMCONFIG
{
	int32_t port;
	int32_t protocol;
	uint8_t endpoint[80];
	int32_t bind_interface;
};

struct NTRIPCONFIG
{
	int32_t port;
	int32_t type;
	int32_t protocol;
	uint8_t endpoint[80];
	uint8_t mountpoint[80];
	uint8_t name[30];
	uint8_t password[30];
	int32_t bind_interface;
};

struct PASSICOM1
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSICOM2
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSICOM3
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSNCOM1
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSNCOM2
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSNCOM3
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct ETHSTATUS_eth_status
{
	int32_t interface;
	uint8_t mac_address[18];
	int32_t interface_config;
};

struct ETHSTATUS
{
	uint32_t eth_status_arraylength;
	ETHSTATUS_eth_status eth_status[2];
};

struct IPSTATUS_ip_status
{
	int32_t interface;
	uint8_t ip_address[16];
	uint8_t netmask[16];
	uint8_t gateway[16];
};

struct IPSTATUS_dns_server
{
	uint8_t ip_address_6[16];
};

struct IPSTATUS
{
	uint32_t ip_status_arraylength;
	IPSTATUS_ip_status ip_status[4];
	uint32_t dns_server_arraylength;
	IPSTATUS_dns_server dns_server[4];
};

struct SOFTLOADFILE
{
	int32_t file_system;
	uint8_t file_name[128];
	int32_t destination_device;
};

struct MODELFEATURES_features
{
	int32_t feature_status;
	int32_t feature;
};

struct MODELFEATURES
{
	uint32_t features_arraylength;
	MODELFEATURES_features features[30];
};

struct PASSTHROUGH
{
	int32_t port;
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct NTRIPSOURCETABLE
{
	uint8_t endpoint[80];
	uint32_t NTRIPSOURCETABLE_reserved1;
	uint32_t NTRIPSOURCETABLE_reserved2;
};

struct SOURCETABLE
{
	uint8_t endpoint[80];
	uint32_t SOURCETABLE_reserved1;
	uint32_t SOURCETABLE_reserved2;
	uint8_t entry_data[512];
};

struct PASSCOM4
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSICOM4
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PROFILE
{
	int32_t profile_option;
	uint8_t profile_name[20];
	uint8_t command[201];
};

struct PROFILEINFO_profile_info_command
{
	uint8_t command[201];
};

struct PROFILEINFO
{
	uint8_t profile_name[20];
	uint32_t status;
	uint32_t profile_info_command_arraylength;
	PROFILEINFO_profile_info_command profile_info_command[20];
};

struct SERIALPROTOCOL
{
	int32_t port;
	int32_t protocol;
};

struct NMEAVERSION
{
	int32_t nmea_version;
};

struct IPSERVICE
{
	int32_t i_net_port;
	int32_t enable;
};

struct PASSCOM5
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSCOM6
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct SETADMINPASSWORD
{
	uint8_t old_password[28];
	uint8_t new_password[28];
};

struct IPSTATS_ip_interface_statistics
{
	int32_t physical_interface;
	uint32_t connection_duration;
	uint32_t rx_count;
	uint32_t tx_count;
};

struct IPSTATS
{
	uint32_t ip_interface_statistics_arraylength;
	IPSTATS_ip_interface_statistics ip_interface_statistics[24];
};

struct LOGIN
{
	int32_t port;
	uint8_t name[32];
	uint8_t password[28];
};

struct LOGOUT
{
	int32_t port;
};

struct SAVEETHERNETDATA
{
	int32_t interface;
};

struct PASSCOM7
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSCOM8
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSCOM9
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSCOM10
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct UPTIME
{
	uint32_t uptime;
};

struct NMEAFORMAT
{
	int32_t field;
	uint8_t format[8];
};

struct RADARSTATUS
{
	uint32_t status;
	int32_t solution_status;
	int32_t solution_type;
	double horizontal_speed_mps;
	double smoothed_horizontal_speed_mps;
	double frequency;
};

struct RADARCONFIG
{
	int32_t sw;
	double frequency_step_hz_per_kph;
	int32_t update_rate;
	int32_t response_mode;
	double response_threshold_kph;
};

struct PGNCONFIG
{
	uint32_t message_id;
	uint32_t pgn;
	uint8_t priority;
};

struct CCOMCONFIG
{
	int32_t port;
	int32_t node;
	int32_t protocol;
	uint32_t pgn;
	uint8_t priority;
	uint8_t address;
};

struct J1939CONFIG
{
	int32_t j1939_node;
	int32_t can_port;
	uint32_t preferred_address;
	uint32_t alternate_address_range_start;
	uint32_t alternate_address_range_end;
	uint32_t manufacturer_code;
	uint32_t industry_group;
	uint32_t device_class;
	uint32_t device_class_instance;
	uint32_t function;
	uint32_t function_instance;
	uint32_t ecu_instance;
};

struct J1939STATUS
{
	int32_t j1939_node;
	int32_t j1939_node_status;
	uint32_t address_claim_count;
	uint8_t claimed_address;
};

struct NVMUSERDATA
{
	uint32_t data_arraylength;
	uint8_t data[2000];
};

struct SAFEMODESTATUS
{
	int32_t safe_mode_status;
	uint32_t reset_count;
	uint8_t description[80];
};

struct FILELIST
{
	int32_t mass_storage_device;
	int32_t entry_type;
	uint32_t file_size;
	uint32_t last_change_date;
	uint32_t last_change_time;
	uint8_t file_name[128];
};

struct FILETRANSFERSTATUS
{
	int32_t file_transfer_status;
	uint32_t total_transferred;
	uint32_t total_transfer_size;
	uint8_t file_name[128];
	uint8_t error_msg[256];
};

struct FILESYSTEMSTATUS
{
	int32_t mass_storage_device;
	int32_t file_system_status;
	uint32_t media_capacity;
	uint8_t error_message[64];
};

struct USBSTICKEJECT
{
};

struct FILECONFIG
{
	int32_t file_operation;
};

struct FILEMEDIACONFIG
{
	int32_t mass_storage_device;
};

struct PASSICOM5
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSICOM6
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct PASSICOM7
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct FILESTATUS
{
	int32_t media_type;
	int32_t file_status;
	uint8_t file_name[128];
	uint32_t file_size;
	uint32_t media_remaining_capacity;
	uint32_t media_total_capacity;
	uint8_t error_msg_string[128];
};

struct FILEROTATECONFIG
{
	uint16_t maximum_file_time;
	uint16_t maximum_file_size;
	int32_t disk_full_action;
};

struct FILEAUTOTRANSFER
{
	int32_t file_auto_transfer_mode;
};

struct LUAFILESYSTEMSTATUS
{
	int32_t file_system_status;
	uint8_t error[52];
};

struct LUAFILELIST
{
	uint32_t file_size;
	uint32_t last_changed_date;
	uint32_t last_changed_time;
	uint8_t file_path[256];
};

struct LUASTATUS
{
	uint32_t executor_number;
	uint8_t script_command_line[400];
	int32_t lua_status;
};

struct LUA
{
	int32_t action;
	uint8_t script_command_line[400];
};

struct FILEDELETE
{
	uint8_t filename[128];
};

struct USERI2CREAD
{
	uint8_t slave_device_address;
	uint32_t slave_register_address_arraylength;
	uint8_t slave_register_address[4];
	uint32_t request_length;
	uint32_t transaction_id;
};

struct USERI2CWRITE
{
	uint8_t slave_device_address;
	uint32_t slave_register_address_arraylength;
	uint8_t slave_register_address[4];
	uint32_t write_data_arraylength;
	uint8_t write_data[256];
	uint32_t transaction_id;
};

struct USERI2CRESPONSE
{
	uint8_t slave_device_address;
	uint32_t slave_register_address;
	int32_t status_code;
	int32_t operation_mode;
	uint32_t transaction_id;
	uint32_t read_data_arraylength;
	uint8_t read_data[256];
};

struct LUAOUTPUT
{
	uint32_t sequence_number;
	uint32_t lua_executor;
	int32_t source;
	uint8_t data[128];
};

struct PPPSEEDSTORESTATUS
{
	int32_t status;
	float horizontal_std_dev;
};

struct NMEABEIDOUTALKER
{
	int32_t talker_id;
};

struct ANTENNATYPE_pc_cs
{
	int32_t frequency;
	float pco[3];
	float pcv[19];
};

struct ANTENNATYPE
{
	int32_t manage_user_defined_antenna;
	int32_t antenna;
	uint8_t antenna_name[16];
	uint32_t pc_cs_arraylength;
	ANTENNATYPE_pc_cs pc_cs[24];
};

struct USERANTENNA_pc_cs
{
	int32_t frequency;
	float pco[3];
	float pcv[19];
};

struct USERANTENNA
{
	int32_t antenna;
	uint8_t antenna_name[16];
	uint32_t pc_cs_arraylength;
	USERANTENNA_pc_cs pc_cs[24];
};

struct TERRASTARAUTOCHANCONFIG
{
	int32_t sw;
};

struct GEODETICDATUM
{
	int32_t option;
	uint8_t name[32];
	uint32_t epsg_code;
	int32_t anchor;
	double semi_major_axis;
	double inverse_flattening;
};

struct GEODETICDATUMS_datums
{
	uint8_t name[32];
	uint32_t epsg_code;
	int32_t anchor;
	double semi_major_axis;
	double inverse_flattening;
};

struct GEODETICDATUMS
{
	uint32_t datums_arraylength;
	GEODETICDATUMS_datums datums[64];
};

struct DATUMTRANSFORMATION
{
	int32_t option;
	uint8_t name[32];
	uint8_t name_2[32];
	double epoch;
	float x_translation;
	float y_translation;
	float z_translation;
	float x_rotation;
	float y_rotation;
	float z_rotation;
	float scale_difference;
	float x_translation_rate;
	float y_translation_rate;
	float z_translation_rate;
	float x_rotation_rate;
	float y_rotation_rate;
	float z_rotation_rate;
	float scale_difference_rate;
};

struct DATUMTRANSFORMATIONS_transformations
{
	uint8_t name[32];
	uint8_t name_2[32];
	double epoch;
	float x_translation;
	float y_translation;
	float z_translation;
	float x_rotation;
	float y_rotation;
	float z_rotation;
	float scale_difference;
	float x_translation_rate;
	float y_translation_rate;
	float z_translation_rate;
	float x_rotation_rate;
	float y_rotation_rate;
	float z_rotation_rate;
	float scale_difference_rate;
};

struct DATUMTRANSFORMATIONS
{
	uint32_t transformations_arraylength;
	DATUMTRANSFORMATIONS_transformations transformations[63];
};

struct USERI2CBITRATE
{
	int32_t bit_rate;
};

struct GPSEPHEM
{
	uint32_t satellite_id;
	double tow;
	uint32_t health7;
	uint32_t iode1;
	uint32_t iode2;
	uint32_t wn;
	uint32_t zwn;
	double toe;
	double a;
	double delta_n;
	double m0;
	double ecc;
	double omega;
	double cuc;
	double cus;
	double crc;
	double crs;
	double cic;
	double cis;
	double i0;
	double i_dot;
	double omega0;
	double omega_dot;
	uint32_t iodc;
	double toc;
	double tgd;
	double af0;
	double af1;
	double af2;
	bool anti_spoofing;
	double n;
	double eph_var;
};

struct IONUTC
{
	double a0;
	double a1;
	double a2;
	double a3;
	double b0;
	double b1;
	double b2;
	double b3;
	uint32_t w_nt;
	uint32_t tot;
	double a0_10;
	double a1_11;
	uint32_t w_nlsf;
	uint32_t dn;
	int32_t delta_tls;
	int32_t delta_tlsf;
	uint32_t delta_tutc;
};

struct CLOCKADJUST
{
	int32_t command;
};

struct RAWGPSSUBFRAME
{
	int32_t frame_decoder_number;
	uint32_t satellite_id;
	uint32_t sub_frame_id;
	uint8_t raw_sub_frame_data[30];
	uint32_t signal_channel_number;
};

struct CLOCKSTEERING
{
	int32_t steering_source;
	int32_t steering_state;
	uint32_t modulus;
	double effective_pulse_width;
	double bandwidth;
	float slope;
	double last_offset;
	double last_rate;
};

struct ASSIGN
{
	uint32_t chan;
	int32_t assign_state;
	uint32_t prn;
	int32_t doppler;
	uint32_t doppler_window;
};

struct ASSIGNALL
{
	int32_t system;
	int32_t assign_state;
	uint32_t prn;
	int32_t doppler;
	uint32_t doppler_window;
};

struct UNASSIGN
{
	uint32_t chan;
	int32_t assign_state;
};

struct UNASSIGNALL
{
	int32_t system;
};

struct RAWEPHEM
{
	uint32_t satellite_id;
	uint32_t week;
	uint32_t toe;
	uint8_t subframe1[30];
	uint8_t subframe2[30];
	uint8_t subframe3[30];
};

struct RANGE_obs
{
	uint16_t sv_prn;
	uint16_t sv_freq;
	double psr;
	float sd_psr;
	double adr;
	float sd_adr;
	float dop;
	float C_No;
	float lock_time;
	uint32_t c_status;
};

struct RANGE
{
	uint32_t obs_arraylength;
	RANGE_obs obs[325];
};

struct SATVIS_sat_vis_list
{
	uint16_t prn;
	uint16_t freq;
	uint32_t sat_health;
	double elevation;
	double azimuth;
	double true_doppler;
	double apparent_doppler;
};

struct SATVIS
{
	bool is_sat_vis_valid;
	bool was_gps_almanac_used;
	uint32_t sat_vis_list_arraylength;
	SATVIS_sat_vis_list sat_vis_list[229];
};

struct ECUTOFF
{
	float elevation_cutoff_angle;
};

struct ALMANAC_sv_alm_data
{
	uint32_t prn;
	uint32_t wn;
	double toa;
	double ecc;
	double omega_dot;
	double omega0;
	double omega;
	double mo;
	double af0;
	double af1;
	double n;
	double a;
	double di;
	uint32_t sv_configuration;
	uint32_t health6;
	uint32_t health8;
	bool anti_spoofing_set;
};

struct ALMANAC
{
	uint32_t sv_alm_data_arraylength;
	ALMANAC_sv_alm_data sv_alm_data[32];
};

struct RAWALM_sub_frame_pages
{
	uint16_t svid;
	uint8_t page_raw_data[30];
};

struct RAWALM
{
	uint32_t weeks;
	uint32_t milliseconds;
	uint32_t sub_frame_pages_arraylength;
	RAWALM_sub_frame_pages sub_frame_pages[46];
};

struct TRACKSTAT_chan_status
{
	uint16_t prn;
	uint16_t freq;
	uint32_t channel_status;
	double psr;
	float doppler;
	float C_No;
	float lock_time;
	float psr_residual;
	int32_t psr_range_reject;
	float psr_filter_weighting;
};

struct TRACKSTAT
{
	int32_t position_status;
	int32_t position_type;
	float tracking_elevation_cutoff;
	uint32_t chan_status_arraylength;
	TRACKSTAT_chan_status chan_status[325];
};

struct TIME
{
	int32_t clock_model_status;
	double offset;
	double offset_std;
	double utc_offset;
	uint32_t utc_year;
	uint8_t utc_month;
	uint8_t utc_day;
	uint8_t utc_hour;
	uint8_t utc_minute;
	uint32_t utc_millisecond;
	int32_t utc_time_status;
};

struct SETAPPROXTIME
{
	uint32_t weeks;
	double seconds;
};

struct LOCKOUT
{
	uint32_t prn;
};

struct UNLOCKOUT
{
	uint32_t prn;
};

struct UNLOCKOUTALL
{
};

struct RANGECMP_range_c_entry
{
	uint8_t range_c_data[24];
};

struct RANGECMP
{
	uint32_t range_c_entry_arraylength;
	RANGECMP_range_c_entry range_c_entry[325];
};

struct EXTERNALCLOCK
{
	int32_t clock_type;
	int32_t frequency;
	double h0;
	double hm1;
	double hm2;
};

struct MARKTIME
{
	int32_t week;
	double seconds;
	double offset;
	double offset_std;
	double utc_offset;
	int32_t status;
};

struct FREQUENCYOUT
{
	int32_t enable;
	uint32_t pulse_width;
	uint32_t period;
};

struct DYNAMICS
{
	int32_t value;
};

struct RAWGPSWORD
{
	uint32_t prn;
	uint32_t raw_word;
};

struct ADJUST1PPS
{
	int32_t mode;
	int32_t period;
	int32_t time;
};

struct CLOCKCALIBRATE
{
	int32_t setting;
	uint32_t modulus;
	uint32_t center;
	float slope;
	float bandwidth;
};

struct TIMESYNC
{
	uint32_t gps_week;
	uint32_t milliseconds;
	int32_t time_status;
};

struct CLOCKOFFSET
{
	int32_t clock_offset;
};

struct PPSCONTROL
{
	int32_t enable_disable;
	int32_t polarity;
	double rate;
	uint32_t pulse_width;
};

struct MARKCONTROL
{
	int32_t mark;
	int32_t enable_disable;
	int32_t polarity;
	int32_t time_bias;
	uint32_t time_guard;
};

struct MARK2TIME
{
	int32_t week;
	double seconds;
	double offset;
	double offset_std;
	double utc_offset;
	int32_t status;
};

struct RANGEGPSL1_obs
{
	uint16_t sv_prn;
	uint16_t sv_freq;
	double psr;
	float sd_psr;
	double adr;
	float sd_adr;
	float dop;
	float C_No;
	float lock_time;
	uint32_t c_status;
};

struct RANGEGPSL1
{
	uint32_t obs_arraylength;
	RANGEGPSL1_obs obs[325];
};

struct GLOALMANAC_sat_alm_data
{
	uint32_t weeks;
	uint32_t milliseconds;
	uint8_t slot;
	char frequency;
	uint8_t sat_type;
	uint8_t health;
	double t_lambda_n;
	double lambda_n;
	double delta_i;
	double ecc;
	double arg_perig;
	double delta_t;
	double delta_td;
	double tau;
};

struct GLOALMANAC
{
	uint32_t sat_alm_data_arraylength;
	GLOALMANAC_sat_alm_data sat_alm_data[24];
};

struct GLOCLOCK
{
	uint32_t nominal_offset;
	double residual_offset;
	double residual_offset_var;
	uint8_t sat_type;
	uint8_t n4;
	double tau_gps;
	uint16_t na;
	double tau_c;
	double b1;
	double b2;
	uint8_t kp;
};

struct GLORAWALM_strings
{
	uint8_t string[11];
	uint8_t GLORAWALM_reserved;
};

struct GLORAWALM
{
	uint32_t weeks;
	uint32_t milliseconds;
	uint32_t strings_arraylength;
	GLORAWALM_strings strings[54];
};

struct GLORAWFRAME_raw_string
{
	uint8_t string[11];
	uint8_t GLORAWFRAME_reserved;
};

struct GLORAWFRAME
{
	uint32_t frame_number;
	uint16_t sloto;
	uint16_t freqo;
	uint32_t weeks;
	uint32_t milliseconds;
	uint32_t frame_decoder_number;
	uint32_t signal_channel_number;
	uint32_t raw_string_arraylength;
	GLORAWFRAME_raw_string raw_string[15];
};

struct GLORAWSTRING
{
	uint8_t slot;
	char freq;
	uint8_t string[11];
	uint8_t GLORAWSTRING_reserved;
};

struct GLOEPHEMERIS
{
	uint16_t sloto;
	uint16_t freqo;
	uint8_t sat_type;
	uint8_t false_iod;
	uint16_t ephem_week;
	uint32_t ephem_time;
	uint32_t time_offset;
	uint16_t nt;
	uint8_t GLOEPHEMERIS_reserved;
	uint8_t GLOEPHEMERIS_reserved_9;
	uint32_t issue;
	uint32_t broadcast_health;
	double pos_x;
	double pos_y;
	double pos_z;
	double vel_x;
	double vel_y;
	double vel_z;
	double ls_acc_x;
	double ls_acc_y;
	double ls_acc_z;
	double tau;
	double delta_tau;
	double gamma;
	uint32_t tk;
	uint32_t p;
	uint32_t ft;
	uint32_t age;
	uint32_t flags;
};

struct GLOECUTOFF
{
	float elevation_cutoff_angle;
};

struct GLORAWEPHEM_glo_raw_ephem
{
	uint8_t string[11];
	uint8_t GLORAWEPHEM_reserved;
};

struct GLORAWEPHEM
{
	uint16_t sloto;
	uint16_t freqo;
	uint32_t signal_channel_number;
	uint32_t weeks;
	uint32_t milliseconds;
	uint32_t glo_raw_ephem_arraylength;
	GLORAWEPHEM_glo_raw_ephem glo_raw_ephem[4];
};

struct FORCEGPSL2CODE
{
	int32_t code_type;
};

struct CNOUPDATE
{
	int32_t c_no_update_rate;
};

struct RAWSBASFRAME
{
	int32_t frame_decoder_num;
	uint32_t prn;
	uint32_t waas_msg_id;
	uint8_t raw_frame_data[29];
	uint32_t signal_channel_num;
};

struct SBAS0
{
	uint32_t prn;
};

struct SBAS1
{
	uint32_t prn;
	uint8_t prn_mask[27];
	uint32_t iodp;
};

struct SBAS10
{
	uint32_t prn;
	uint32_t brrc;
	uint32_t cltc_lsb;
	uint32_t cltc_v1;
	uint32_t iltc_v1;
	uint32_t cltc_v0;
	uint32_t iltc_v0;
	uint32_t cgeo_lsb;
	uint32_t cgeo_v;
	uint32_t igeo;
	uint32_t cer;
	uint32_t ciono_step;
	uint32_t iiono;
	uint32_t ciono_ramp;
	uint32_t rssudre;
	uint32_t rss_iono;
	uint8_t spare_bits[11];
};

struct SBAS17_items
{
	uint16_t data_id;
	uint16_t prn_3;
	uint16_t health;
	int32_t x;
	int32_t y;
	int32_t z;
	int32_t x_vel;
	int32_t y_vel;
	int32_t z_vel;
};

struct SBAS17
{
	uint32_t prn;
	uint32_t items_arraylength;
	SBAS17_items items[3];
	uint32_t t0;
};

struct SBAS18
{
	uint32_t prn;
	uint32_t num_bands;
	uint32_t band_num;
	uint32_t iodi;
	uint8_t igp_mask[26];
	uint32_t spare_bit;
};

struct SBAS2
{
	uint32_t prn;
	uint32_t iodf;
	uint32_t iodp;
	int32_t prc[13];
	uint32_t udrei[13];
};

struct SBAS24
{
	uint32_t prn;
	int32_t prc[6];
	uint32_t udrei[6];
	uint32_t iodp;
	uint32_t block_id;
	uint32_t iodf;
	uint32_t spare;
	uint32_t velocity_code;
	uint32_t prn_mask_number1;
	uint32_t iode1;
	int32_t dx1;
	int32_t dy1;
	int32_t dz1;
	int32_t a_f01;
	uint32_t prn_mask_number2;
	uint32_t iode2;
	int32_t dx2or_ddx;
	int32_t dy2or_ddy;
	int32_t dz2or_ddz;
	int32_t a_f02ora_f1;
	uint32_t tod;
	uint32_t iodp_21;
	uint32_t spare_22;
};

struct SBAS25
{
	uint32_t prn;
	uint32_t velocity_code;
	uint32_t prn_mask_number1;
	uint32_t iode1;
	int32_t dx1;
	int32_t dy1;
	int32_t dz1;
	int32_t a_f01;
	uint32_t prn_mask_number2;
	uint32_t iode2;
	int32_t dx2or_ddx;
	int32_t dy2or_ddy;
	int32_t dz2or_ddz;
	int32_t a_f02ora_f1;
	uint32_t tod;
	uint32_t iodp;
	uint32_t spare;
	uint32_t velocity_code_17;
	uint32_t prn_mask_number1_18;
	uint32_t iode1_19;
	int32_t dx1_20;
	int32_t dy1_21;
	int32_t dz1_22;
	int32_t a_f01_23;
	uint32_t prn_mask_number2_24;
	uint32_t iode2_25;
	int32_t dx2or_ddx_26;
	int32_t dy2or_ddy_27;
	int32_t dz2or_ddz_28;
	int32_t a_f02ora_f1_29;
	uint32_t tod_30;
	uint32_t iodp_31;
	uint32_t spare_32;
};

struct SBAS26_grid_point_data
{
	uint32_t igpvde;
	uint32_t givei;
};

struct SBAS26
{
	uint32_t prn;
	uint32_t band_num;
	uint32_t block_id;
	uint32_t grid_point_data_arraylength;
	SBAS26_grid_point_data grid_point_data[15];
	uint32_t iodi;
	uint32_t spare_bits;
};

struct SBAS3
{
	uint32_t prn;
	uint32_t iodf;
	uint32_t iodp;
	int32_t prc[13];
	uint32_t udrei[13];
};

struct SBAS4
{
	uint32_t prn;
	uint32_t iodf;
	uint32_t iodp;
	int32_t prc[13];
	uint32_t udrei[13];
};

struct SBAS5
{
	uint32_t prn;
	uint32_t iodf;
	uint32_t iodp;
	int32_t prc[13];
	uint32_t udrei[13];
};

struct SBAS7
{
	uint32_t prn;
	uint32_t system_latency;
	uint32_t iodp;
	uint32_t spare_bits;
	uint32_t degradation_factor[51];
};

struct SBAS9
{
	uint32_t prn;
	uint32_t iodn;
	uint32_t t0;
	uint32_t ura;
	double x;
	double y;
	double z;
	double x_vel;
	double y_vel;
	double z_vel;
	double x_accel;
	double y_accel;
	double z_accel;
	double af0;
	double af1;
};

struct SBASECUTOFF
{
	float elevation_cutoff_angle;
};

struct DLLTIMECONST
{
	int32_t signal_type;
	uint32_t time_const_sec;
};

struct SATVIS2_sat_vis_list
{
	uint32_t id;
	uint32_t sat_health;
	double elevation;
	double azimuth;
	double true_doppler;
	double apparent_doppler;
};

struct SATVIS2
{
	int32_t system_type;
	bool is_sat_vis_valid;
	bool was_gnss_almanac_used;
	uint32_t sat_vis_list_arraylength;
	SATVIS2_sat_vis_list sat_vis_list[63];
};

struct RAWCNAVFRAME
{
	uint32_t sig_chan_num;
	uint32_t prn;
	uint32_t frame_id;
	uint8_t raw_frame_data[38];
};

struct MARK3TIME
{
	int32_t week;
	double seconds;
	double offset;
	double offset_std;
	double utc_offset;
	int32_t status;
};

struct MARK4TIME
{
	int32_t week;
	double seconds;
	double offset;
	double offset_std;
	double utc_offset;
	int32_t status;
};

struct GALECUTOFF
{
	float elevation_cutoff_angle;
};

struct GALALMANAC
{
	uint32_t sat_id;
	bool fnav_received;
	bool inav_received;
	uint8_t e1_b_health;
	uint8_t e5a_health;
	uint8_t e5b_health;
	uint8_t GALALMANAC_reserved1;
	uint32_t io_da;
	uint32_t weeks;
	uint32_t milliseconds;
	double ecc;
	double omega_dot;
	double omega0;
	double omega;
	double m0;
	double af0;
	double af1;
	double delta_root_a;
	double delta_i;
};

struct GALCLOCK
{
	double a0;
	double a1;
	int32_t delta_tls;
	uint32_t tot;
	uint32_t w_nt;
	uint32_t w_nlsf;
	uint32_t dn;
	int32_t delta_tlsf;
	double a0g;
	double a1g;
	uint32_t t0g;
	uint32_t wn0g;
};

struct GALEPHEMERIS
{
	uint32_t sat_id;
	bool fnav_received;
	bool inav_received;
	uint8_t e1_b_health;
	uint8_t e5a_health;
	uint8_t e5b_health;
	uint8_t e1_bdvs;
	uint8_t e5a_dvs;
	uint8_t e5b_dvs;
	uint8_t inavsisa;
	uint8_t GALEPHEMERIS_reserved1;
	uint32_t iod_nav;
	uint32_t t0e;
	double root_a;
	double delta_n;
	double m0;
	double ecc;
	double omega;
	double cuc;
	double cus;
	double crc;
	double crs;
	double cic;
	double cis;
	double i0;
	double i_dot;
	double omega0;
	double omega_dot;
	uint32_t fnavt0c;
	double fnav_af0;
	double fnav_af1;
	double fnav_af2;
	uint32_t inavt0c;
	double inav_af0;
	double inav_af1;
	double inav_af2;
	double e1_e5a_bgd;
	double e1_e5b_bgd;
};

struct GALFNAVRAWALMANAC
{
	uint32_t io_da;
	uint32_t w_na;
	uint32_t t0a;
	uint8_t raw_data[20];
};

struct GALFNAVRAWEPHEMERIS_raw_pages
{
	uint8_t page[27];
	uint8_t GALFNAVRAWEPHEMERIS_reserved;
};

struct GALFNAVRAWEPHEMERIS
{
	uint32_t sat_id;
	uint32_t weeks;
	uint32_t milliseconds;
	uint32_t raw_pages_arraylength;
	GALFNAVRAWEPHEMERIS_raw_pages raw_pages[4];
};

struct GALINAVRAWALMANAC
{
	uint32_t io_da;
	uint32_t w_na;
	uint32_t t0a;
	uint8_t raw_data[20];
};

struct GALINAVRAWEPHEMERIS_raw_words
{
	uint8_t word[16];
};

struct GALINAVRAWEPHEMERIS
{
	uint32_t sat_id;
	uint32_t weeks;
	uint32_t milliseconds;
	uint32_t raw_words_arraylength;
	GALINAVRAWEPHEMERIS_raw_words raw_words[6];
};

struct GALIONO
{
	double ai0;
	double ai1;
	double ai2;
	uint8_t sf1;
	uint8_t sf2;
	uint8_t sf3;
	uint8_t sf4;
	uint8_t sf5;
};

struct MARK1TIME
{
	int32_t week;
	double seconds;
	double offset;
	double offset_std;
	double utc_offset;
	int32_t status;
};

struct SETUTCLEAPSECONDS
{
	uint32_t seconds;
	uint32_t future_week_number;
	uint32_t future_day_number;
	uint32_t future_seconds;
};

struct LBANDTRACKSTAT_l_band_chan_states
{
	uint8_t beam_name[8];
	uint32_t assigned_frequency;
	uint16_t baud_rate;
	uint16_t service_id;
	uint16_t tracking_status;
	uint16_t LBANDTRACKSTAT_reserved;
	float doppler;
	float cn0;
	float phase_std_dev;
	float lock_time;
	uint32_t total_unique_word_bits;
	uint32_t bad_unique_word_bits;
	uint32_t bad_unique_words;
	uint32_t total_viterbi_symbols;
	uint32_t corrected_viterbi_syms;
	float ber;
};

struct LBANDTRACKSTAT
{
	uint32_t l_band_chan_states_arraylength;
	LBANDTRACKSTAT_l_band_chan_states l_band_chan_states[5];
};

struct FORCEGLOL2CODE
{
	int32_t l2_code_type;
};

struct SETTIMEBASE_backup_system
{
	int32_t system;
	uint32_t time_out;
};

struct SETTIMEBASE
{
	int32_t primary_system;
	uint32_t backup_system_arraylength;
	SETTIMEBASE_backup_system backup_system[4];
};

struct RANGECMP2
{
	uint32_t range_data_arraylength;
	uint8_t range_data[7800];
};

struct GALINAVEPHEMERIS
{
	uint32_t sat_id;
	uint8_t e5b_health;
	uint8_t e5b_dvs;
	uint8_t GALINAVEPHEMERIS_reserved1;
	uint8_t GALINAVEPHEMERIS_reserved2;
	uint8_t e1b_health;
	uint8_t e1b_dvs;
	uint8_t GALINAVEPHEMERIS_reserved3;
	uint8_t GALINAVEPHEMERIS_reserved4;
	uint16_t io_dnav;
	uint8_t sisa;
	uint8_t inav_signal_type;
	uint32_t toe;
	uint32_t toc;
	double m0;
	double delta_n;
	double ecc;
	double root_a;
	double i0;
	double i_dot;
	double omega0;
	double omega;
	double omega_dot;
	double cuc;
	double cus;
	double crc;
	double crs;
	double cic;
	double cis;
	double af0;
	double af1;
	double af2;
	double e1_e5a_bgd;
	double e1_e5b_bgd;
};

struct GALFNAVEPHEMERIS
{
	uint32_t sat_id;
	uint8_t e5a_health;
	uint8_t e5a_dvs;
	uint8_t GALFNAVEPHEMERIS_reserved1;
	uint8_t GALFNAVEPHEMERIS_reserved2;
	uint16_t io_dnav;
	uint8_t sisa;
	uint8_t GALFNAVEPHEMERIS_reserved3;
	uint32_t toe;
	uint32_t toc;
	double m0;
	double delta_n;
	double ecc;
	double root_a;
	double i0;
	double i_dot;
	double omega0;
	double omega;
	double omega_dot;
	double cuc;
	double cus;
	double crc;
	double crs;
	double cic;
	double cis;
	double af0;
	double af1;
	double af2;
	double e1_e5a_bgd;
};

struct TRACKSV
{
	int32_t system;
	uint32_t sat_id;
	int32_t condition;
};

struct QZSSRAWSUBFRAME
{
	uint32_t satellite_id;
	uint32_t sub_frame_id;
	uint8_t raw_sub_frame_data[30];
	uint32_t signal_channel_number;
};

struct QZSSRAWEPHEM
{
	uint32_t satellite_id;
	uint32_t week;
	uint32_t toe;
	uint8_t subframe1[30];
	uint8_t subframe2[30];
	uint8_t subframe3[30];
};

struct QZSSEPHEMERIS
{
	uint32_t satellite_id;
	double tow;
	uint32_t health7;
	uint32_t iode1;
	uint32_t iode2;
	uint32_t wn;
	uint32_t zwn;
	double toe;
	double a;
	double delta_n;
	double m0;
	double ecc;
	double omega;
	double cuc;
	double cus;
	double crc;
	double crs;
	double cic;
	double cis;
	double i0;
	double i_dot;
	double omega0;
	double omega_dot;
	uint32_t iodc;
	double toc;
	double tgd;
	double af0;
	double af1;
	double af2;
	bool anti_spoofing;
	double n;
	double eph_var;
	uint8_t fit_interval;
	char char_as_int;
	char char_as_int_34;
	char char_as_int_35;
};

struct QZSSRAWALMANAC_sub_frame_pages
{
	uint16_t svid;
	uint8_t page_raw_data[30];
};

struct QZSSRAWALMANAC
{
	uint32_t weeks;
	uint32_t milliseconds;
	uint32_t sub_frame_pages_arraylength;
	QZSSRAWALMANAC_sub_frame_pages sub_frame_pages[46];
};

struct QZSSALMANAC_sv_alm_data
{
	uint32_t prn;
	uint32_t wn;
	double toa;
	double ecc;
	double omega_dot;
	double omega0;
	double omega;
	double mo;
	double af0;
	double af1;
	double n;
	double a;
	double di;
	uint32_t health6;
	uint32_t health8;
};

struct QZSSALMANAC
{
	uint32_t sv_alm_data_arraylength;
	QZSSALMANAC_sv_alm_data sv_alm_data[10];
};

struct QZSSIONUTC
{
	double a0;
	double a1;
	double a2;
	double a3;
	double b0;
	double b1;
	double b2;
	double b3;
	uint32_t w_nt;
	uint32_t tot;
	double a0_10;
	double a1_11;
	uint32_t w_nlsf;
	uint32_t dn;
	int32_t delta_tls;
	int32_t delta_tlsf;
	uint32_t delta_tutc;
};

struct AUTHCODES_auth_codes
{
	int32_t type;
	bool valid;
	uint8_t auth_code[80];
};

struct AUTHCODES
{
	int32_t signature_status;
	uint32_t auth_codes_arraylength;
	AUTHCODES_auth_codes auth_codes[24];
};

struct QZSSECUTOFF
{
	float elevation_cutoff_angle;
};

struct RTKTRACKINGCONTROL
{
	int32_t mode;
};

struct GALFNAVRAWPAGE
{
	uint32_t sig_chan_num;
	uint32_t sat_id;
	uint8_t raw_frame_data[27];
};

struct GALINAVRAWWORD
{
	uint32_t sig_chan_num;
	uint32_t sat_id;
	int32_t signal_type;
	uint8_t raw_frame_data[16];
};

struct SATXYZ2_sats
{
	int32_t system_type;
	uint32_t id;
	double x;
	double y;
	double z;
	double rb;
	double iono_corr;
	double tropo_corr;
	double dummy;
	double dummy_10;
};

struct SATXYZ2
{
	uint32_t sats_arraylength;
	SATXYZ2_sats sats[72];
};

struct DOPPLERWINDOW
{
	int32_t system;
	int32_t setting;
	uint32_t doppler_window;
	int32_t center_frequency;
};

struct APPROXPOSTIMEOUT
{
	uint32_t timeout_sec;
};

struct QZSSRAWCNAVMESSAGE
{
	uint32_t sig_chan_num;
	uint32_t prn;
	uint32_t message_id;
	uint8_t raw_frame_data[38];
};

struct DATADECODESIGNAL
{
	int32_t signal_type;
	int32_t data_decode_mode;
};

struct BDSECUTOFF
{
	float elevation_cutoff_angle;
};

struct BDSALMANAC
{
	uint32_t satellite_id;
	uint32_t wn;
	uint32_t toa;
	double root_a;
	double eccentricity;
	double omega;
	double m0;
	double omega0;
	double omega_dot;
	double delta_i;
	double a0;
	double a1;
	uint32_t health;
};

struct BDSIONO
{
	uint32_t transmitting_satellite_id;
	double alpha0;
	double alpha1;
	double alpha2;
	double alpha3;
	double beta0;
	double beta1;
	double beta2;
	double beta3;
};

struct BDSCLOCK
{
	double a0_utc;
	double a1_utc;
	int16_t delta_tls;
	uint16_t w_nlsf;
	uint16_t dn;
	int16_t delta_tlsf;
	double a0_gps;
	double a1_gps;
	double a0_gal;
	double a1_gal;
	double a0_glo;
	double a1_glo;
};

struct EVENTOUTCONTROL
{
	int32_t mark;
	int32_t enable_disable;
	int32_t polarity;
	uint32_t active_period;
	uint32_t not_active_period;
};

struct EVENTINCONTROL
{
	int32_t mark;
	int32_t event_in_enable;
	int32_t polarity;
	int32_t time_bias;
	uint32_t time_guard;
};

struct RFINPUTGAIN
{
	int32_t frequency;
	int32_t mode;
	float gain;
};

struct BDSRAWNAVSUBFRAME
{
	uint32_t signal_channel_number;
	uint32_t satellite_id;
	int32_t bds_data_source;
	uint32_t subframe_id;
	uint8_t raw_subframe_data[28];
};

struct BDSEPHEMERIS
{
	uint32_t satellite_id;
	uint32_t wn;
	double ura;
	uint32_t sat_h1;
	double tgd1;
	double tgd2;
	uint32_t iodc;
	uint32_t toc;
	double a0;
	double a1;
	double a2;
	uint32_t iode;
	uint32_t toe;
	double root_a;
	double eccentricity;
	double omega;
	double delta_n;
	double m0;
	double omega0;
	double omega_dot;
	double i0;
	double i_dot;
	double cuc;
	double cus;
	double crc;
	double crs;
	double cic;
	double cis;
};

struct ASSIGNLBANDBEAM
{
	int32_t assign_mode;
	uint8_t beam_name[8];
	uint32_t frequency;
	uint32_t baud_rate;
	uint32_t doppler_window;
};

struct RANGECMP3
{
	uint32_t range_data_arraylength;
	uint8_t range_data[16250];
};

struct ELEVATIONCUTOFF
{
	int32_t system_type;
	float elevation_cutoff_angle;
	uint32_t ELEVATIONCUTOFF_reserved;
};

struct PPSCONTROL2
{
	int32_t enable_disable;
	int32_t polarity;
	double rate;
	uint32_t pulse_width;
	int32_t estimated_error_limit;
};

struct ITSPECTRALANALYSIS
{
	int32_t data_source;
	int32_t frequency;
	uint32_t psd_update_period_ms;
	int32_t fft_size;
	uint32_t psd_time_ave_window_sec;
	uint32_t psd_subcarrier_ave_window;
};

struct ITPSDFINAL
{
	uint32_t status_word;
	float frequency_start_m_hz;
	float step_size_hz;
	uint32_t samples_arraylength;
	uint16_t samples[1024];
};

struct ITFILTTABLE_filter_coef_status_nf_status
{
	int32_t enabled_9;
	int32_t pfid;
	int32_t mode;
	float lower_cut_off_frequency;
	float higher_cut_off_frequency;
	float frequency_width;
};

struct ITFILTTABLE_filter_coef_status
{
	int32_t frequency;
	uint32_t encoder_id;
	int32_t ddc_filter_type;
	uint32_t status;
	int32_t enabled;
	float lower_cut_off_freqency;
	float higher_cut_off_freqency;
	uint32_t nf_status_arraylength;
	ITFILTTABLE_filter_coef_status_nf_status nf_status[3];
};

struct ITFILTTABLE
{
	uint32_t filter_coef_status_arraylength;
	ITFILTTABLE_filter_coef_status filter_coef_status[25];
};

struct ITBANDPASSCONFIG
{
	int32_t frequency;
	int32_t enable;
	float cut_off_frequency;
};

struct ITPROGFILTCONFIG
{
	int32_t frequency;
	int32_t pfid;
	int32_t enable;
	int32_t mode;
	float cut_off_frequency;
	float notch_width;
};

struct ITBANDPASSBANK_bpf_bank_entries
{
	int32_t frequency;
	float min_lower_cut_off_frequency;
	float max_lower_cut_off_frequency;
	float min_higher_cut_off_frequency;
	float max_higher_cut_off_frequency;
	float frequency_step;
};

struct ITBANDPASSBANK
{
	uint32_t bpf_bank_entries_arraylength;
	ITBANDPASSBANK_bpf_bank_entries bpf_bank_entries[12];
};

struct ITPROGFILTBANK_nf_bank_entries_nf_parameters
{
	int32_t nf_mode;
	float min_lower_cut_off_frequency;
	float max_lower_cut_off_frequency;
	float min_higher_cut_off_frequency;
	float max_higher_cut_off_frequency;
	float frequency_step;
	float notch_width;
};

struct ITPROGFILTBANK_nf_bank_entries
{
	int32_t frequency;
	uint32_t nf_parameters_arraylength;
	ITPROGFILTBANK_nf_bank_entries_nf_parameters nf_parameters[5];
};

struct ITPROGFILTBANK
{
	uint32_t nf_bank_entries_arraylength;
	ITPROGFILTBANK_nf_bank_entries nf_bank_entries[24];
};

struct ITFRONTENDMODE
{
	int32_t freq;
	int32_t mode;
};

struct RANGECMP4
{
	uint32_t range_data_arraylength;
	uint8_t range_data[16250];
};

struct ITPSDDETECT
{
	uint32_t status_word;
	float frequency_start_m_hz;
	float step_size_hz;
	uint32_t samples_arraylength;
	uint16_t samples[1024];
};

struct ITDETECTSTATUS_interference_statuses
{
	int32_t frequency;
	int32_t int_detect_method;
	float parameter1;
	float parameter2;
	float parameter3;
	float parameter4;
	uint32_t severity1;
	uint32_t severity2;
	uint32_t ITDETECTSTATUS_reserved3;
};

struct ITDETECTSTATUS
{
	uint32_t interference_statuses_arraylength;
	ITDETECTSTATUS_interference_statuses interference_statuses[80];
};

struct NAVICRAWSUBFRAME
{
	uint32_t sig_chan_num;
	uint32_t sat_id;
	uint32_t frame_id;
	uint8_t raw_subframe_data[33];
};

struct NAVICALMANAC
{
	uint32_t w_na;
	double ecc;
	uint32_t toa;
	double di;
	double omega_dot;
	double root_a;
	double omega0;
	double omega;
	double m0;
	double af0;
	double af1;
	uint32_t alm_svid;
	uint32_t inter_sig_corr;
	uint32_t spare;
	uint32_t svid;
};

struct NAVICEPHEMERIS
{
	uint32_t sat_id;
	uint32_t wn;
	double af0;
	double af1;
	double af2;
	uint32_t ura;
	uint32_t toc;
	double tgd;
	double delta_n;
	uint32_t iodec;
	uint32_t NAVICEPHEMERIS_reserved;
	uint32_t l5_health;
	uint32_t s_health;
	double cuc;
	double cus;
	double cic;
	double cis;
	double crc;
	double crs;
	double i_dot;
	uint32_t spare1;
	double m0;
	uint32_t toe;
	double ecc;
	double root_a;
	double omega0;
	double omega;
	double omega_dot;
	double i0;
	uint32_t spare2;
	uint32_t alert_flag;
	uint32_t auto_nav_flag;
};

struct NAVICIONO
{
	uint32_t prnid;
	double alpha0;
	double alpha1;
	double alpha2;
	double alpha3;
	double beta0;
	double beta1;
	double beta2;
	double beta3;
	uint32_t spare;
};

struct NAVICSYSCLOCK
{
	uint32_t prnid;
	double a0_utc;
	double a1_utc;
	double a2_utc;
	int32_t delta_tls;
	uint32_t toutc;
	uint32_t w_noutc;
	uint32_t wnlsf;
	uint32_t dn;
	int32_t delta_tlsf;
	uint32_t gnssid;
	double a0;
	double a1;
	double a2;
	uint32_t tot;
	uint32_t w_not;
	uint32_t spare;
};

struct NAVICECUTOFF
{
	float elevation_cutoff_angle;
};

struct ITDETECTCONFIG
{
	int32_t frequency;
	uint32_t ITDETECTCONFIG_reserved1;
	uint32_t ITDETECTCONFIG_reserved2;
	uint32_t ITDETECTCONFIG_reserved3;
};

struct RAWSBASFRAME2
{
	uint32_t prn;
	uint32_t signal_channel_num;
	uint8_t sbas_signal;
	uint8_t preamble_type;
	uint16_t reserve;
	uint32_t waas_msg_id;
	uint8_t raw_frame_data[29];
};

struct FORCEGALE6CODE
{
	int32_t type;
};

struct GALCNAVRAWPAGE
{
	uint32_t chan;
	uint32_t sat_id;
	uint16_t message_id;
	uint16_t page_id;
	uint8_t page[58];
};

struct QZSSCNAVRAWMESSAGE
{
	uint32_t sig_chan_num;
	uint32_t prn;
	int32_t signal_type;
	uint32_t message_id;
	uint8_t raw_frame_data[38];
};

struct GPSCNAVRAWMESSAGE
{
	uint32_t sig_chan_num;
	uint32_t prn;
	int32_t signal_type;
	uint32_t frame_id;
	uint8_t raw_frame_data[38];
};

struct SPRINKLERDATA
{
	uint32_t config;
	uint32_t start_sample_index;
	uint32_t samples_arraylength;
	int16_t samples[1024];
};

struct SPRINKLERCONFIG
{
	int32_t sprinker_channel;
	int32_t enable;
	int32_t signal;
	int32_t sample_source;
	uint32_t num_samples_requested;
	uint32_t period_ms;
	uint32_t offset_ms;
};

struct SPRINKLERDATAH
{
	int32_t signal;
	int32_t freq_band;
	uint32_t config;
	uint32_t rf_start_frequency;
	uint32_t sample_rate;
	int32_t collection_status;
	uint16_t adc_pulse_width;
	uint16_t agc_pulse_modulus;
	uint32_t num_samples_collected;
};

struct ITWARNINGCONFIG
{
	uint32_t level;
	uint32_t ITWARNINGCONFIG_reserved;
};

struct TRACKSIGNAL
{
	int32_t signal;
	int32_t enable;
};

struct SKDETECTCONFIG
{
	int32_t enable;
	uint32_t SKDETECTCONFIG_reserved0;
	uint32_t SKDETECTCONFIG_reserved1;
};

struct SKCALIBRATESTATUS_calibration_results
{
	int32_t signal_type;
	int32_t mode;
	int32_t front_end_mode;
	int32_t result;
	uint32_t data;
};

struct SKCALIBRATESTATUS
{
	int32_t overall_status;
	int32_t frequency_plan;
	uint32_t calibration_results_arraylength;
	SKCALIBRATESTATUS_calibration_results calibration_results[10];
};

struct SKCALIBRATE
{
	int32_t mode;
	int32_t option;
	uint32_t SKCALIBRATE_reserved;
};

struct BDSBCNAV1EPHEMERIS
{
	uint32_t satellite_id;
	uint32_t wn;
	uint32_t sat_status;
	uint32_t iode;
	uint32_t toe;
	uint32_t sat_type;
	double delta_a;
	double a_dot;
	double delta_n;
	double n_dot;
	double m0;
	double eccentricity;
	double omega;
	double omega0;
	double i0;
	double omega_dot;
	double i_dot;
	double cis;
	double cic;
	double crs;
	double crc;
	double cus;
	double cuc;
	uint32_t iodc;
	uint32_t toc;
	double a0;
	double a1;
	double a2;
	double tgdb1_cp;
	double tgdb2_ap;
	double iscb1_cd;
	uint32_t BDSBCNAV1EPHEMERIS_reserved;
};

struct BDSBCNAV2EPHEMERIS
{
	uint32_t satellite_id;
	uint32_t wn;
	uint32_t sat_status;
	uint32_t iode;
	uint32_t toe;
	uint32_t sat_type;
	double delta_a;
	double a_dot;
	double delta_n;
	double n_dot;
	double m0;
	double eccentricity;
	double omega;
	double omega0;
	double i0;
	double omega_dot;
	double i_dot;
	double cis;
	double cic;
	double crs;
	double crc;
	double cus;
	double cuc;
	uint32_t iodc;
	uint32_t toc;
	double a0;
	double a1;
	double a2;
	double tgdb1_cp;
	double tgdb2_ap;
	double iscb2ad;
	uint32_t BDSBCNAV2EPHEMERIS_reserved;
};

struct BDSBCNAV1RAWMESSAGE
{
	uint32_t sig_chan_num;
	uint32_t prn;
	uint32_t page_id;
	uint8_t subframe1_data[2];
	uint8_t subframe2_data[72];
	uint8_t subframe3_data[30];
};

struct BDSBCNAV2RAWMESSAGE
{
	uint32_t sig_chan_num;
	uint32_t prn;
	uint32_t message_type;
	uint8_t raw_message_data[36];
};

struct BDSBCNAV3RAWMESSAGE
{
	uint32_t signal_channel;
	uint32_t satellite_id;
	uint32_t message_id;
	uint8_t raw_message_data[62];
	char char_as_int;
	char char_as_int_5;
};

struct BDSBCNAV3EPHEMERIS
{
	uint32_t satellite_id;
	uint32_t wn;
	uint32_t sat_status;
	uint32_t toe;
	uint32_t sat_type;
	double delta_a;
	double a_dot;
	double delta_n;
	double n_dot;
	double m0;
	double eccentricity;
	double omega;
	double omega0;
	double i0;
	double omega_dot;
	double i_dot;
	double cis;
	double cic;
	double crs;
	double crc;
	double cus;
	double cuc;
	uint32_t toc;
	double a0;
	double a1;
	double a2;
	double tgdb2b_i;
	uint32_t BDSBCNAV3EPHEMERIS_reserved;
};

struct LOCKOUTSATELLITE
{
	int32_t system;
	uint32_t system_id;
};

struct UNLOCKOUTSATELLITE
{
	int32_t system;
	uint32_t system_id;
};

struct INSATT
{
	uint32_t week;
	double seconds;
	double roll;
	double pitch;
	double azimuth;
	int32_t ins_solution_status;
};

struct INSPOS
{
	uint32_t week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	int32_t ins_solution_status;
};

struct INSSPD
{
	uint32_t week;
	double seconds;
	double ground_track;
	double horizontal_speed;
	double up_vel;
	int32_t ins_solution_status;
};

struct INSVEL
{
	uint32_t week;
	double seconds;
	double north_vel;
	double east_vel;
	double up_vel;
	int32_t ins_solution_status;
};

struct RAWIMU
{
	uint32_t gps_week;
	double gps_seconds;
	uint32_t imu_status;
	int32_t accel_z;
	int32_t accel_y;
	int32_t accel_x;
	int32_t gyro_z;
	int32_t gyro_y;
	int32_t gyro_x;
};

struct INSATTS
{
	uint32_t week;
	double seconds;
	double roll;
	double pitch;
	double azimuth;
	int32_t ins_solution_status;
};

struct INSPOSS
{
	uint32_t week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	int32_t ins_solution_status;
};

struct INSSPDS
{
	uint32_t week;
	double seconds;
	double ground_track;
	double horizontal_speed;
	double up_vel;
	int32_t ins_solution_status;
};

struct INSVELS
{
	uint32_t week;
	double seconds;
	double north_vel;
	double east_vel;
	double up_vel;
	int32_t ins_solution_status;
};

struct RAWIMUS
{
	uint32_t gps_week;
	double gps_seconds;
	uint32_t imu_status;
	int32_t accel_z;
	int32_t accel_y;
	int32_t accel_x;
	int32_t gyro_z;
	int32_t gyro_y;
	int32_t gyro_x;
};

struct INSCOMMAND
{
	int32_t command;
};

struct INSZUPT
{
	bool perform_zupt;
};

struct INSPVA
{
	uint32_t week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	double north_vel;
	double east_vel;
	double up_vel;
	double roll;
	double pitch;
	double azimuth;
	int32_t ins_solution_status;
};

struct INSPVAS
{
	uint32_t week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	double north_vel;
	double east_vel;
	double up_vel;
	double roll;
	double pitch;
	double azimuth;
	int32_t ins_solution_status;
};

struct CORRIMUDATA
{
	uint32_t week;
	double seconds;
	double corr_wb_ib[3];
	double corr_fb[3];
};

struct CORRIMUDATAS
{
	uint32_t week;
	double seconds;
	double corr_wb_ib[3];
	double corr_fb[3];
};

struct SETINITAZIMUTH
{
	double azimuth;
	float azimuth_std_dev;
};

struct MARK1PVA
{
	uint32_t week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	double north_vel;
	double east_vel;
	double up_vel;
	double roll;
	double pitch;
	double azimuth;
	int32_t ins_solution_status;
};

struct MARK2PVA
{
	uint32_t week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	double north_vel;
	double east_vel;
	double up_vel;
	double roll;
	double pitch;
	double azimuth;
	int32_t ins_solution_status;
};

struct MARK3PVA
{
	uint32_t week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	double north_vel;
	double east_vel;
	double up_vel;
	double roll;
	double pitch;
	double azimuth;
	int32_t ins_solution_status;
};

struct MARK4PVA
{
	uint32_t week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	double north_vel;
	double east_vel;
	double up_vel;
	double roll;
	double pitch;
	double azimuth;
	int32_t ins_solution_status;
};

struct ALIGNMENTMODE
{
	int32_t alignment_mode;
};

struct TILTDATA
{
	uint32_t status;
	double x_accel;
	double y_accel;
	double avg_x_incl;
	double x_incl;
	double y_incl;
	double avg_y_incl;
	uint32_t ulong;
};

struct TILTFILTER
{
	int32_t enable;
	float antenna_height;
	uint32_t TILTFILTER_reserved1;
	uint32_t TILTFILTER_reserved2;
};

struct TILTZERO
{
	int32_t tilt_zero_action;
};

struct TAGNEXTMARK
{
	int32_t mark;
	uint32_t tag;
};

struct TAGGEDMARK1PVA
{
	uint32_t week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	double north_vel;
	double east_vel;
	double up_vel;
	double roll;
	double pitch;
	double azimuth;
	int32_t ins_solution_status;
	uint32_t tag_id;
};

struct TAGGEDMARK2PVA
{
	uint32_t week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	double north_vel;
	double east_vel;
	double up_vel;
	double roll;
	double pitch;
	double azimuth;
	int32_t ins_solution_status;
	uint32_t tag_id;
};

struct SETIMUSPECS
{
	uint32_t data_rate;
	double accel_bias;
	double accel_vrw;
	double gyro_bias;
	double gyro_arw;
	uint32_t accel_scale_factor_error;
	uint32_t gyro_scale_factor_error;
	double data_latency;
	int32_t scale_factor;
};

struct IMURATEPVAS
{
	uint32_t week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	double north_vel;
	double east_vel;
	double up_vel;
	double roll;
	double pitch;
	double azimuth;
	int32_t ins_solution_status;
};

struct INPUTGIMBALANGLE
{
	double euler[3];
	double euler_stdev[3];
};

struct VARIABLELEVERARM
{
	double offsets[3];
	double stdevs[3];
};

struct GIMBALLEDPVA
{
	uint32_t week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	double north_vel;
	double east_vel;
	double up_vel;
	double roll;
	double pitch;
	double azimuth;
	int32_t ins_solution_status;
};

struct TAGGEDMARK3PVA
{
	uint32_t week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	double north_vel;
	double east_vel;
	double up_vel;
	double roll;
	double pitch;
	double azimuth;
	int32_t ins_solution_status;
	uint32_t tag_id;
};

struct TAGGEDMARK4PVA
{
	uint32_t week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	double north_vel;
	double east_vel;
	double up_vel;
	double roll;
	double pitch;
	double azimuth;
	int32_t ins_solution_status;
	uint32_t tag_id;
};

struct SETUPSENSOR
{
	int32_t sensor_id;
	int32_t span_event_out;
	int32_t span_evt_out_polarity;
	uint32_t span_evt_out_active_pulse_ms;
	int32_t span_event_in;
	int32_t span_evt_in_enable_disable;
	int32_t span_evt_in_polarity;
	int32_t span_evt_in_time_bias_ns;
	uint32_t span_evt_in_time_guard_ms;
};

struct TIMEDEVENTPULSE
{
	int32_t selected_sensors;
	uint32_t target_week;
	double target_seconds;
	uint32_t tag;
};

struct IMURATECORRIMUS
{
	uint32_t week;
	double seconds;
	double pitch;
	double roll;
	double azimuth;
	double lateral;
	double longitudinal;
	double vertical;
};

struct HEAVE
{
	uint32_t week;
	double seconds;
	double heave;
};

struct SETHEAVEWINDOW
{
	int32_t heave_window;
};

struct SETALIGNMENTVEL
{
	double min_velocity;
};

struct CONNECTIMU
{
	int32_t imu_port;
	int32_t imu_type;
};

struct RELINSPVA
{
	int32_t rel_ins_output;
	double delta_pos_n;
	double delta_pos_e;
	double delta_pos_u;
	double delta_vel_n;
	double delta_vel_e;
	double delta_vel_u;
	double delta_roll;
	double delta_pitch;
	double delta_heading;
	float diff_age;
	uint8_t rover_id[4];
	int32_t rover_ins_status;
	uint8_t master_id[4];
	int32_t master_ins_status;
	int32_t rtk_baseline_status;
	uint32_t extended_sol_stat;
};

struct INSTHRESHOLDS
{
	int32_t ins_thresholds;
	double INSTHRESHOLDS_reserved1_threshold;
	double INSTHRESHOLDS_reserved2_threshold;
	double INSTHRESHOLDS_reserved3_threshold;
};

struct TSS1
{
	int32_t ins_solution_status;
	double x;
	double y;
	double z;
	double heave;
	double roll;
	double pitch;
	uint32_t week;
	double seconds;
};

struct INSATTX
{
	int32_t ins_solution_status;
	int32_t position_type;
	double roll;
	double pitch;
	double azimuth;
	float roll_std_dev;
	float pitch_std_dev;
	float azimuth_std_dev;
	uint32_t extended_sol_stat;
	uint16_t time_since_update;
};

struct INSVELX
{
	int32_t ins_solution_status;
	int32_t position_type;
	double north_vel;
	double east_vel;
	double up_vel;
	float north_vel_std_dev;
	float east_vel_std_dev;
	float up_vel_std_dev;
	uint32_t extended_sol_stat;
	uint16_t time_since_update;
};

struct INSPOSX
{
	int32_t ins_solution_status;
	int32_t position_type;
	double latitude;
	double longitude;
	double orthometric_height;
	float undulation;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	uint32_t extended_sol_stat;
	uint16_t time_since_update;
};

struct RAWIMUX
{
	uint8_t imu_status_info;
	uint8_t imu_type;
	uint16_t gps_week;
	double gps_seconds;
	uint32_t imu_status;
	int32_t accel_z;
	int32_t accel_y;
	int32_t accel_x;
	int32_t gyro_z;
	int32_t gyro_y;
	int32_t gyro_x;
};

struct RAWIMUSX
{
	uint8_t imu_status_info;
	uint8_t imu_type;
	uint16_t gps_week;
	double gps_seconds;
	uint32_t imu_status;
	int32_t accel_z;
	int32_t accel_y;
	int32_t accel_x;
	int32_t gyro_z;
	int32_t gyro_y;
	int32_t gyro_x;
};

struct EXTERNALPVAS
{
	double position[3];
	float velocity[3];
	float attitude[3];
	float position_stdev[3];
	float velocity_stdev[3];
	float attitude_stdev[3];
	uint32_t update_type;
	uint32_t options;
};

struct INSPVAX
{
	int32_t ins_solution_status;
	int32_t position_type;
	double latitude;
	double longitude;
	double orthometric_height;
	float undulation;
	double north_vel;
	double east_vel;
	double up_vel;
	double roll;
	double pitch;
	double azimuth;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	float north_vel_std_dev;
	float east_vel_std_dev;
	float up_vel_std_dev;
	float roll_std_dev;
	float pitch_std_dev;
	float azimuth_std_dev;
	uint32_t extended_sol_stat;
	uint16_t time_since_update;
};

struct SYNCHEAVE
{
	double heave;
	double heave_std_dev;
};

struct DELAYEDHEAVE
{
	double heave;
	double heave_std_dev;
};

struct SYNCRELINSPVA
{
	int32_t rel_ins_output;
	double delta_pos_n;
	double delta_pos_e;
	double delta_pos_u;
	double delta_vel_n;
	double delta_vel_e;
	double delta_vel_u;
	double delta_roll;
	double delta_pitch;
	double delta_heading;
	float diff_age;
	uint8_t rover_id[4];
	int32_t rover_ins_status;
	uint8_t master_id[4];
	int32_t master_ins_status;
	int32_t rtk_baseline_status;
	uint32_t extended_sol_stat;
};

struct RELINSAUTOMATION
{
	int32_t enable;
	int32_t port;
	uint32_t baud_rate;
	uint32_t req_rate_in_hz;
	int32_t send_heading_ext;
	int32_t interface_mode;
};

struct SETIMUPORTPROTOCOL
{
	int32_t port_protocol;
};

struct SETRELINSOUTPUTFRAME
{
	int32_t rel_ins_output;
	bool from_master;
};

struct IMURATEPVA
{
	uint32_t week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	double north_vel;
	double east_vel;
	double up_vel;
	double roll;
	double pitch;
	double azimuth;
	int32_t ins_solution_status;
};

struct RELINSCONFIG
{
	int32_t enable;
	int32_t rel_ins_rx;
	int32_t port;
	uint32_t baud_rate;
	uint32_t req_rate_in_hz;
};

struct SETMAXALIGNMENTTIME
{
	int32_t enable;
	uint32_t max_align_seconds;
};

struct SETINSUPDATE
{
	int32_t ins_update;
	int32_t trigger;
};

struct INSUPDATESTATUS
{
	int32_t pos_type;
	int32_t num_psr;
	int32_t num_phase;
	int32_t num_dop;
	int32_t wheel_status;
	int32_t heading_update_status;
	uint32_t ext_solution_status;
	uint32_t ins_update_options;
	uint32_t INSUPDATESTATUS_reserved1;
	uint32_t INSUPDATESTATUS_reserved2;
};

struct INSCALIBRATE
{
	int32_t offset;
	int32_t trigger;
	float stdev_threshold;
};

struct INSPVASDCMP
{
	uint16_t week;
	uint32_t week_milliseconds;
	uint16_t latitude_std_dev;
	uint16_t longitude_std_dev;
	uint16_t height_std_dev;
	uint16_t velocity_north_std_dev;
	uint16_t velocity_east_std_dev;
	uint16_t velocity_up_std_dev;
	uint16_t roll_std_dev;
	uint16_t pitch_std_dev;
	uint16_t azimuth_std_dev;
	uint8_t time_since_pos_update;
	uint8_t gnss_position_update_type;
	uint32_t extended_sol_stat;
	uint8_t align_age;
	uint32_t milliseconds;
};

struct INSSEED
{
	int32_t ins_seed;
	int32_t injection_option;
	uint32_t INSSEED_reserved1;
	uint32_t INSSEED_reserved2;
};

struct SETINSTRANSLATION
{
	int32_t offset;
	float elements[3];
	float elements_2[3];
	int32_t input_frame;
};

struct SETINSROTATION
{
	int32_t offset;
	float elements[3];
	float elements_2[3];
	uint32_t SETINSROTATION_reserved;
};

struct SETINSPROFILE
{
	int32_t ins_profile;
};

struct INSCONFIG_ins_translations
{
	int32_t ins_offset;
	int32_t input_frame;
	float offset[3];
	float offset_stdev[3];
	int32_t source_status;
};

struct INSCONFIG_ins_rotations
{
	int32_t ins_offset_26;
	int32_t input_frame_27;
	float offset_28[3];
	float offset_stdev_29[3];
	int32_t source_status_30;
};

struct INSCONFIG
{
	int32_t imu_type;
	uint8_t mapping;
	uint8_t scaled_alignment_vel;
	uint16_t heave_window;
	int32_t ins_profile;
	uint32_t enabled_updates;
	int32_t alignment_mode;
	int32_t rel_ins_output;
	bool rel_from_master;
	uint32_t ins_rx_status;
	uint8_t ins_seed;
	uint8_t ins_seed_injection_option;
	uint16_t ins_grade_mode;
	uint32_t INSCONFIG_reserved2;
	uint32_t ins_logistics;
	uint32_t INSCONFIG_reserved4;
	uint32_t INSCONFIG_reserved5;
	uint32_t INSCONFIG_reserved6;
	uint32_t INSCONFIG_reserved7;
	uint32_t ins_translations_arraylength;
	INSCONFIG_ins_translations ins_translations[11];
	uint32_t ins_rotations_arraylength;
	INSCONFIG_ins_rotations ins_rotations[9];
};

struct INSCALSTATUS
{
	int32_t ins_offset;
	float calibration[3];
	float calibration_stdev[3];
	int32_t source_status;
	uint32_t calibration_count;
};

struct SETIMUEVENT
{
	int32_t imu_event_direction;
	int32_t imu_event_line;
};

struct INSSTDEV
{
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	float north_vel_std_dev;
	float east_vel_std_dev;
	float up_vel_std_dev;
	float roll_std_dev;
	float pitch_std_dev;
	float azimuth_std_dev;
	uint32_t extended_sol_stat;
	uint16_t time_since_update;
	uint16_t ushort;
	uint32_t enabled_updates;
	uint32_t ulong;
};

struct INSSTDEVS
{
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	float north_vel_std_dev;
	float east_vel_std_dev;
	float up_vel_std_dev;
	float roll_std_dev;
	float pitch_std_dev;
	float azimuth_std_dev;
	uint32_t extended_sol_stat;
	uint16_t time_since_update;
	uint16_t ushort;
	uint32_t enabled_updates;
	uint32_t ulong;
};

struct INSATTQS
{
	uint32_t week;
	double seconds;
	double quaternion[4];
	int32_t ins_solution_status;
};

struct INSSEEDSTATUS
{
	int32_t ins_seed_status;
	int32_t ins_seed_validity;
	float att[3];
	double xyz[3];
	float undulation;
	uint32_t INSSEEDSTATUS_reserved1;
	uint32_t INSSEEDSTATUS_reserved2;
	uint32_t INSSEEDSTATUS_reserved3;
	uint32_t INSSEEDSTATUS_reserved4;
};

struct INSALIGNCONFIG
{
	int32_t master_port;
	int32_t rover_port;
	uint32_t baud_rate;
	uint32_t req_rate_in_hz;
	uint32_t INSALIGNCONFIG_reserved;
};

struct SETALIGNMENTORIENTATION
{
	int32_t init_verification_state;
	double angle_threshold;
};

struct CORRIMUS
{
	uint32_t accum_count;
	double corr_wb_ib[3];
	double corr_fb[3];
	float latency_ms;
	uint32_t ulong;
};

struct RAWDMI
{
	int32_t values[4];
	uint32_t mask;
};

struct DMICONFIG
{
	int32_t dmi;
	int32_t enable;
	int32_t dmi_source;
	int32_t high_rate_event;
};

struct TILTSTATUS
{
	uint32_t compensation_status;
	double pitch;
	double roll;
	double x;
	double y;
	double z;
	double azimuth;
	uint32_t ulong;
};

struct INSVELUSER
{
	int32_t ins_solution_status;
	double north_vel;
	double east_vel;
	double up_vel;
	float north_vel_std_dev;
	float east_vel_std_dev;
	float up_vel_std_dev;
	float slip_angle;
	uint32_t ulong;
	uint32_t ulong_9;
	uint32_t ext_vel_status;
};

struct INSDATUMINFO
{
	uint8_t name[32];
	uint32_t epsg_code;
	double epoch;
	int32_t transformation_status;
	float INSDATUMINFO_reserved;
	uint32_t INSDATUMINFO_reserved_5;
};

struct CLOCKMODEL
{
	int32_t status;
	uint32_t reject_count;
	uint32_t milliseconds;
	uint32_t milliseconds_3;
	double par[3];
	double data[9];
	double inst_range_bias;
	double inst_drift;
	bool constellation_change;
};

struct BESTPOS
{
	int32_t solution_status;
	int32_t position_type;
	double latitude;
	double longitude;
	double orthometric_height;
	float undulation;
	int32_t datum_id;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	uint8_t base_id[4];
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	uint8_t extended_solution_status2;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct FIX
{
	int32_t fix_cmd;
	double param1;
	double param2;
	double param3;
};

struct PSRPOS
{
	int32_t solution_status;
	int32_t position_type;
	double latitude;
	double longitude;
	double orthometric_height;
	float undulation;
	int32_t datum_id;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	uint8_t base_id[4];
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	char char_as_int;
	char char_as_int_16;
	uint8_t extended_solution_status2;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct USERDATUM
{
	double semi_major_axis;
	double inverse_flattening;
	double x;
	double y;
	double z;
	double x_5;
	double y_6;
	double z_7;
	double scale;
};

struct RTKELEVMASK
{
	int32_t elev_mask_type;
	float angle;
};

struct RTKSVENTRIES
{
	uint32_t max_sv;
};

struct MATCHEDPOS
{
	int32_t solution_status;
	int32_t position_type;
	double latitude;
	double longitude;
	double orthometric_height;
	float undulation;
	int32_t datum_id;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	uint8_t base_id[4];
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	uint8_t extended_solution_status2;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct RTKCOMMAND
{
	int32_t command;
};

struct BESTVEL
{
	int32_t status;
	int32_t type;
	float latency;
	float diff_age;
	double horizontal_speed;
	double ground_track;
	double z;
	int32_t rsvd_field_for_velocity_logs;
};

struct PSRVEL
{
	int32_t velocity_status;
	int32_t velocity_type;
	float latency;
	float diff_age;
	double horizontal_speed;
	double ground_track;
	double z;
	int32_t rsvd_field_for_velocity_logs;
};

struct SETRTCM16
{
	uint8_t text[90];
};

struct RTKPOS
{
	int32_t solution_status;
	int32_t position_type;
	double latitude;
	double longitude;
	double orthometric_height;
	float undulation;
	int32_t datum_id;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	uint8_t base_id[4];
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	uint8_t extended_solution_status2;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct DGPSTXID
{
	int32_t dgps_type;
	uint8_t dgps_id[5];
};

struct DATUM
{
	int32_t datum_id;
};

struct NAVIGATE
{
	int32_t solution_status;
	int32_t position_type;
	int32_t velocity_status;
	int32_t nav_status;
	double distance;
	double bearing;
	double along_track;
	double x_track;
	uint32_t eta_weeks;
	double eta_seconds;
};

struct SETNAV
{
	double from_latitude;
	double from_longitude;
	double to_latitude;
	double to_longitude;
	double track_offset;
	uint8_t from_point[6];
	uint8_t to_point[6];
};

struct AVEPOS
{
	double latitude;
	double longitude;
	double orthometric_height;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	int32_t ave_status;
	uint32_t ave_time;
	uint32_t num_sample;
};

struct POSAVE
{
	int32_t command;
	float max_time;
	float max_horiz;
	float max_height;
};

struct PSRDOP
{
	float gdop;
	float pdop;
	float hdop;
	float htdop;
	float tdop;
	float gps_elev_mask;
	uint32_t sats_arraylength;
	uint32_t sats[325];
};

struct REFSTATION
{
	uint32_t ref_status;
	double ecef_x;
	double ecef_y;
	double ecef_z;
	uint32_t health;
	int32_t ref_type;
	uint8_t ref_id[5];
};

struct MAGVAR
{
	int32_t mag_var_type;
	float correction;
	float correction_std_dev;
};

struct MARKPOS
{
	int32_t solution_status;
	int32_t position_type;
	double latitude;
	double longitude;
	double orthometric_height;
	float undulation;
	int32_t datum_id;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	uint8_t base_id[4];
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	char char_as_int;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct RTKDYNAMICS
{
	int32_t dynamics;
};

struct UNDULATION
{
	int32_t undulation_type;
	float undulation;
};

struct RTKVEL
{
	int32_t velocity_status;
	int32_t velocity_type;
	float latency;
	float diff_age;
	double horizontal_speed;
	double ground_track;
	double z;
	int32_t rsvd_field_for_velocity_logs;
};

struct BESTXYZ
{
	int32_t solution_status;
	int32_t position_type;
	double x;
	double y;
	double z;
	float x_std_dev;
	float y_std_dev;
	float z_std_dev;
	int32_t status;
	int32_t type;
	double x_10;
	double y_11;
	double z_12;
	float x_std_dev_13;
	float y_std_dev_14;
	float z_std_dev_15;
	uint8_t base_id[4];
	float latency;
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	char char_as_int;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct MATCHEDXYZ
{
	int32_t solution_status;
	int32_t position_type;
	double x;
	double y;
	double z;
	float x_std_dev;
	float y_std_dev;
	float z_std_dev;
	uint8_t base_id[4];
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	char char_as_int;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct PSRXYZ
{
	int32_t solution_status;
	int32_t position_type;
	double x;
	double y;
	double z;
	float x_std_dev;
	float y_std_dev;
	float z_std_dev;
	int32_t velocity_status;
	int32_t velocity_type;
	double x_10;
	double y_11;
	double z_12;
	float x_std_dev_13;
	float y_std_dev_14;
	float z_std_dev_15;
	uint8_t base_id[4];
	float latency;
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	char char_as_int;
	char char_as_int_23;
	char char_as_int_24;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct RTKXYZ
{
	int32_t solution_status;
	int32_t position_type;
	double x;
	double y;
	double z;
	float x_std_dev;
	float y_std_dev;
	float z_std_dev;
	int32_t velocity_status;
	int32_t velocity_type;
	double x_10;
	double y_11;
	double z_12;
	float x_std_dev_13;
	float y_std_dev_14;
	float z_std_dev_15;
	uint8_t base_id[4];
	float latency;
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	char char_as_int;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct SETAPPROXPOS
{
	double lat;
	double lon;
	double hgt;
};

struct PDPFILTER
{
	int32_t command;
};

struct PDPPOS
{
	int32_t solution_status;
	int32_t position_type;
	double latitude;
	double longitude;
	double orthometric_height;
	float undulation;
	int32_t datum_id;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	uint8_t base_id[4];
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	char char_as_int;
	char char_as_int_16;
	uint8_t extended_solution_status2;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct PDPVEL
{
	int32_t velocity_status;
	int32_t velocity_type;
	float latency;
	float diff_age;
	double horizontal_speed;
	double ground_track;
	double z;
	int32_t rsvd_field_for_velocity_logs;
};

struct PDPXYZ
{
	int32_t solution_status;
	int32_t position_type;
	double x;
	double y;
	double z;
	float x_std_dev;
	float y_std_dev;
	float z_std_dev;
	int32_t velocity_status;
	int32_t velocity_type;
	double x_10;
	double y_11;
	double z_12;
	float x_std_dev_13;
	float y_std_dev_14;
	float z_std_dev_15;
	uint8_t base_id[4];
	float latency;
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	char char_as_int;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct PSRDIFFSOURCE
{
	int32_t corr_type;
	uint8_t station_id[8];
};

struct RTKSOURCE
{
	int32_t corr_type;
	uint8_t station_id[8];
};

struct POSTIMEOUT
{
	uint32_t time_out;
};

struct MARK2POS
{
	int32_t solution_status;
	int32_t position_type;
	double latitude;
	double longitude;
	double orthometric_height;
	float undulation;
	int32_t datum_id;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	uint8_t base_id[4];
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	char char_as_int;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct SBASCONTROL
{
	int32_t enable;
	int32_t system;
	uint32_t prn;
	int32_t test_mode;
};

struct BSLNXYZ
{
	int32_t solution_status;
	int32_t position_type;
	double x;
	double y;
	double z;
	float x_std_dev;
	float y_std_dev;
	float z_std_dev;
	uint8_t base_id[4];
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	char char_as_int;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct SETDIFFCODEBIASES
{
	int32_t code_pair;
	float biases[40];
};

struct GGAQUALITY_mappings
{
	int32_t sol_type;
	uint32_t gga_quality;
};

struct GGAQUALITY
{
	uint32_t mappings_arraylength;
	GGAQUALITY_mappings mappings[20];
};

struct SETIONOTYPE
{
	int32_t model;
};

struct BESTUTM
{
	int32_t solution_status;
	int32_t position_type;
	uint32_t zone_number;
	uint32_t zone_letter;
	double northing;
	double easting;
	double height;
	float undulation;
	int32_t datum_id;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	uint8_t base_id[4];
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	char char_as_int;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct UTMZONE
{
	int32_t utm_zone_command;
	int32_t param;
};

struct MOVINGBASESTATION
{
	int32_t moving_base_station_enum;
};

struct USEREXPDATUM
{
	double semi_major_axis;
	double inverse_flattening;
	double x;
	double y;
	double z;
	double x_5;
	double y_6;
	double z_7;
	double scale;
	double x_9;
	double y_10;
	double z_11;
	double x_12;
	double y_13;
	double z_14;
	double scale_rate;
	double reference_date;
};

struct SETBESTPOSCRITERIA
{
	int32_t selection_type;
	uint32_t num_epochs;
};

struct RTKQUALITYLEVEL
{
	int32_t rtk_quality_level;
};

struct RTKANTENNA
{
	int32_t position_reference_enum;
	int32_t pcv_model_enum;
	bool RTKANTENNA_reserved1;
	bool RTKANTENNA_reserved2;
};

struct LOCKOUTSYSTEM
{
	int32_t system;
};

struct SETRTCM36
{
	uint8_t text[90];
};

struct PSRTIME_system_offsets
{
	int32_t system;
	double bias;
	double bias_std_dev;
};

struct PSRTIME
{
	uint32_t system_offsets_arraylength;
	PSRTIME_system_offsets system_offsets[5];
};

struct UNLOCKOUTSYSTEM
{
	int32_t system;
};

struct RTKTIMEOUT
{
	uint32_t rtk_time_out;
};

struct DIFFCODEBIASCONTROL
{
	int32_t enable_enum;
};

struct RTKNETWORK
{
	int32_t rtk_network;
	uint32_t network_number;
};

struct RTKDOP
{
	float gdop;
	float pdop;
	float hdop;
	float htdop;
	float tdop;
	float gps_elev_mask;
	uint32_t sats_arraylength;
	uint32_t sats[325];
};

struct PDPMODE
{
	int32_t mode;
	int32_t dynamics;
};

struct HEADING
{
	int32_t solution_status;
	int32_t position_type;
	float b_length;
	float heading;
	float pitch;
	float float_1;
	float heading_std_dev;
	float pitch_std_dev;
	uint8_t base_id[4];
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	uint8_t extended_solution_status2;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct SBASTIMEOUT
{
	int32_t time_out_enum;
	double time_out;
	double SBASTIMEOUT_reserved;
};

struct MASTERPOS
{
	int32_t solution_status;
	int32_t position_type;
	double latitude;
	double longitude;
	double orthometric_height;
	float undulation;
	int32_t datum_id;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	uint8_t base_id[4];
	float float_1;
	float float_1_12;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	uint8_t extended_solution_status2;
	char char_as_int;
	char char_as_int_19;
	char char_as_int_20;
};

struct ROVERPOS
{
	int32_t solution_status;
	int32_t position_type;
	double latitude;
	double longitude;
	double orthometric_height;
	float undulation;
	int32_t datum_id;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	uint8_t base_id[4];
	float float_1;
	float float_1_12;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	uint8_t extended_solution_status2;
	char char_as_int;
	char char_as_int_19;
	char char_as_int_20;
};

struct HDTOUTTHRESHOLD
{
	float heading_std_threshold;
};

struct HEADINGOFFSET
{
	float heading_offset;
	float pitch_offset;
};

struct HEADINGEXT
{
	uint8_t solution_status;
	uint8_t solution_type;
	float heading_offset;
	float pitch_offset;
	double x;
	double y;
	double z;
	double x_var;
	double y_var;
	double z_var;
	double xy_var;
	double yz_var;
	double zx_var;
	uint8_t master_id[4];
	uint8_t rover_id[4];
	float undulation;
	uint8_t num_sats_tracked;
	uint8_t num_sats_in_sol;
	uint8_t num_high_sats;
	uint8_t num_high_l2_sats;
	uint8_t ext_sol_stat;
	uint8_t gp_sand_glo_freqs_in_sol;
};

struct SETROVERID
{
	uint8_t id[5];
};

struct OUTPUTDATUM
{
	uint8_t name[32];
	int32_t epoch_option;
	double epoch;
};

struct PSRSATS_satellite_entries
{
	int32_t system_type;
	uint32_t id;
	int32_t status;
	uint32_t status_mask;
};

struct PSRSATS
{
	uint32_t satellite_entries_arraylength;
	PSRSATS_satellite_entries satellite_entries[325];
};

struct PSRDOP2_tdo_ps
{
	int32_t system;
	float dop;
};

struct PSRDOP2
{
	float gdop;
	float pdop;
	float hdop;
	float vdop;
	uint32_t tdo_ps_arraylength;
	PSRDOP2_tdo_ps tdo_ps[5];
};

struct RTKDOP2_tdo_ps
{
	int32_t system;
	float dop;
};

struct RTKDOP2
{
	float gdop;
	float pdop;
	float hdop;
	float vdop;
	uint32_t tdo_ps_arraylength;
	RTKDOP2_tdo_ps tdo_ps[5];
};

struct RTKSATS_satellite_entries
{
	int32_t system_type;
	uint32_t id;
	int32_t status;
	uint32_t status_mask;
};

struct RTKSATS
{
	uint32_t satellite_entries_arraylength;
	RTKSATS_satellite_entries satellite_entries[325];
};

struct MATCHEDSATS_satellite_entries
{
	int32_t system_type;
	uint32_t id;
	int32_t status;
	uint32_t status_mask;
};

struct MATCHEDSATS
{
	uint32_t satellite_entries_arraylength;
	MATCHEDSATS_satellite_entries satellite_entries[325];
};

struct BESTSATS_satellite_entries
{
	int32_t system_type;
	uint32_t id;
	int32_t status;
	uint32_t status_mask;
};

struct BESTSATS
{
	uint32_t satellite_entries_arraylength;
	BESTSATS_satellite_entries satellite_entries[325];
};

struct IONOCONDITION
{
	int32_t iono_condition;
};

struct SETRTCMRXVERSION
{
	int32_t version;
};

struct PDPSATS_satellite_entries
{
	int32_t system_type;
	uint32_t id;
	int32_t status;
	uint32_t status_mask;
};

struct PDPSATS
{
	uint32_t satellite_entries_arraylength;
	PDPSATS_satellite_entries satellite_entries[325];
};

struct GENERATERTKCORRECTIONS
{
	int32_t correction_type_enum;
	int32_t port;
};

struct RAIMMODE
{
	int32_t mode;
	double hal;
	double val;
	double pfa;
};

struct RAIMSTATUS_rejected_s_vs
{
	int32_t system_type;
	uint32_t id;
};

struct RAIMSTATUS
{
	int32_t mode;
	int32_t integrity_status;
	int32_t hpl_status;
	double hpl;
	int32_t vpl_status;
	double vpl;
	uint32_t rejected_s_vs_arraylength;
	RAIMSTATUS_rejected_s_vs rejected_s_vs[20];
};

struct GENERATEDIFFCORRECTIONS
{
	int32_t interface_mode_enum;
	int32_t port;
};

struct ALIGNBSLNXYZ
{
	int32_t solution_status;
	int32_t position_type;
	double x;
	double y;
	double z;
	float x_std_dev;
	float y_std_dev;
	float z_std_dev;
	uint8_t rover_id[4];
	uint8_t base_id[4];
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	uint8_t extended_solution_status2;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct ALIGNBSLNENU
{
	int32_t solution_status;
	int32_t solution_type;
	double longitude;
	double latitude;
	double height;
	float longitude_std_dev;
	float latitude_std_dev;
	float height_std_dev;
	uint8_t rover_id[4];
	uint8_t master_id[4];
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	uint8_t measurement_source;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct HEADINGSATS_satellite_entries
{
	int32_t system_type;
	uint32_t id;
	int32_t status;
	uint32_t status_mask;
};

struct HEADINGSATS
{
	uint32_t satellite_entries_arraylength;
	HEADINGSATS_satellite_entries satellite_entries[325];
};

struct SETRTCMTXVERSION
{
	int32_t version;
};

struct ALIGNAUTOMATION
{
	int32_t enable;
	int32_t port;
	uint32_t baud_rate;
	uint32_t req_rate_in_hz;
	int32_t send_heading_ext;
	int32_t interface_mode;
};

struct REFSTATIONINFO
{
	double latitude;
	double longitude;
	double height;
	int32_t datum;
	float arp_height;
	uint32_t health;
	int32_t ref_type;
	uint8_t station_id[5];
	uint8_t antenna_model[32];
	uint8_t antenna_serial[32];
};

struct ALIGNDOP
{
	float gdop;
	float pdop;
	float hdop;
	float htdop;
	float tdop;
	float gps_elev_mask;
	uint32_t sats_arraylength;
	uint32_t sats[325];
};

struct HEADING2
{
	int32_t solution_status;
	int32_t position_type;
	float b_length;
	float heading;
	float pitch;
	float float_1;
	float heading_std_dev;
	float pitch_std_dev;
	uint8_t rover_id[4];
	uint8_t base_id[4];
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	uint8_t extended_solution_status2;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct GENERATEALIGNCORRECTIONS
{
	int32_t port;
	uint32_t baud_rate;
	uint32_t obs_rate_in_hz;
	uint32_t ref_rate_in_hz;
	int32_t correction_interface;
};

struct SETBASERECEIVERTYPE
{
	int32_t receiver_type;
};

struct BASEANTENNAPCO
{
	int32_t frequency;
	double offset[3];
	int32_t corr_type ;
	uint8_t station_id[8];
};

struct BASEANTENNAPCV
{
	int32_t frequency;
	double pcv[19];
	int32_t corr_type ;
	uint8_t station_id[8];
};

struct THISANTENNAPCO
{
	int32_t frequency;
	double offset[3];
};

struct THISANTENNAPCV
{
	int32_t frequency;
	double pcv[19];
};

struct BASEANTENNATYPE
{
	int32_t antenna_type;
	int32_t antenna_raydom;
	int32_t corr_type;
	uint8_t station_id[8];
};

struct THISANTENNATYPE
{
	int32_t antenna_type;
	int32_t antenna_raydom;
};

struct SBASALMANAC
{
	uint32_t satellite_id;
	int32_t system_variant;
	uint32_t t0;
	uint16_t data_id;
	uint16_t health;
	int32_t x;
	int32_t y;
	int32_t z;
	int32_t x_vel;
	int32_t y_vel;
	int32_t z_vel;
};

struct BESTGNSSPOS
{
	int32_t solution_status;
	int32_t position_type;
	double latitude;
	double longitude;
	double orthometric_height;
	float undulation;
	int32_t datum_id;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	uint8_t base_id[4];
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	uint8_t extended_solution_status2;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct BESTGNSSVEL
{
	int32_t velocity_status;
	int32_t velocity_type;
	float latency;
	float diff_age;
	double horizontal_speed;
	double ground_track;
	double z;
	float float_1;
};

struct SETTROPOMODEL
{
	int32_t model;
};

struct RTKSOURCETIMEOUT
{
	int32_t setting;
	uint32_t time_out;
};

struct RTKMATCHEDTIMEOUT
{
	uint32_t timeout;
};

struct PSRDIFFSOURCETIMEOUT
{
	int32_t setting;
	uint32_t time_out;
};

struct PSRDIFFTIMEOUT
{
	uint32_t time_out;
};

struct STEADYLINE
{
	int32_t command;
	int32_t offset_deweight;
};

struct PPPPOS
{
	int32_t solution_status;
	int32_t position_type;
	double latitude;
	double longitude;
	double orthometric_height;
	float undulation;
	int32_t datum_id;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	uint8_t base_id[4];
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	uint8_t extended_solution_status2;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct PPPSATS_satellite_entries
{
	int32_t system_type;
	uint32_t id;
	int32_t status;
	uint32_t status_mask;
};

struct PPPSATS
{
	uint32_t satellite_entries_arraylength;
	PPPSATS_satellite_entries satellite_entries[325];
};

struct PPPRESET
{
	int32_t type;
};

struct PPPXYZ
{
	int32_t solution_status;
	int32_t position_type;
	double x;
	double y;
	double z;
	float x_std_dev;
	float y_std_dev;
	float z_std_dev;
	int32_t velocity_status;
	int32_t velocity_type;
	double x_10;
	double y_11;
	double z_12;
	float x_std_dev_13;
	float y_std_dev_14;
	float z_std_dev_15;
	uint8_t base_id[4];
	float latency;
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	char char_as_int;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct PPPSEED
{
	int32_t mode;
	double latitude_in_degrees;
	double longitude_in_degrees;
	double height;
	float northing_std_dev;
	float easting_std_dev;
	float height_std_dev;
	float undulation;
};

struct PPPDOP2_tdo_ps
{
	int32_t system;
	float dop;
};

struct PPPDOP2
{
	float gdop;
	float pdop;
	float hdop;
	float vdop;
	uint32_t tdo_ps_arraylength;
	PPPDOP2_tdo_ps tdo_ps[5];
};

struct PPPDYNAMICS
{
	int32_t dynamics;
};

struct PPPTIMEOUT
{
	uint32_t time_out;
};

struct PPPCONVERGEDCRITERIA
{
	int32_t criteria;
	float threshold;
};

struct UALCONTROL
{
	int32_t accuracy_level_action;
	double accuracy_level1_upper_bound;
	double accuracy_level2_upper_bound;
};

struct HEADINGEXT2
{
	uint8_t rover_id[4];
	uint8_t master_id[4];
	uint32_t data_arraylength;
	uint8_t data[512];
};

struct BESTVELTYPE
{
	int32_t type;
};

struct HEADINGRATE
{
	int32_t solution_status;
	int32_t solution_type;
	float latency;
	float b_length;
	float heading;
	float pitch;
	float b_length_std_dev;
	float heading_std_dev;
	float pitch_std_dev;
	float float_1;
	uint8_t rover_id[4];
	uint8_t base_id[4];
	uint8_t extended_solution_status2;
	char char_as_int;
	char char_as_int_14;
	char char_as_int_15;
};

struct PPPSOURCE
{
	int32_t type;
};

struct LBANDBEAMTABLE_items
{
	uint8_t name[8];
	uint8_t region_id[8];
	uint32_t frequency_in_hz;
	uint32_t baud_rate;
	float longitude;
	uint32_t beam_access;
};

struct LBANDBEAMTABLE
{
	uint32_t items_arraylength;
	LBANDBEAMTABLE_items items[32];
};

struct TERRASTARINFO
{
	uint8_t pac[16];
	int32_t operating_mode;
	uint32_t subscription_details;
	uint32_t contract_end_day_of_year;
	uint32_t contract_end_year;
	uint32_t timed_enable_period;
	int32_t region_restriction;
	float local_area_center_point_latitude;
	float local_area_center_point_longitude;
	uint32_t local_area_radius;
};

struct VERIPOSINFO
{
	uint32_t serial_number;
	int32_t operating_mode;
	uint32_t subscription_details;
	uint8_t service_code[4];
};

struct TERRASTARSTATUS
{
	int32_t access_status;
	int32_t decoder_sync_state;
	uint32_t timed_enable_remaining_time;
	int32_t local_area_status;
	int32_t geogating_status;
};

struct VERIPOSSTATUS
{
	int32_t access_status;
	int32_t decoder_sync_state;
};

struct MARK3POS
{
	int32_t solution_status;
	int32_t position_type;
	double latitude;
	double longitude;
	double orthometric_height;
	float undulation;
	int32_t datum_id;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	uint8_t base_id[4];
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	char char_as_int;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct MARK4POS
{
	int32_t solution_status;
	int32_t position_type;
	double latitude;
	double longitude;
	double orthometric_height;
	float undulation;
	int32_t datum_id;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
	uint8_t base_id[4];
	float diff_age;
	float solution_age;
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	char char_as_int;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct GLIDEINITIALIZATIONPERIOD
{
	double wait_duration;
};

struct DUALANTENNAALIGN
{
	int32_t enable;
	uint32_t obs_rate_hz;
	uint32_t pos_rate_hz;
};

struct AUTOSURVEY
{
	int32_t sw;
	uint32_t max_time;
	float accuracy;
	float horiz_tol;
	int32_t save_positions_in_nvm;
	uint8_t id[5];
};

struct RTKPORTMODE
{
	int32_t port;
	int32_t port_mode;
};

struct PPPBASICCONVERGEDCRITERIA
{
	int32_t criteria;
	float threshold;
};

struct SAVEDSURVEYPOSITIONS_positions
{
	uint8_t id[5];
	double latitude;
	double longitude;
	double height;
};

struct SAVEDSURVEYPOSITIONS
{
	uint32_t positions_arraylength;
	SAVEDSURVEYPOSITIONS_positions positions[32];
};

struct SURVEYPOSITION
{
	int32_t option;
	uint8_t id[5];
	double latitude;
	double longitude;
	double height;
	double tolerance;
};

struct RTKASSIST
{
	int32_t enable;
};

struct PDPDOP2_tdo_ps
{
	int32_t system;
	float dop;
};

struct PDPDOP2
{
	float gdop;
	float pdop;
	float hdop;
	float vdop;
	uint32_t tdo_ps_arraylength;
	PDPDOP2_tdo_ps tdo_ps[5];
};

struct PPPDOP
{
	float gdop;
	float pdop;
	float hdop;
	float htdop;
	float tdop;
	float gps_elev_mask;
	uint32_t sats_arraylength;
	uint32_t sats[325];
};

struct PDPDOP
{
	float gdop;
	float pdop;
	float hdop;
	float htdop;
	float tdop;
	float gps_elev_mask;
	uint32_t sats_arraylength;
	uint32_t sats[325];
};

struct STEADYLINEDIFFERENTIALTIMEOUT
{
	float time_out;
};

struct RTKASSISTTIMEOUT
{
	int32_t limit_type;
	uint32_t time_out;
};

struct REFERENCESTATIONTIMEOUT
{
	int32_t setting;
	uint32_t time_out;
};

struct DUALANTENNAHEADING
{
	int32_t solution_status;
	int32_t position_type;
	float b_length;
	float heading;
	float pitch;
	float float_1;
	float heading_std_dev;
	float pitch_std_dev;
	uint8_t base_id[4];
	uint8_t num_svs;
	uint8_t num_soln_svs;
	uint8_t num_soln_L1_svs;
	uint8_t num_soln_multi_svs;
	uint8_t extended_solution_status2;
	uint8_t ext_sol_stat;
	uint8_t gal_and_bds_mask;
	uint8_t gps_and_glo_mask;
};

struct GPHDTDUALANTENNA
{
	int32_t solution_status;
	int32_t position_type;
	bool output_hdt;
	float heading;
	uint8_t system_set;
};

struct RTKASSISTSTATUS
{
	int32_t state;
	int32_t status;
	float remaining_time;
	float corrections_age;
};

struct RTKINTEGERCRITERIA
{
	int32_t criteria;
	float threshold;
};

struct PPPDYNAMICSEED
{
	uint32_t week;
	uint32_t seconds ;
	double latitude_in_degrees;
	double longitude_in_degrees;
	double ellipsoidal_height;
	float northing_std_dev;
	float easting_std_dev;
	float height_std_dev;
	float northing_easting_covariance;
	float northing_height_covariance;
	float easting_height_covariance;
};

struct RTKRESET
{
	int32_t type;
};

struct OCEANIXINFO
{
	uint8_t pac[16];
	int32_t operating_mode;
	uint32_t subscription_details;
	uint32_t contract_end_day_of_year;
	uint32_t contract_end_year;
	uint32_t timed_enable_period;
	int32_t region_restriction;
};

struct OCEANIXSTATUS
{
	int32_t access_status;
	int32_t decoder_sync_state;
	int32_t region_restriction_status;
};

struct BESTVELX
{
	int32_t status;
	int32_t vel_type;
	double x;
	double y;
	double z;
	float x_std_dev;
	float y_std_dev;
	float z_std_dev;
	uint32_t rsvd_field;
	uint16_t latency_ms;
};

struct PPPSEEDAPPLICATIONSTATUS
{
	int32_t status;
	double latitude;
	double longitude;
	double height;
	float latitude_std_dev;
	float longitude_std_dev;
	float height_std_dev;
};

struct TECTONICSCOMPENSATIONSOURCE
{
	int32_t velocity_source;
	uint8_t name[32];
};

struct TECTONICSCOMPENSATION
{
	int32_t status;
	uint8_t name[32];
	float x;
	float y;
	float z;
};

struct PPPDATUMINFO
{
	uint8_t name[32];
	uint32_t epsg_code;
	double epoch;
	int32_t transformation_status;
};

struct PSRDATUMINFO
{
	uint8_t name[32];
	uint32_t epsg_code;
	double epoch;
	int32_t transformation_status;
};

struct BESTGNSSDATUMINFO
{
	uint8_t name[32];
	uint32_t epsg_code;
	double epoch;
	int32_t transformation_status;
};

struct PDPDATUMINFO
{
	uint8_t name[32];
	uint32_t epsg_code;
	double epoch;
	int32_t transformation_status;
};

struct RTKDATUMINFO
{
	uint8_t name[32];
	uint32_t epsg_code;
	double epoch;
	int32_t transformation_status;
};

struct BESTDATUMINFO
{
	uint8_t name[32];
	uint32_t epsg_code;
	double epoch;
	int32_t transformation_status;
};

struct TILTCOMPENSATIONCONTROL
{
	int32_t enable;
};

struct WIFIAPPASSKEY
{
	uint8_t passkey[65];
};

struct WIFIAPCHANNEL
{
	int32_t channel;
};

struct WIFIAPSETTINGS
{
	uint8_t ssid[33];
	uint8_t passkey[65];
	int32_t band;
	int32_t security_type;
	int32_t encryption;
	int32_t region;
	int32_t channel;
	uint8_t bssid[18];
};

struct WIFIAPIPCONFIG
{
	uint8_t ip_address[16];
	uint8_t netmask[16];
};

struct MEDIAFORMAT
{
	int32_t mass_storage_device;
};

struct WIFIMODE
{
	int32_t mode;
};

struct SATELCONTROL
{
	int32_t action;
	uint8_t value[24];
};

struct SATELDETECT
{
	int32_t comm_port;
};

struct SATELSTATUS
{
	int32_t satel_status;
	int32_t satel_error;
	uint8_t failed_command[48];
};

struct WIFIAPSSID
{
	uint8_t ssid[33];
};

struct WIFISTATUS_clients
{
	uint8_t mac_address[18];
};

struct WIFISTATUS
{
	int32_t status;
	uint8_t ssid[33];
	int32_t rssi;
	uint32_t channel;
	uint8_t bssid[18];
	int32_t security;
	uint32_t clients_arraylength;
	WIFISTATUS_clients clients[4];
};

struct WIFINETLIST_net_ap_info
{
	uint8_t ssid[33];
	int32_t rssi;
	uint32_t channel;
	uint8_t bssid[18];
	int32_t security;
};

struct WIFINETLIST
{
	uint32_t net_ap_info_arraylength;
	WIFINETLIST_net_ap_info net_ap_info[20];
};

struct WIFINETCONFIG
{
	uint32_t network_id;
	int32_t sw;
	uint8_t ssid[33];
	uint8_t passkey[65];
	int32_t address_mode;
	uint8_t ip_address[16];
	uint8_t net_mask[16];
	uint8_t gateway[16];
	uint8_t dns_server[16];
};

struct WIFIALIGNAUTOMATION
{
	int32_t sw;
	uint32_t network_id;
	int32_t icom;
	uint32_t req_rate_hz;
	int32_t send_heading_ext;
	int32_t interface_mode;
};

struct SATEL4CONFIG
{
	int32_t satel4_radio_behaviour;
	uint32_t tx_power_m_w;
	uint32_t freq_hz;
	uint32_t compatibility_mode;
	int32_t satel4_base_type;
};

struct SATEL4INFO
{
	int32_t satel_protocol;
	uint32_t tx_freq_hz;
	uint32_t rx_freq_hz;
	uint32_t channel_spacing_hz;
	uint32_t tx_power_m_w;
	bool fec_enabled;
};

struct SATEL9CONFIG
{
	int32_t satel9_modem_mode;
	uint32_t freq_key;
	uint32_t network_id;
	uint32_t min_packet_size;
	uint32_t max_packet_size;
	uint32_t retry_timeout;
	uint32_t subnet;
	bool repeaters;
	uint32_t master_packet_repeat;
	uint32_t tx_power_m_w;
};

struct SATEL9CONFIGL
{
	int32_t satel9_modem_mode;
	uint32_t leica_channel;
	uint32_t retry_timeout;
	uint32_t subnet;
	bool repeaters;
	uint32_t master_packet_repeat;
	uint32_t tx_power_m_w;
};

struct SATEL9CONFIGN
{
	int32_t satel9_modem_mode;
	uint32_t novariant_channel;
	uint32_t retry_timeout;
	uint32_t subnet;
	bool repeaters;
	uint32_t master_packet_repeat;
	uint32_t tx_power_m_w;
};

struct SATEL9INFO
{
	int32_t satel9_modem_mode;
	int32_t leica_channel;
	int32_t novariant_channel;
	uint32_t freq_key;
	uint32_t network_id;
	uint32_t min_packet_size;
	uint32_t max_packet_size;
	uint32_t retry_timeout;
	uint32_t subnet;
	bool repeaters;
	uint32_t master_packet_repeat;
	uint32_t tx_power_m_w;
	uint32_t hop_table_version;
	uint8_t freq_zone[16];
};

struct SATELSTARTUPDETECT
{
	int32_t enable;
	int32_t comm_port;
};

struct BLUETOOTHCONTROL
{
	int32_t bluetooth_enable;
};

struct PASSETH1
{
	uint32_t buffer_arraylength;
	char buffer[80];
};

struct SBAS12
{
	uint32_t prn;
	double a1;
	double a0;
	uint32_t t0t;
	uint16_t wn;
	int16_t dt_ls;
	uint16_t wnlsf;
	uint16_t dn;
	int16_t dt_lsf;
	uint16_t utcid;
	uint32_t gpstow;
	uint32_t gpswn;
	bool glonass_indicator;
	uint8_t SBAS12_reserved_bits[10];
};

struct SBAS27_regions
{
	int32_t lat1;
	int32_t long1;
	int32_t lat2;
	int32_t long2;
	uint32_t shape;
};

struct SBAS27
{
	uint32_t prn;
	uint32_t iods;
	uint32_t num_service_msgs;
	uint32_t service_msg_num;
	uint32_t priority_code;
	uint32_t udre_inside;
	uint32_t udre_outside;
	uint32_t regions_arraylength;
	SBAS27_regions regions[5];
	uint32_t SBAS27_reserved;
};

struct SBAS6
{
	uint32_t prn;
	uint32_t iodf2;
	uint32_t iodf3;
	uint32_t iodf4;
	uint32_t iodf5;
	uint32_t udrei[51];
};

struct INSPVACMP
{
	uint32_t week_milliseconds;
	uint8_t ins_solution_status;
	uint8_t gnss_position_type;
	int64_t latitude;
	int64_t longitude;
	int32_t height;
	int16_t north_vel;
	int16_t east_vel;
	int16_t up_vel;
	int16_t roll;
	int16_t pitch;
	uint16_t azimuth;
	int16_t azimuth_rate;
	uint32_t milliseconds;
};

struct INSCOV
{
	uint32_t week;
	double seconds;
	double elements[9];
	double elements_3[9];
	double elements_4[9];
};

struct INSCOVS
{
	uint32_t week;
	double seconds;
	double elements[9];
	double elements_3[9];
	double elements_4[9];
};

struct SETMARK3OFFSET
{
	double input[3];
	double input_1[3];
};

struct SETMARK4OFFSET
{
	double input[3];
	double input_1[3];
};

struct SETIMUTOANTOFFSET
{
	double input[3];
	double input_stdev[3];
};

struct SETIMUORIENTATION
{
	int32_t orientation;
};

struct RVBCALIBRATE
{
	int32_t cmd;
};

struct VEHICLEBODYROTATION
{
	double input[3];
	double input_stdev[3];
};

struct INSWHEELUPDATE
{
	int32_t trigger;
};

struct BESTLEVERARM
{
	double input[3];
	double input_stdev[3];
	int32_t mapping;
};

struct LEVERARMCALIBRATE
{
	int32_t trigger;
	double max_time;
	double max_offset_std;
};

struct SETINSOFFSET
{
	double input[3];
};

struct INSUPDATE
{
	int32_t pos_type;
	int32_t num_psr;
	int32_t num_phase;
	int32_t num_dop;
	bool zupt_epoch;
	int32_t wheel_status;
	int32_t heading_update_status;
};

struct SETINITATTITUDE
{
	double pitch;
	double roll;
	double azimuth;
	float pitch_std_dev;
	float roll_std_dev;
	float azimuth_std_dev;
};

struct APPLYVEHICLEBODYROTATION
{
	int32_t trigger;
};

struct EXTHDGOFFSET
{
	double heading_offset;
	double heading_offset_std_dev;
	double pitch_offset;
	double pitch_offset_std_dev;
};

struct SETIMUTOANTOFFSET2
{
	double input[3];
	double input_stdev[3];
};

struct BESTLEVERARM2
{
	double input[3];
	double input_stdev[3];
	int32_t mapping;
};

struct IMUTOANTOFFSETS_lever_arm
{
	int32_t offset;
	float elements[3];
	float elements_4[3];
	int32_t status;
};

struct IMUTOANTOFFSETS
{
	int32_t mapping;
	uint32_t lever_arm_arraylength;
	IMUTOANTOFFSETS_lever_arm lever_arm[3];
};

struct INSZUPTCONTROL
{
	int32_t trigger;
};

struct SETGIMBALORIENTATION
{
	int32_t orientation;
};

struct GIMBALSPANROTATION
{
	double input[3];
	double input_stdev[3];
};

struct SETIMUTOGIMBALOFFSET
{
	double input[3];
	double input_stdev[3];
};

struct SETMARK1OFFSET
{
	double input[3];
	double input_1[3];
};

struct SETMARK2OFFSET
{
	double input[3];
	double input_1[3];
};

struct SETWHEELSOURCE
{
	int32_t wheel_sensor_input;
	int32_t polarity;
	uint32_t SETWHEELSOURCE_reserved;
};

struct RAWLBANDFRAME
{
	uint16_t service_id;
	uint16_t RAWLBANDFRAME_reserved;
	uint32_t data_arraylength;
	uint8_t data[512];
};

struct RAWLBANDPACKET
{
	uint32_t data_arraylength;
	uint8_t data[512];
};

struct PDPVELOCITYOUT
{
	int32_t pdp_velocity_out_cmd;
};

struct DGPSEPHEMDELAY
{
	uint32_t ephem_delay;
};

struct GPGGARTK
{
	double latitude;
	double longitude;
	float undulation;
	double height;
	int32_t solution_status;
	int32_t position_type;
	uint8_t num_soln_svs;
	float hdop;
	float diff_age;
	uint8_t base_id[4];
	uint32_t gga_quality;
};

struct LBANDINFO
{
	uint32_t assigned_frequency;
	uint32_t baud_rate;
	uint16_t service_id;
	uint16_t ushort;
	uint32_t osn;
	int32_t subscription_type;
	uint32_t expiration_week;
	uint32_t expiration_seconds;
	int32_t subscription_type_8;
	uint32_t expiration_week_9;
	uint32_t expiration_seconds_10;
	int32_t subscription_mode;
};

struct BASEANTENNAMODEL
{
	uint8_t antenna_name[32];
	uint8_t serial_number[32];
	uint32_t setup_id;
	int32_t antenna_type;
	double l1_offset[3];
	double l1_pcv[19];
	double l2_offset[3];
	double l2_pcv[19];
};

struct SBAS32
{
	uint32_t prn;
	uint32_t iodp;
	int32_t prc[11];
	uint32_t udrei[11];
};

struct SBAS33
{
	uint32_t prn;
	uint32_t iodp;
	int32_t prc[11];
	uint32_t udrei[11];
};

struct SBAS34
{
	uint32_t prn;
	uint32_t iodp;
	int32_t prc[11];
	uint32_t udrei[11];
};

struct SBAS35
{
	uint32_t prn;
	uint32_t iodp;
	int32_t prc[11];
	uint32_t udrei[11];
};

struct SBAS45
{
	uint32_t prn;
	uint32_t prn_mask_number;
	uint32_t iode;
	int32_t dx;
	int32_t dy;
	int32_t dz;
	int32_t ddx;
	int32_t ddy;
	int32_t ddz;
	int32_t daf0;
	uint32_t tod;
	uint32_t prn_mask_number_11;
	uint32_t iode_12;
	int32_t dx_13;
	int32_t dy_14;
	int32_t dz_15;
	int32_t ddx_16;
	int32_t ddy_17;
	int32_t ddz_18;
	int32_t daf0_19;
	uint32_t tod_20;
	uint32_t iodp;
};

struct SETCANNAME
{
	uint32_t manufacturer_code;
	uint32_t industry_group;
	uint32_t device_class;
	uint32_t device_class_instance;
	uint32_t function;
	uint32_t function_instance;
	uint32_t ecu_instance;
	uint32_t preferred_address;
	int32_t can_port;
};


#pragma pack()
}

#endif //NOVATEL_MESSAGE_DEFINITIONS_HPP
