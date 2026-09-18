# SSH Guide

## Access to UMN Network
The pis are located within the university network and are only accessible from within it.
There are a few methods to get into the network from afar.

### VPN
This is the most universal method. Anyone with a university login should be able to set this up.

Information on the **Cisco VPN client** necessary for this step is available at [this article](https://it.umn.edu/services-technologies/virtual-private-network-vpn) with installation instructions.

Or, for a quicker install,

- **Windows:** Download at [this link](https://z.umn.edu/AnyConnect_Windows)
- **Mac:** Download at [this link](https://z.umn.edu/AnyConnect_macOS)

Once installed, log in using [these instructions](https://tdx.umn.edu/TDClient/31/Portal/KB/ArticleDet?ID=3991).
Select the UMN - Split Tunnel, connect, and log in with your x500.

### CSE Labs
If you have a CSE Labs Account, you can ssh directly into the lab from wherever. This provides the general entry into the UMN network, rather than the VPN.
If you do not have an account yet and are in CSE, create one [here](https://wwws.cs.umn.edu/account-management/).
Then, you should be able ssh directly into any of the computers [here](https://cse.umn.edu/cseit/classrooms-labs), and from within them ssh into the lab.

This can be made easier with ssh keys and a good config file, which will be discussed further in this. The most useful ssh option for this is `ProxyJump`, shown further below.

If ssh keys are loaded onto the CSE Labs machine, duo authentication is unnecessary. This is also discussed below.

## SSH Config File
Edit/create the file at:

- **Windows:** C:\Users\\\<username>\\.ssh\config
- **Mac/Linux:** ~/.ssh/config

Note that this filename has no extensions (it isn't config.txt)

In this file, we want to fill out hosts in the following generic form.

```sshconfig
Host <host>
    HostName <IP address>
    User <username>
    ProxyJump <other host (optional)>
```

Using ProxyJump makes `ssh <host>` behave as though `ssh <username>@<IP address>` was called from `<other host>` computer.

Additional options are available to be set if desired. A good guide on this is available [here](https://linuxize.com/post/using-the-ssh-config-file/).

## Pi Addresses
These can change occassionally. The only hosts publicly available are `MPLS` and `STPAUL`, and `det`. All others must first go through those.
The detector development Pi is still in Tate, so it doesn't require a proxy jump.

An example configuration is shown below,
	with the Pi IP addresses appearing in the `HostName` field.

```sshconfig
Host MPLS
	HostName 10.131.126.15
	User gnd

Host FLIGHTIMP
    HostName 192.168.2.20
    user flightimp
    ProxyJump MPLS

Host FLIGHT
	HostName 192.168.2.71
	User flight
	ProxyJump MPLS

Host BUILD
	HostName 192.168.2.85
	User build
	ProxyJump MPLS

Host GPS
	HostName 192.168.2.65
	User GPS
	ProxyJump MPLS

Host STPAUL
	HostName 10.131.82.127
	User gnd

Host EXACTCOMMS
	HostName 192.168.3.158
	User exactcomms
	ProxyJump STPAUL

Host BUILDEXA
	HostName 192.168.3.10
	User buildexa
	ProxyJump STPAUL

Host FLIGHTEXA
    HostName 192.168.3.20
    User flighexa
    ProxyJump STPAUL

# This Pi has its own WiFi dongle, so we don't need a proxy jump
host det
	hostname 10.136.95.201
	user det

```

With this config file, if you are connected to the VPN, you can ssh into any of the pis with `ssh <name>` etc. If you are using a CSE labs machine, you must ProxyJump from that machine to MPLS and STPAUL.

## SSH Keys
You can set up an SSH key so you don't have to set up a password to log in. However, due to the nature of our constantly changing system it may be annoying to constantly maintain ssh keys on all machines.

### Generating a Key
Run the following command in a terminal.

```bash
ssh-keygen
```

Follow the prompts as asked, and **do not** overwrite the key if it exists and you are already using ssh keys elsewhere.
If you are indeed setting up a new key, just spam enter and yes. It is just asking you for where you want to put the file, and default is best.

If you use a different key name/location than default, this can be specified for use within the config file using the `IdentityFile` option as seen below.

```sshconfig
Host <host>
    HostName <IP address>
    User <username>
    IdentityFile <path/to/file>
```

### Distributing a Key
Now the key needs to be distributed to the computers you want to log in with.

#### Windows
Run the following command in **Powershell** (this is important). Include the word type.

```powershell
type $env:USERPROFILE\.ssh\id_rsa.pub | ssh <host> "cat >> .ssh/authorized_keys"
```

#### Mac/Linux
Run the following command

```bash
ssh-copy-id <host>
```

Both of these commands act the same way, copying your ssh key over to the computer specified by `<host>` and saving it in the authorized keys. You can do this for each host set up in the ssh config file.

Now, if this was successful, you should be able to directly ssh into any of the pis without typing a password.
