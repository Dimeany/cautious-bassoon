# CSH Starter Guide

This is a starter guide for using CSH. CSH is the software we use which interacts with the EPS stack
and allows us to set things like voltage and current limits for the PDU hardware we have. As of the time of writing, IMPRESS uses a PDU-P3 and EXACT uses a PDU-P4 (From SpaceInventor).

***************************************************************************************************
DO NOT SET OR CHANGE VALUES IN CSH UNLESS YOU ARE CERTAIN IT IS SAFE TO DO SO. CONTACT A CE OR THE
EPS LEAD PRIOR TO MAKING ANY CHANGES.
***************************************************************************************************

1. Start CSP Shell.
    - The executable file is located in ~/csh/builddir
    - Run from the home directory using ```./csh/builddir/csh -i csh/init/can.csh```

2. Scan for can devices and enter the PDU
    - Run: ```csp scan``` and press q once you have scanned enough nodes.
    - Run: ```node x```, replacing x with the node you want. For IMPRESS, it's 3 and EXACT it's 5.

3. To see the current settings
    - Run: ```list download```
    - Run: ```pull```
    - You should see a long list of parameters and their values.

In order to see the available commands:
- Run: ```help```

You can get the usage info for commands as well:
- Run: ```help <command name>```
