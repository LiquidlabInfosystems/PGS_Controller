#!/bin/bash

CONFIG_DIR=/home/pgs/config

SCRIPTS_DIR=/home/pgs/scripts

CODE_DIR=/home/pgs/code

DATA_DIR=/home/pgs/data

PARAM_DIR=/home/pgs/parameters

IS_MAIN_CONTROLLER=0

cd $SCRIPTS_DIR

cp reset_occupancy.sh delete_datafile.sh download.sh update_occupancy.sh view_status.sh /usr/local/bin/

cd $PARAM_DIR

touch main_parameters.txt level_parameters.txt create_datafile_parameters.txt log.txt

cd $CONFIG_DIR

CONFIG_FILE=`ls | grep xml`

SERVER_IP=`cat $CONFIG_FILE | grep server_ip | cut -d '"' -f2 | tee $PARAM_DIR/level_parameters.txt $PARAM_DIR/main_parameters.txt` 


#server_ip=`cat $config_file | grep server_ip | cut -d '"' -f2 > main_parameters.txt` 


PORT_NO=$(cat $CONFIG_FILE | grep port | cut -d '"' -f2)


echo $(awk '{print $0 " "'$PORT_NO'}' $PARAM_DIR/level_parameters.txt) > $PARAM_DIR/level_parameters.txt 


echo $(awk '{print $0 " "'$PORT_NO'}' $PARAM_DIR/main_parameters.txt) > $PARAM_DIR/main_parameters.txt 


HOST_IP=$(ifconfig eth0 | grep inet | cut -d ":" -f2| cut -d " " -f1)


#echo host-ip $host_ip


MAIN_CONTROLLER_IP=$(cat $CONFIG_FILE | grep main_controller_ip | cut -d '"' -f2)


#echo $(awk '{print $0 " "'$MAIN_CONTROLLER_IP'}' $PARAM_DIR/parameters.txt) > $PARAM_DIR/level_parameters.txt


#echo main controller ip $main_controller_ip


#cat $PARAM_DIR/level_parameters.txt


#echo $HOST_IP


LEVEL_CONTROLLER_IP=$(cat $CONFIG_FILE |grep level_controller_ip | grep $HOST_IP |cut -d '"' -f2)


#echo level-controller-ip $level_controller_ip


LEVEL_ID=$(cat $CONFIG_FILE | grep level_controller_ip |grep $HOST_IP| cut -d '-' -f1 | cut -d 'p' -f2 | cut -d ' ' -f2)


#echo level-id $level_id


CONTROLLER_NO=$(cat $CONFIG_FILE | grep Controller_no | grep $LEVEL_ID | cut -d '"' -f2)


#echo controller no $CONTROLLER_NO


echo $(awk '{print $0" " '$CONTROLLER_NO'}' $PARAM_DIR/level_parameters.txt) > $PARAM_DIR/level_parameters.txt


TOTAL_SPACES=$(cat $CONFIG_FILE | grep Total_spaces | cut -d '"' -f2)


echo $(awk '{print $0 " "'$TOTAL_SPACES'}' $PARAM_DIR/main_parameters.txt) > $PARAM_DIR/main_parameters.txt


#echo total spaces $total_spaces


TOTAL_LEVELS=$(cat $CONFIG_FILE | grep Total_levels | cut -d '"' -f2)


echo $(awk '{print $0 " "'$TOTAL_LEVELS'}' $PARAM_DIR/main_parameters.txt) > $PARAM_DIR/main_parameters.txt


echo $(awk '{print $0" " '$TOTAL_LEVELS'}' $PARAM_DIR/level_parameters.txt )> $PARAM_DIR/level_parameters.txt


ZONES=$(cat $CONFIG_FILE | grep zones | cut -d '"' -f2)


#echo $(awk '{print $0" " '$zones'}' level_parameters.txt )> level_parameters.txt


echo $ZONES>>$PARAM_DIR/level_parameters.txt


echo $(paste -s -d " " $PARAM_DIR/level_parameters.txt) > $PARAM_DIR/level_parameters.txt


#echo Total_levels $total_levels


#echo zones $zones


#bash -c "ifconfig eth0 $LEVEL_CONTROLLER_IP"


LEVEL_SEARCH_KEYWORD=$(echo "$LEVEL_ID"-spaces) 


LEVEL_SPACES=$(cat $CONFIG_FILE |grep $LEVEL_SEARCH_KEYWORD |cut -d '"' -f2)


#echo $level_id spaces $level_spaces


echo $(awk '{print $0" " '$LEVEL_SPACES'}' $PARAM_DIR/level_parameters.txt)> $PARAM_DIR/level_parameters.txt


LEVEL_ZONES=$(cat $CONFIG_FILE | grep zones |grep $LEVEL_ID | cut -d '"' -f2)


echo $(awk '{print $0" " '$LEVEL_ZONES'}' $PARAM_DIR/level_parameters.txt)> $PARAM_DIR/level_parameters.txt


LEVEL_DETECTORS=$(cat $CONFIG_FILE | grep detectors |grep $LEVEL_ID |cut -d '"' -f2)


#echo detectors $level_detectors


echo $(awk '{print $0" " '$LEVEL_DETECTORS'}' $PARAM_DIR/level_parameters.txt)> $PARAM_DIR/level_parameters.txt


#level_details=`cat parameters.txt | grep 51614`


ZONE_DETAILS=$(cat $CONFIG_FILE| grep Z$c_spaces |grep $LEVEL_ID| cut -d '"' -f2 | cut -d '>' -f2)


echo $ZONE_DETAILS >> $PARAM_DIR/level_parameters.txt


LEVEL_PARAMETERS=$(awk 'NR%2{printf $0" ";next;}1' $PARAM_DIR/level_parameters.txt)


ZONES=$(cat $CONFIG_FILE | grep zones | cut -d '"' -f2)


echo $ZONES>>$PARAM_DIR/main_parameters.txt


MAIN_PARAMETERS=$(awk 'NR%2{printf $0" ";next;}1' $PARAM_DIR/main_parameters.txt)


#echo main_parameters $main_parameters


#echo $level_parameters


let DIRECTIONAL_DETECTOR_COUNT=2*$LEVEL_DETECTORS


#echo $directional_detector_count


if [ "$(ls -A $DATA_DIR)" ]; then

     		echo "Take action $DIR is not Empty"



else
	        echo "$DATA_DIR is Empty"
            
		cd $CODE_DIR

		#echo $TOTAL_LEVELS $ZONES>create_datafile_parameters.txt
		
		#create_datafile_parameters=$(awk 'NR%2{printf $0" ";next;}1' create_datafile_parameters.txt)
	
		bash -c "./create_datafile $LEVEL_ZONES"
fi


cd $CODE_DIR



if [ "$MAIN_CONTROLLER_IP" == "$LEVEL_CONTROLLER_IP" ]

then
		cd $CODE_DIR

		IS_MAIN_CONTROLLER=1

		#echo main paramters $MAIN_PARAMETERS

		bash -c "./main_test $MAIN_PARAMETERS"&
		
		#echo main controller
fi



if lsmod | grep --quiet  GPIO; then

		#rmmod GPIO_driver_v4
		 rmmod GPIO_driver_v5_lulu

fi

	#	insmod GPIO_driver_v4.ko detector_count=$DIRECTIONAL_DETECTOR_COUNT
		insmod GPIO_driver_v5_lulu.ko detector_count=$DIRECTIONAL_DETECTOR_COUNT



#echo IS_MAIN_CONTROLLER $IS_MAIN_CONTROLLER


#echo MAIN_CONTROLLER_IP $MAIN_CONTROLLER_IP


#echo MAIN_PARAMETERS $MAIN_PARAMETERS


#echo LEVEL_PARAMETERS $LEVEL_PARAMETERS


echo $IS_MAIN_CONTROLLER $MAIN_CONTROLLER_IP $LEVEL_PARAMETERS>level_controller_parameters_lulu.txt

bash -c "./level_controller $IS_MAIN_CONTROLLER $MAIN_CONTROLLER_IP $LEVEL_PARAMETERS"&


#rm level_parameters.txt

exit 0
