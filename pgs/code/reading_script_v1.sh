#!/bin/bash

cd /home/pgs/config/

touch temp.txt lines.txt parameters.txt

xml2 < config_v4.xml >temp.txt

lines=`cat temp.txt | wc -l`

cat temp.txt | grep @ip | cut -d '=' -f2 >  parameters.txt 

#cat parameters.txt

port_no=$(cat temp.txt | grep @port | cut -d '=' -f2)

echo $(awk '{print $0 " "'$port_no'}' parameters.txt) > parameters.txt 

controller_no=$(cat temp.txt | grep Controller_no | cut -d '=' -f2)

echo $(awk '{print $0" " '$controller_no'}' parameters.txt )> parameters.txt

level_spaces=$(cat temp.txt | grep @L1-spaces | cut -d "=" -f2)

echo $(awk '{print $0" " '$level_spaces'}' parameters.txt)> parameters.txt

level_zones=$(cat temp.txt | grep @L1-zones | cut -d "=" -f2)

echo $(awk '{print $0" " '$level_zones'}' parameters.txt)> parameters.txt

level_detectors=$(cat temp.txt | grep @L1-detectors | cut -d "=" -f2)

echo $(awk '{print $0" " '$level_detectors'}' parameters.txt)> parameters.txt

level_details=`cat parameters.txt | grep 51616`

c=1

zone_details=$(cat temp.txt | grep @Z$c_spaces | cut -d "=" -f2)

parameters=$level_details" "$zone_details

#echo $parameters

let directional_detector_count=2*$level_detectors

#echo $directional_detector_count

cd /home/pgs/code/

insmod GPIO_driver_v4.ko detector_count=$directional_detector_count

bash -c "./signal_user_v9  $parameters "

exit 0
