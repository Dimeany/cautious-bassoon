// csp_driver.h
// Created by Simeon Shaffar on 11/17/2023
// This h file declares everything that csp_driver.c uses

#ifndef CSP_DRIVER_H
#define CSP_DRIVER_H

#include <unistd.h>
#include <pthread.h>
#include <sys/utsname.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <csp/csp_yaml.h>
#include <csp/csp_cmp.h>

#include <param/param_server.h>
#include <param/param_client.h>
#include <param/param_list.h>
#include <param/param_string.h>

#include <vmem/vmem_server.h>

#include "csp_parameters.h"
#include "eps_health.h"
#include "eps_settings.h"
#include "eps_info.h"

#ifdef __cplusplus
extern "C" {
#endif


// These are constants defined at compile time using a macro
// As the compiler preprocesses these fields, it replaces it with the literal number defined with each name
#define CSP_VERSION 2
#define VERBOSE 0
#define INDEX_ALL -1
#define TIMEOUT 10
#define ACK_WITH_PULL 1
#define PARAM_PORT_SERVER 10
#define VMEM_PORT_SERVER 14


// Main Driver Functions -------------------------------------------

// Pull Functions
const char* pull_parameter_string(const char* name, bool mission, int offset);
int pull_parameter(param_t* param, void* bucket);
param_t* find_parameter(const char* name, bool mission);
const char* param_value_string(param_t * param, int offset);

// Push Functions
int push_parameter_string(const char* name, bool mission, const char* offset_str, const char* new_value_str);
int push_parameter(param_t* param, void* new_value, int offset);

// Health Packet Functions
int build_impress_health(eps_pdup3_health_t* hp);
int build_impress_health_queue(eps_pdup3_health_t* hp);

int build_exact_health(eps_pdup4_health_t* hp);
int build_exact_health_queue(eps_pdup4_health_t* hp);

int pdup4_get_settings(eps_pdup4_settings_t* settings);
int pdup4_get_info(eps_pdup4_info_t* info);
int try_pull_too_many_params();

// Internal CSP Functions --------------------------------------------

// This is our in house funtion that starts up csp, which wraps csp_init() to also start up the CAN and router
void start_csp();
void start_csp_extended();

// These functions define the loops that the secondary threads would use
void * router_task(void *);
// Used later in initCSP(), helps create pthreads (whatever that is)
void * vmem_server_task(void *);


// These are some other functions that are useful during development time, 
// especially when working with new devices
int csp_scan();
int list_download(unsigned int node, unsigned int timeout);
int cmd_pull(unsigned int node, unsigned int timeout);
void print_param_fields();

// -------------------------------------------------------------------


#ifdef __cplusplus
}
#endif

#endif