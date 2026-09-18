# Directory Structure Documentation

On a flight Pi, there are a few main directories to take note of:
- `/SAT`
- `/usr/local/bin`
- `/etc/systemd/system`

Each directory plays a different role, detailed below.

## `/SAT`
The `/SAT` directory is where most of the work the satellite does takes place. It's where packets are created, scanned for, and stored. The `/SAT` directory contains a few subdirectories:
- `/LIVE`
  - The directory where open health packet files are actively being written to by their respective services.
- `/HOT`
  - The directory where health packet files are transferred after being closed. This directory is scanned for new files periodically, and upon finding one it is compressed by NORM and sent to ground.
- `/SENT`
  - The directory where sent health packet files are stored, for archival / redundancy. In case a file is corrupted or lost, we can try to obtain it from here. Ideally, this folder should be upkept by deleting old, unneeded files to prevent storage from filling up.
- `/RECV`
  - The directory where files received from ground are initially stored. After a file is received, it will likely be moved to another directory.
 
Each `/SAT` subdirectory (besides `/SAT/RECV`) contains their own subdirectory structure, split up by subsystem:
- `/ADCS`
  - EXACT doesn't have an ADCS system, so it lacks this subdirectory
- `/CDH`
- `/CMD`
- `/DET`
  - For Detector health packets
- `/DET-SCI`
  - For detector science packets
- `/EPS`
- `/GPS`

 ## `/usr/local/bin`
This is the directory where we put our binary executables and bash scripts. This directory is part of the system PATH, so you don't need to specify the full file path to run something in this folder - just the name. This folder also contains the `/config` subdirectory, which contains the `health_bounds.json` file that the CDH Service loads its bounds from.
  
 ## `/etc/systemd/system`
 This is the directory where our systemd service files are stored.

 ## Other places to note
 `/etc/environment` is where our current mission-specific env file is located. On deployment, we delete the existing file and replace it with the correct file. Every systemd service should have the line `EnvironmentFile=/etc/environment`, which would then point towards the mission-specific env file.
