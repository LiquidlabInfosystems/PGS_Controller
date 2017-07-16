#!/bin/bash

cd /home/pgs/main_config/
#cd /home/rakesh/Liquidlab/Projects/PGS/Main_controller/ver-1.0/

touch main_parameters.txt lines.txt level_parameters.txt

lines=`cat config_v4.xml | wc -l`


server_ip=`cat config_v4.xml | grep server_ip | cut -d '"' -f2 > level_parameters.txt` 


server_ip=`cat config_v4.xml | grep server_ip | cut -d '"' -f2 > main_parameters.txt` 

port_no=$(cat config_v4.xml | grep port | cut -d '"' -f2)


echo $(awk '{print $0 " "'$port_no'}' level_parameters.txt) > level_parameters.txt 
echo $(awk '{print $0 " "'$port_no'}' main_parameters.txt) > main_parameters.txt 


host_ip=$(ifconfig eth0 | grep inet | cut -d ":" -f2| cut -d " " -f1)

echo host-ip $host_ip

main_controller_ip=$(cat config_v4.xml | grep main_controller_ip | cut -d '"' -f2)

#echo $(awk '{print $0 " "'$main_controller_ip'}' parameters.txt) > level_parameters.txt
echo main controller ip $main_controller_ip

cat level_parameters.txt
level_controller_ip=$(cat config_v4.xml |grep level_controller_ip | grep $host_ip |cut -d '"' -f2)

echo level-controller-ip $level_controller_ip

level_id=$(cat config_v4.xml | grep level_controller_ip |grep $host_ip| cut -d '-' -f1 | cut -d " " -f2)

echo level-id $level_id


controller_no=$(cat config_v4.xml | grep Controller_no | grep $level_id | cut -d '"' -f2)

echo controller no $controller_no

echo $(awk '{print $0" " '$controller_no'}' level_parameters.txt )> level_parameters.txt


total_spaces=$(cat config_v4.xml | grep Total_spaces | cut -d '"' -f2)

echo $(awk '{print $0 " "'$total_spaces'}' main_parameters.txt) > main_parameters.txt

echo total spaces $total_spaces

total_levels=$(cat config_v4.xml | grep Total_levels | cut -d '"' -f2)
echo $(awk '{print $0 " "'$total_levels'}' main_parameters.txt) > main_parameters.txt


#bash -c "ifconfig eth0 $level_controller_ip"

level_search_keyword=$(echo "$level_id"-spaces) 
level_spaces=$(cat config_v4.xml |grep $level_search_keyword |cut -d '"' -f2)

echo $level_id spaces $level_spaces

echo $(awk '{print $0" " '$level_spaces'}' level_parameters.txt)> level_parameters.txt

level_zones=$(cat config_v4.xml | grep zones |grep $level_id | cut -d '"' -f2)

echo $(awk '{print $0" " '$level_zones'}' level_parameters.txt)> level_parameters.txt


level_detectors=$(cat config_v4.xml | grep detectors |grep $level_id |cut -d '"' -f2)

echo detectors $level_detectors

echo $(awk '{print $0" " '$level_detectors'}' level_parameters.txt)> level_parameters.txt

level_details=`cat parameters.txt | grep 51614`

zone_details=$(cat config_v4.xml | grep Z$c_spaces |grep $level_id| cut -d '"' -f2 | cut -d '>' -f2)

echo $zone_details >> level_parameters.txt

level_parameters=$(awk 'NR%2{printf $0" ";next;}1' level_parameters.txt)

main_parameters=$(awk 'NR%2{printf $0" ";next;}1' main_parameters.txt)



echo $level_parameters

let directional_detector_count=2*$level_detectors

echo $directional_detector_count

cd /home/pgs/main_code/

if [ "$main_controller_ip" == "$level_controller_ip" ]
then
cd /home/pgs/main_code/
echo main parameters $main_parameters
bash -c "./main_controller $main_parameters"&
echo main controller
fi


if lsmod | grep --quiet  GPIO; then
rmmod GPIO_driver_v4
fi
insmod GPIO_driver_v4.ko detector_count=$directional_detector_count


bash -c "./signal_user_v9 $main_controller_ip $level_parameters"&

exit 0
