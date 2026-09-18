# Usage documentation and handy commands

This document is meant to pool together the knowledge and usage methods everyone has.
Ideally this will help people learn more about how to better use the computers we're working on.

## nmap

[nmap manual](https://linux.die.net/man/1/nmap)

To find other computers on the network use nmap.
With this command `nmap` scans port 22 (ssh) in the local area 192.168.2.X and will report if the available computers have that port open or not

`nmap -p 22 192.168.2.1/24`

## tshark

[tshark manual](https://www.wireshark.org/docs/man-pages/tshark.html)

`tshark` is used to scan network interfaces.

## candump

`candump any`

## tcpdump

`tcpdump -i can0`

`tcpdump -i eth0`

## strace

`strace -p [PID] -f --tt`

## pgrep

`pgrep` is helpful for finding the process ids of a known process name

`pgrep -a adcs_service`
