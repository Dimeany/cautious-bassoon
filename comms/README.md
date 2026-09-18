# COMMS Service

## Setup
To build, ensure `-DSMALLSAT_SUBSYSTEMS` has `COMMS`

## Binaries
[auto generated text on 11/18/24 from running comms_service -h]: #
```
comms_service: multiplex and demultiplex packets with a serial, ethernet, and file backend
    -h, --help: print this message
    -v, --verbose: more verbose output. Can be specified multiple times.
    -p, --ppp: enable ppp framing
    -f, --force: disable file checking
    -u, --usec-wait: microseconds to wait after sending a packet
    -l, --live-address <ip:port>: set live port address (default: 0.0.0.0:50000)
    -n, --norm-address <ip:port>: set norm port address (default: 0.0.0.0:56000)
    -N, --norm-receiver <port>: set norm receiver port (default: 57000)
    -s, --serial <interface>: specify serial interface as transfer mode
    -e, --ethernet-source <ip:port>: specify ip address and port number to send from as transfer mode
    -d, --ethernet-dest <ip:port>: specify ip address and port number to send to as transfer mode
    -i, --file-in <file>: specify filename to read from as transfer mode
    -o, --file-out <file>: specify filename to write to as transfer mode
    -D, --dump <file>: specify filename to read without sending out packets
```
## TODO
- Add serial link processing
- Improve UDP ethernet link backend
- Add TCP ethernet link backend
- Improve unthrottled speed
- Add speed throttling option
