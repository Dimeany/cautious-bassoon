# EPS Subsystem Process

Download packages:
```
sudo apt-get update 
sudo apt install cmake
sudo apt install meson
sudo apt-get -y install libgpiod-dev
```

After cloning the code, run

```
git submodule update --init
```

To build, from the EPS-Subsystem-Process directory run

```
sudo meson build
cd build
ninja
```

To run the project, from the build directory run

```
./EpsSubsystemProcess
```
