#!/bin/bash         

CONFIG_DIR=/home/pgs/config

CODE_DIR=/home/pgs/code

cd $CONFIG_DIR

CONFIG_FILE=`ls | grep xml`

SERVER_IP=`cat $CONFIG_FILE | grep server_ip | cut -d '"' -f2`

PORT_NO=$(cat $CONFIG_FILE | grep port | cut -d '"' -f2)

HOST_IP=$(ifconfig eth0 | grep inet | cut -d ":" -f2| cut -d " " -f1)

LEVEL_ID=$(cat $CONFIG_FILE | grep level_controller_ip |grep $HOST_IP| cut -d '-' -f1 | cut -d 'p' -f2 | cut -d ' ' -f2) 

CONTROLLER_NO=$(cat $CONFIG_FILE | grep Controller_no | grep $LEVEL_ID | cut -d '"' -f2)

cd $CODE_DIR

./update_occupancy $1 $2

DATA=`echo "31-$CONTROLLER_NO-$1-$2-$HOST_IP occupancy updated for zone $1"`

./send_data_to_server $SERVER_IP $PORT_NO $DATA

exit 0

