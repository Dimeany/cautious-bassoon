# applyheader(1)
#### NAME
applyheader - applies the SSRL header.
#### SYNOPSIS
applyheader [**OPTION**]...
#### DESCRIPTION
Takes bytes from `STDIN` (via a pipe) and applies to SSRL header to the front. Bytes are then printed to `STDOUT`.
#### OPTIONS
**-p**  source_port\
Header field source port.

**-d** destination_port\
Header field destination port.

**-t** packet_type\
Header field packet type.

**-q**\
Header field sequence number.

**-n**\
Puts the header into network byte order.

**-h**\
Help information is written to `STDERR`.
#### EXIT STATUS
On success process exits with 0. On failure process exits with 1 and information about that error is printed to `STDERR`. On timeout of read (2 seconds) process exits with 2.
#### FILES
source file under `Nebula/src/udp/applyheader.cc`\
`PacketType` enum file under `Nebula/src/udp/udp.h`
#### EXAMPLES
Sending a string with a SSRL packet header.
```bash
echo -n "radio_on" | applyheader -d 50000 -t 0 | sendwait -p 50000  -a "127.0.0.1" 

```
#### SEE ALSO
Help with pipes: [tutorial](https://www.geeksforgeeks.org/piping-in-unix-or-linux/); [man page](https://man7.org/linux/man-pages/man2/pipe.2.html)

Help with Linux files: [article](https://www.howtogeek.com/435903/what-are-stdin-stdout-and-stderr-on-linux/)

[sendwait](sendwait.md)
