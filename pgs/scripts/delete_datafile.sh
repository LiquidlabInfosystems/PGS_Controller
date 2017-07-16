#!/bin/bash

DATA_DIR=/home/pgs/data

CODE_DIR=/home/pgs/code

CONFIG_DIR=/home/pgs/config

cd $CONFIG_DIR

CONFIG_FILE=`ls | grep xml`

SERVER_IP=`cat $CONFIG_FILE | grep server_ip | cut -d '"' -f2`

PORT_NO=$(cat $CONFIG_FILE | grep port | cut -d '"' -f2)

HOST_IP=$(ifconfig eth0 | grep inet |cut -d ":" -f2 | cut -d " " -f1)


cd $DATA_DIR

DATA_FILE=`ls | grep bin`

rm $DATA_FILE


DATA=`echo "$HOST_IP Data file Deleted Successfully...Controller Rebooting.."`

cd $CODE_DIR

./send_data_to_server $SERVER_IP $PORT_NO $DATA

bash -c reboot

exit 0
