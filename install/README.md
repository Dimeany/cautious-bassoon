# Install

## Setup

The Pis can be set up with the following command:
```sh
sudo make install
```
This will copy these files to the proper places in the system.

## What is Installed

It contains a `config.txt` that gets installed to `/boot/firmware/config.txt`
**If the OS is not yet bookworm, make sure to manually install to /boot/config.tx instead**

It also contains our systemd unit files for the services. These will get installed to `/etc/systemd/system/services`.

## systemd

With these in place, the services can be started, restarted, or stopped, with the respective following commands.
```sh
systemctl start <service>
systemctl restart <service>
systemctl stop <service>
```

Or, automatic starting at boot can be enabled or disabled with
```sh
systemctl enable <service>
systemctl disable <service>
```
