# sendwait(1)
#### NAME
sendwait - sends a UDP packet and waits for a response.
#### SYNOPSIS
sendwait **-p** port **-a** ipv4_address [**OPTION**]...
#### DESCRIPTION
Takes bytes from `STDIN` (via a pipe) or a message string (via **-m** option) and sends said bytes as a UDP packet to the address specified with the **-p** and **-a** options. The process will then block until it receives a response UDP packet. If a timeout (**-t** timeout_in_ms) is specified the process will only block for that amount of time. The received udp packets bytes are then written to `STDOUT`.
#### OPTIONS
**-p** port\
The port the UDP packet will be sent to.

**-a** ipv4_address_string\
The address the UDP packet will be sent to.

**-t** time_in_ms\
The timeout will be set on the socket file descriptor via `setsockopt()`. In principle, this will limit the block time on the `recvfrom()` call to whatever time is specified by this option.

**-s**\
The process does not wait for a response UDP packet. Exits early.

**-v**\
Verbose output is written to `STDERR`. This is mostly for debugging purposes. 

**-m** message_string\
Instead of passing bytes via a pipe, alternatively, this option allows the UDP packet to be constructed via a string. This is not recommended as only bytes that can be parsed by your terminal and `getopt()` are available.

**-h**\
Help information is written to `STDERR`.
#### EXIT STATUS
On success process exits with 0. On failure process exits with 1 and information about that error is printed to `STDERR`. On timeout (if **-t** timeout_in_ms is specified) process exits with 2.
#### FILES
source file under `Nebula/src/udp/sendwait.cc`
#### EXAMPLES
Sending a basic string to `localhost` on port 50000.
```bash
echo -n "hello localhost on port 50000" | sendwait -p 50000  -a "127.0.0.1"
sendwait -p 50000  -a "127.0.0.1" -m "hello localhost on port 50000"
```
Sending a string with a SSRL packet header.
```bash
echo -n "radio_on" | applyheader -d 50000 -t 0 | sendwait -p 50000  -a "127.0.0.1" 

```
#### SEE ALSO
Help with pipes: [tutorial](https://www.geeksforgeeks.org/piping-in-unix-or-linux/); [man page](https://man7.org/linux/man-pages/man2/pipe.2.html)

Help with Linux files: [article](https://www.howtogeek.com/435903/what-are-stdin-stdout-and-stderr-on-linux/)

Man pages referenced:
    [recvfrom](https://pubs.opengroup.org/onlinepubs/007904875/functions/recvfrom.html); [setsockopt](https://pubs.opengroup.org/onlinepubs/000095399/functions/setsockopt.html);
    [getopt](https://man7.org/linux/man-pages/man3/getopt.3.html);
    [applyheader](applyheader.md)
