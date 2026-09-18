// csp_functions.c
// Simeon Shaffar - May 1, 2025
// This file defines some arbitrary functions that CSP uses, some for debugging/development only

#include "csp_driver.h"


// This function is the master function that sets up Cubesat Space Protocol (CSP)
// It effectively runs every line of executable code that is needed for CSP to work.
// The statically defined stuff to make CSP work is above, and this contains everything
// that needs to get run to initialize it
void start_csp() {
	// This is CSP's main internal init function, it does most things required for setup but needs
	// us to do the initialization of the CAN interface and create the router thread below
	csp_init();


	// This is the address of the yaml file in the flight environment
	char * yamlname = "/etc/SMALLSAT/eps/can.yaml";
	// This is the default CSP address of the flight computer, Space Inventor recommends 0
	unsigned int dfl_addr = 0;
	// This tells CSP to read the yaml file, initializing CAN as one of its connections 
	csp_yaml_init(yamlname, &dfl_addr);


	// CSP basically always requires that you initialize a second thread to do routing of packets
	// They require that a user does it manually in their code and not in csp_init() for some reason
	static pthread_t router_handle;
	pthread_create(&router_handle, NULL, &router_task, NULL);


	printf("\nCSP Initialized!\n");
} //end of start_csp()


// This is the remnants of the original initializer that was used when Kurtis first got the driver working
// He did a lot of back and forth with Kevin Carlsen @ Space Inventor and some of this code was directly
// from Kevin's emails. There is a lot of redundancy in it so the official initializer is cut down, but
// I wanted to preserve this original version for future reference.
void start_csp_extended() {
	char * hostname = NULL;
    char * model = NULL;
    char * revision = NULL;
	int version = 2;
	int dedup = 0;	// Was originally 3, but this caused some dropped packets when spamming 1 parameter


	static struct utsname info;
	uname(&info);

    if (hostname){
        strncpy(info.nodename, hostname, _UTSNAME_NODENAME_LENGTH - 1);
    }
    if (model){
        strncpy(info.version, model, _UTSNAME_VERSION_LENGTH - 1);
    }
    if (revision){
        strncpy(info.release, revision, _UTSNAME_RELEASE_LENGTH - 1);
    }

    hostname = info.nodename;
    model = info.version;
    revision = info.release;

    printf("  Version %d\n", version);
    printf("  Hostname: %s\n", hostname);
    printf("  Model: %s\n", model);
    printf("  Revision: %s\n", revision);
    printf("  Deduplication: %d\n", dedup);

    csp_conf.hostname = hostname;
	csp_conf.model = model;
	csp_conf.revision = revision;
	csp_conf.version = version;
	csp_conf.dedup = dedup;
	csp_init();

    csp_bind_callback(csp_service_handler, CSP_ANY);
	csp_bind_callback(param_serve, PARAM_PORT_SERVER);

	static pthread_t router_handle;
	pthread_create(&router_handle, NULL, &router_task, NULL);

	static pthread_t vmem_server_handle;
	pthread_create(&vmem_server_handle, NULL, &vmem_server_task, NULL);

    csp_iflist_check_dfl();

	csp_rdp_set_opt(3, 10000, 5000, 1, 2000, 2);
	// csp_rdp_set_opt(5, 10000, 5000, 1, 2000, 4);
	// csp_rdp_set_opt(10, 10000, 5000, 1, 2000, 8);
	// csp_rdp_set_opt(25, 10000, 5000, 1, 2000, 20);
	// csp_rdp_set_opt(40, 3000, 1000, 1, 250, 35);


	// This is the address of the yaml file in the flight environment
	char * yamlname = "/etc/SMALLSAT/eps/can.yaml";
	// This is the default CSP address of the flight computer, Space Inventor recommends 0
	unsigned int dfl_addr = 0;
	// This tells CSP to read the yaml file, initializing CAN as one of its connections 
	csp_yaml_init(yamlname, &dfl_addr);

	printf("CSP Initialized Using Extended Settings!\n");
} //start_csp_extended


// This defines how the router thread will work. The router thread is a separate thread that will 
// run in parallel to the main service thread. It essentially just plucks a packet off of a queue
// and sends it to wherever its destination information indicates it should be sent
//
// This implementation is based directly off of how Space Inventor recommends we configure it, 
// the function csp_route_work() just plucks a single packet off the queue and sends it
void * router_task(void * param) {
    (void)param;
    while(1) {
      csp_route_work();
  }
}

// Used later in initCSP(), helps create pthreads (whatever that is)
void * vmem_server_task(void * param) {
	vmem_server_loop(param);
	return NULL;
}


// DEVELOPMENT ONLY
// csp_scan() is a debug function that can be used whilst troubleshooting connections
// It tells you all the devices CSP has on the CAN bus
int csp_scan() {
    unsigned int begin = 0;
    unsigned int end = 100;
	char * search_str = 0;

	bool search = (search_str && (strlen(search_str) > 0));

	printf("CSP SCAN  [%u:%u]\n", begin, end);
	if (search) {
		printf("Searching for host name sub-string '%s'\n", search_str);
	}

    for (unsigned int i = begin; i <= end; i++) {
        printf("trying %u: \r", i);
        if (csp_ping(i, 20, 0, CSP_O_CRC32) >= 0) {
			printf("Found something on addr %u...\n", i);
				
			struct csp_cmp_message message;
			if (csp_cmp_ident(i, 100, &message) == CSP_ERR_NONE) {
				if ((!search) || (strstr(message.ident.hostname, search_str) != NULL)) {
					printf("%s\n%s\n%s\n%s %s\n\n", message.ident.hostname, message.ident.model, message.ident.revision, message.ident.date, message.ident.time);
				}
			}
        } else {
			printf("Nothing found on addr %u\n", i);
		}

    }
	printf("\r");

	return 0;
}

// DEVELOPMENT ONLY
// list_download() asks a given device on the CAN bus what all its parameters are.
// Note that this is not the same thing as asking for the VALUES of the parameters,
// It only asks what the names are for each parameter, this allows later functions to pull values
int list_download(unsigned int node, unsigned int timeout){
    //change this so it does not print to the screen
    unsigned int version = 2;
    int include_remotes = 0;
    param_list_download(node, timeout, version, include_remotes);
    return 0;
} //list_download()

// DEVELOPMENT ONLY
// Asks for the values of every parameter for a given device to be updated,
// Note that this only matters for one specific way of defining parameters.
// If you use param_push_single() or param_pull_single() as we currently do in the driver,
// cmd_pull() is completely unnecessary
int cmd_pull(unsigned int node, unsigned int timeout) {
	//printing to screen bad, slows you a ridiculous amount
	char * include_mask_str = NULL;
	char * exclude_mask_str = NULL;
	uint32_t include_mask = 0xFFFFFFFF;
	uint32_t exclude_mask = PM_REMOTE | PM_HWREG;

	if (include_mask_str)
		include_mask = param_maskstr_to_mask(include_mask_str);
	if (exclude_mask_str)
	    exclude_mask = param_maskstr_to_mask(exclude_mask_str);
	if (param_pull_all(CSP_PRIO_NORM, VERBOSE, node, include_mask, exclude_mask, timeout, CSP_VERSION)) {
		printf("No response\n");
		return -4;
	}
	return 0;
} //cmd_pull()
