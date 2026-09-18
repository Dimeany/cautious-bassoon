### software overview

#### services

The bulk of the Nebula repository is for our various services, which are compiled programs that are ran as systemd services. The main bulk of the source code for a service is under Nebula/src/service_name.

Each function of the satellite has it's own service, meaning we have services both for software (CDH, CMD) and hardware (ADCS, GPS, EPS). The COMMS service is special in the fact that interfaces with both software (as it's the first communication point for software) and hardware (because all of the communication has to be done over specific hardware).

Our services are meant to be running at all times, even when associated hardware may not be communicating properly. This is because we want to see health packets coming in at all times, even if they may be empty.

Each service opens a socket on which it listens to ascii commands, which are the main way of interfacing with a service. Hardware services use this to take in user input to modify hardware settings. These commands are defined directly in the service code, so if you want to add a new one you must edit the service's associated code.

#### scripts

Our scripts are written in bash, and are under Nebula/scripts. Scripts are the main way we interface with our services, essentially creating shortcuts for sending the ascii commands to a service's command socket. Some scripts are just tools to automate long and/or complex tasks, such as all of the deployment and flash scripts.

#### udpcapture

udpcapture acts as a middleman, capturing incoming packets and saving them to file, before sending them to the intended destination. Each udpcapture instance is opened in a specific subdirectory in /SAT/LIVE/, and creates a file which it writes incoming packets to. Every hour this file is closed, moved to the respective /SAT/HOT/ subdirectory, and a new file is opened to start writing to.

### overall system function

All services which interface with some specific hardware generate health packets, containing telemetry pertaining to the hardware, at a fixed rate. These packets are sent to their respective udpcapture instance, and then sent to the CDH service. The CDH service caches these packets, and also takes specific actions based on telemetry if necessary. The CDH service is responsible for then sending these cached packets at the correct interval to the comms service, which finally sends them to ground/DICE.
