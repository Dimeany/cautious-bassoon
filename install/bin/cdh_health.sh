#!/bin/bash

function health_data () {
  echo "Start CDH health"
  date

  echo -e "\nDisk usage"
  df

  echo -e "\nMemory usage"
  free

  echo -e "\nProcess list"
  ps ux

  echo -e "\nTemperature"
  MC="$(cat /sys/class/thermal/thermal_zone*/temp)"
  C="$(( MC / 1000 ))"
  echo "${C}°C"

  echo "\n"
}

while :; do
  health_data | sendwait -s -a 127.0.0.1 -p 51001
  sleep 600
done
