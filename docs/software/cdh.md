### CDH Service

The CDH (Command and Data Handling) service is mainly responsible for collecting all of the health packets from the various other services into one place. It caches the most recent packets and handles sending out H&S packets and LRT packets at the correct rate to the COMMS service. Cached packets must be checked for staleness before sending out to make sure we don't repeat old data.

## EXACT Specific

We send out H&S packets once per second. Because we only receive new health packets every 10 seconds, we need to cache and repeat values in the H&S packets. However, we also want to only send valid data - meaning we need to filter out old data, zeroing out stale fields in the H&S packet.

## CVTs

The CVTs (Current value tables) hold the most recent values from a specific health packet. They hold values as a dictionary mapping a name to a field. We have a seperate CVT for each different health packet.
Each field in a CVT contains information about the bounds the value should be between, the action to take if the value is out-of-bounds, and the current value held in the field.

## Bounds Loader

The bounds loader parses a json file (By default ```/usr/local/bin/config/health_bounds.json```, but can also be specified in the environment file as HEALTH_BOUNDS_PATH). This file should be where the bounds for the CVTs should be specified, to avoid having magic numbers in code, and to easily change the bounds without having to recompile. 
