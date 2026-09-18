# Systemd Service guide

This is meant to show the basic interactions and functions of a service script

This guide will be using `gps.service` as an example. `sudo` is requred to modify service files if run as a standard user.

Service files can be found in `/etc/systemd/system/` and use the file extension `.service`

## A new service

When a service has been copied into `/etc/systemd/system` or it has been edited the daemon must be reloaded for the changes to take effect. To reload the daemon run

```bash
sudo systemctl daemon-reload
```

## Running a service

To start `gps.service` run:

```bash
sudo systemctl start gps.service
```

This will start the service and it will continue to run for the duration of that boot instance

## Enabling a service

When the user needs the service to run on every boot, the service has to be enabled. This can be done by running

```bash
sudo systemctl enable gps.service
```

Note that this does not start the service, it only enables the service to be run on boot and all subsequent boots

## Service Status

To check the status of a service run

```bash
sudo systemctl status gps.service
```

This command will output the current status of the service as well as a few of the most recent logs from the service.
To see more output from a service its output can be viewed with

```bash
sudo journalctl -u gps.service 
```

This will start at the first log so use `G` to get to the end of the file.
Alternatively to watch a certian service use the follow flag `-f` to observe the live output

## Stopping a service

If a service needs to be stopped while it is running, run:

```bash
sudo systemctl stop gps.service
```

Note that if the service is enabled, it will still run on next boot.

## Disabling a service

In order to fully stop a service run

```bash
sudo systemctl disable gps.service
```

This *should* stop the service indefinately. There is a caveat though, if the stopped service is required by another service it will be restarted and reenabled by that service. If a service is persisting even after you disable it, it's likely a depended on by another service

## REFERENCE

For further reading check Debians guide on services.

[Debian Service Guide](https://wiki.debian.org/systemd/Services)
