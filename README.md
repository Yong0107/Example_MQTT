# Example_MQTT
This is created for taking an exmple about how to use libmosquitto & json-c to pubilish message to MQTT broker.


necessary lib

apt-get install libmosquitto-dev

apt-get install libjson-c-dev

compile program

gcc -o mqtttest example_mqtt.c -lmosquitto -ljson-c

excute program

./mqtttest
-----------------------------------------------------
Before you excute this sample program, you should modify the following parameters:
Line 28 : char *mqtthost = "10.0.0.51"; // Target host

by Yong
