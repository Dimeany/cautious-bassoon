# Nebula

Nebula contains utilities that will facilitate communication, scheduling, and services for cube satellites

## Setup

### Dependencies

Basic building blocks

```bash
sudo apt install git g++ cmake make build-essential pkg-config
```

Libraries

```bash
sudo apt install libgpiod-dev libgtest-dev libbsd-dev libyaml-dev libsocketcan-dev libboost-dev libusb-1.0-0-dev libsystemd-dev
```

### Getting the repo

In your working folder run:

```bash
git clone https://github.umn.edu/UMN-CubeSat-Team/Nebula.git
```

Alternatively, you can use ssh to avoid having to type your credentials, but it requires extra setup. This should all be done on the computer you are going to be developing on, in most cases on your user in buildexa/buildimp. These instructions require bash. If you are on Windows, you should install git bash.
First, generate an ssh key, or use an existing one. All default settings are fine, you can press enter on all prompts. To generate a new one, run:

```bash
ssh-keygen -t ed25519 -C "youremail@example.com"
```

Verify that everything has worked by running:

```bash
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_ed25519
ssh-add -l
```

As long as you see no errors, everything should be configured correctly.
Next, copy the generated public key file to your clipboard. You can read it by running:

```bash
cat ~/.ssh/id_ed25519.pub
```

Make sure you copy the full output. With the key file copied, go to your github account settings under 'SSH and GPG keys'. Click 'New SSH key', and paste in the whole file you copied. Add whatever title you want, that will help you remember what the key is from.
Make sure you are in whatever folder you want the repo cloned to. Finally, you can clone the repository, using: (It must be done like this because of the way our submodules are formatted)

```bash
git clone ssh://git@github.umn.edu/UMN-CubeSat-Team/Nebula.git
```

After cloning the repository, pull down the submodules.  This submodule command can also be
run to update your submodules if they change after a pull or a
checkout.

```bash
cd Nebula
git submodule update --init --recursive
```

### Building

#### Unix

Building for ubuntu/debian based systems

```bash
mkdir build && cd build
cmake .. && make -j 2
```

#### Windows

Currently untested but this will likely work with [git for windows](https://git-scm.com/downloads)

An alternative would be to use docker containers

#### Mac

[//]: # (The dependencies will likely need to be downloaded and installed or with homebrew)

Similar to the Unix workflow but there might be differences with the dependencies.

If you have experience with this installation process please update the document.

### Building One or a Few Subsystems

If you would like to build only one or a few subsystems during
development, you can use the CMake variable `SMALLSAT_SUBSYSTEMS`.  It
is a semicolon-separated list of subsystems.  Its default value is "",
which signifies all subsystems.  If it is set, only the subsystems
listed will be configured and built.

The full list of subsystems is ADCS, CDH, CMD, COMMS, DET, EPS, and GPS; see
the setting in CMakeLists.txt.

To build only the ADCS subsystem, define this variable when you run CMake:

```bash
cmake -DSMALLSAT_SUBSYSTEMS="ADCS" ..
```

The variable will persist across `cmake` and `make` runs until you set
the variable again.

To build the ADCS and GPS subsystems, specify both ADCS and GPS in a
semicolon-separated string:

```bash
cmake -DSMALLSAT_SUBSYSTEMS="ADCS;GPS" ..
```

And to build all subsystems, set the variable to the empty string:

```bash
cmake -DSMALLSAT_SUBSYSTEMS="" ..
```

### Stripping Binaries

To automatically strip the compiled binaries, run with the additional CMake option `-DSTRIP_BINARIES=ON`. This will cause the CMake linker flag `-s` to be invoked, causing all compiled binaries to be stripped at linking time. This CMake variable is not cached, meaning that unlike some other CMake options (not including -DSMALLSAT_SUBSYSTEMS), it will default to off (no stripping) if the `-DSTRIP_BINARIES` option is not passed, regardless of the previous configuration. A message including the status of the `-DSTRIP_BINARIES` option is printed during the CMake configuration. One could of course manually set stripping to not occur by passing the option `-DSTRIP_BINARIES=OFF`, but this is unnecessary. One item of note is that CMake will rebuild all applicable binaries if this option is changed, unlike the previous behavior of only changed binaries being rebuilt - this may be undesirable to some, so be warned. This rebuilding will occur even if CMake was only configured to have a different stripping option and the binaries weren't actually compiled, so don't switch it on and off again and expect it to not rebuild all of your targeted binaries.

Example command to build all subsystems with stripped binaries:

```bash
cmake -DSMALLSAT_SUBSYSTEMS="" -DSTRIP_BINARIES=ON ..
```

### Getting version from binaries

To get the version embedded into the binary executable, run

```bash
strings <binary> | grep "Nebula-v*"
```

## Running

### Scripts

The scripts in `Nebula/scripts` are intended to be copied into a
directory in the `PATH` environment variable so that they can be
executed without giving their full file paths.  When you are
developing scripts, your local `scripts` directory should be in the
PATH variable so that if you edit a script that's called by another
script, it calls your version rather than the installed version of
that script.

The PATH variable has colon-separated directories to search for
commands.  If Nebula is cloned into your home directory, run the
following command to add the Nebula/scripts directory to PATH (or if
Nebula is cloned somewhere else, substitute its actual location):

```
export PATH="$HOME/Nebula/scripts:$PATH"
```

If you want this to be set automatically every time you log into a Pi,
you can edit your `~/.bashrc` file and add that line to it.

To check that PATH was set correctly, you can use `which` to find one
of the scripts.  Running `which gps_request` should give output like
`/home/flight/Nebula/scripts/gps_request`.


## Guides and Reference

[Contribution guide](./docs/contribution_guide.md)

[Systemd Service guide](./docs/service_guide.md)

[SSH and VPN guide](./docs/ssh_guide.md)

[Hardware reference](./docs/hardware_resources.md)

Data Packet Definitions : [drive link](https://docs.google.com/spreadsheets/d/10jFse666sjIIkAfqztmKOs7d_b8XoZnAh7q1Cqrj8B8/edit?usp=sharing)

IMPRESS Software Architecture : [drive link](https://drive.google.com/file/d/1e1auBI8sHP2bYqzt9Ne8o3tMmgifjRDq/view?usp=sharing)

EXACT Software Architecture : [drive link](https://drive.google.com/file/d/17Vt6iiWWpUYtNjWBMgAQTRFh8jcU7gn1/view?usp=sharing)

[initial CDH DHCP pi setup](./docs/pi_setup/dhcp_setup.md)

[usage and handy commands](.docs/usage_and_handy_commands.md)

## Working data

Each subsystem will be generating data and commands that will come across the UDP connections. Some of these files are available in the google drive

| Data type |  Link |
| :-------- | :--: |
| Flight-Like Data | [drive link](https://drive.google.com/drive/folders/1pdF-lz3KbJdMWmJr4hHVA85CNwIwqocH?usp=drive_link) |
| GPS data | [drive link](https://drive.google.com/drive/folders/1Hswe-Cn4tJcpB5LdSynPcEwDygpMpPQx?usp=drive_link) |
