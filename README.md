# Example_MQTT
This is created for taking an exmple how to use libmosquitto & json-c to pubilish message to MQTT broker.


necessary lib
apt-get install libmosquitto-dev
apt-get install libjson-c-dev
compile program
gcc -o mqtttest example_mqtt_.c -lmosquitto -ljson-c
