// csp_example.c
// Simeon Shaffar 4/25/2025
// This is meant to be a self contained example of a barebones implementation
// of libcsp and libparam. It shows one example of getting a parameter and
// one example of setting a parameter on the P3 PCDU

#include <unistd.h>
#include <pthread.h>

#include <csp/csp.h>
#include <csp/csp_yaml.h>
#include <param/param_client.h>

#define CSP_VERSION 2
#define VERBOSE 0
#define TIMEOUT 10
#define ACK_WITH_PULL 1
#define INDEX_ALL -1
#define ARRAY_SIZE 12

uint16_t PCDU = 3;

int16_t _pcdu_temp = 0;
PARAM_DEFINE_REMOTE(30, pcdu_temp, &PCDU, PARAM_TYPE_INT16, 1, sizeof(int16_t), PM_TELEM, &_pcdu_temp, "This is the temperature of the PCDU")
uint8_t _pcdu_dfl_on[12] = {0};
PARAM_DEFINE_REMOTE(230, pcdu_dfl_on, &PCDU, PARAM_TYPE_UINT8, 12, sizeof(uint8_t), PM_CONF, &_pcdu_dfl_on, "")


// Define some symbols for libparam just to get the program to compile
void * router_task(void * param) {
    (void)param;
  while(1) {
    csp_route_work();
  }
}

void init_csp_barebones() {
  // Generalized csp init function from libcsp
  csp_init();
  printf("initialized csp\n");

  
  // Use the yaml file to initialize the CAN connection as CSP's interface
  char * yamlname = "/etc/SMALLSAT/eps/bridge.yaml";
	unsigned int dfl_addr = 0;
	csp_yaml_init(yamlname, &dfl_addr);
  printf("Initialized the yaml\n");


  // Set up a pthread for the router
  // CSP basically always requires that you initialize a second thread to do routing
  // They require that a user does it manually in their code and not in csp_init() for some reason
  static pthread_t router_handle;
	pthread_create(&router_handle, NULL, &router_task, NULL);

  printf("Started the pthread for the router, csp initialized\n");
}

int main() {
  init_csp_barebones();

  uint8_t ch11_dfl_on = 0;

  for(int i = 0; i < 12; i++) {
    _pcdu_dfl_on[i] = 0;
  }

  //Spams all telemetry
  while(1) {
    if (param_pull_single(&pcdu_temp, INDEX_ALL, CSP_PRIO_NORM, VERBOSE, *pcdu_temp.node, TIMEOUT, CSP_VERSION) < 0)
      printf("\tparam_pull_single failed on pcdu_temp\n");
    

    // Print stuff for debug
    // int nodes = 3;
    // param_print(&pcdu_dfl_on, INDEX_ALL, &nodes, 1, 10, *pcdu_dfl_on.timestamp);

    csp_timestamp_t zeroTimestamp = {0, 0};
    *pcdu_dfl_on.timestamp = zeroTimestamp;
    param_set(&pcdu_dfl_on, 11, &ch11_dfl_on);
    if (param_push_single(&pcdu_dfl_on, 11, CSP_PRIO_HIGH, NULL, VERBOSE, *pcdu_dfl_on.node, TIMEOUT, CSP_VERSION, ACK_WITH_PULL) < 0)
      printf("\tparam_push_single failed for pcdu_dfl_on\n");
    
    if (param_pull_single(&pcdu_dfl_on, INDEX_ALL, CSP_PRIO_LOW, VERBOSE, *pcdu_dfl_on.node, TIMEOUT, CSP_VERSION) < 0)
      printf("\tparam_pull_single failed on pcdu_dfl_on\n");

    printf("pcdu_temp: %d\n", _pcdu_temp);
    printf("pcdu_dfl_on[11]: %d\n", param_get_uint8_array(&pcdu_dfl_on, 11));


    ch11_dfl_on = !ch11_dfl_on;

    sleep(1);
  }

  return 0;
}

