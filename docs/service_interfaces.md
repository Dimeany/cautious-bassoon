# Port Guide

## Subsystem Port Numbering

### GPS:  `550XX`

### DET:  `540XX`
- The "DET health packet header attacher" runs on port 61666.
- It forwards to CDH at 51000 and COMMS at 50001.

### ADCS:  `530XX`

### EPS:  `520XX`

### CDH:  `510XX`

### COMM:  `500XX`

## Ports given to each subsystem

`00`: The port corresponding to the actual subsystem service

`01`: The port used for the primary udp capture service

`02`: The port used for the decoders. Packets sent to the udp capture service forward here

`03`: Miscellaneous, ADCS uses it for the commissioning packet
