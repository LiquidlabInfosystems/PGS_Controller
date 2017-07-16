#!/bin/bash

CONFIG_DIR=/home/pgs/config

CODE_DIR=/home/pgs/code

DATA_DIR=/home/pgs/data

HOST_IP=$(ifconfig eth0 | grep inet | cut -d ":" -f2| cut -d " " -f1)


cd $CONFIG_DIR


CONFIG_FILE=`ls | grep xml`

LEVEL_ID=$(cat $CONFIG_FILE | grep level_controller_ip |grep $HOST_IP| cut -d '-' -f1 |cut -d 'p' -f2 | cut -d ' ' -f2)


CONTROLLER_NO=$(cat $CONFIG_FILE | grep Controller_no | grep $LEVEL_ID | cut -d '"' -f2)


DATA=`echo "32-$CONTROLLER_NO-Controller is ready to accept new config file"`




SERVER_IP=`cat $CONFIG_FILE | grep server_ip | cut -d '"' -f2`

PORT_NO=$(cat $CONFIG_FILE | grep port | cut -d '"' -f2)

rm *.xml

cd $DATA_DIR

rm new_data.bin

rm zone_space_data.bin

cd $CODE_DIR


./send_data_to_server $SERVER_IP $PORT_NO $DATA
 
cd $CONFIG_DIR

while [ ! `ls | grep xml` ]
do
:
done


	NEW_CONFIG_FILE=`ls | grep xml`

	NEW_SERVER_IP=`cat $CONFIG_FILE | grep server_ip | cut -d '"' -f2`
	
	NEW_PORT_NO=$(cat $CONFIG_FILE | grep port | cut -d '"' -f2)

	HOST_IP=$(ifconfig eth0 | grep inet | cut -d ":" -f2| cut -d " " -f1)


	LEVEL_ID=$(cat $CONFIG_FILE | grep level_controller_ip |grep $HOST_IP| cut -d '-' -f1 | cut -d 'p' -f2 | cut -d ' ' -f2)

	CONTROLLER_NO=$(cat $CONFIG_FILE | grep Controller_no | grep $LEVEL_ID | cut -d '"' -f2)
	

	DATA=`echo "33-$CONTROLLER_NO-New Config File Downloaded Successfully"`

	cd $CODE_DIR

	./send_data_to_server $NEW_SERVER_IP $NEW_PORT_NO $DATA



bash -c shutdown -r now
