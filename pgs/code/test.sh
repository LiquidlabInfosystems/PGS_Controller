#!/bin/bash
cd /home/pgs/config/

touch  temp1.txt

xml2 < config_v4.xml >temp1.txt

exit 0
