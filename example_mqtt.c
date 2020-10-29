#include <stdio.h>
#include <mosquitto.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <json-c/json.h>

struct mosquitto *mosq = NULL;
	char *mqtttopic = NULL;

void mosq_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
	//Callback Parameters
	//mosq	the mosquitto instance making the callback.
	//obj	the user data provided in mosquitto_new
	//level	the log message level from the values: MOSQ_LOG_INFO MOSQ_LOG_NOTICE MOSQ_LOG_WARNING MOSQ_LOG_ERR MOSQ_LOG_DEBUG
	//str	the message string.
	/* Pring all log messages regardless of level. */
  
  switch(level){
   // case MOSQ_LOG_DEBUG:
  //  case MOSQ_LOG_INFO:
  //  case MOSQ_LOG_NOTICE:
    case MOSQ_LOG_WARNING:
    case MOSQ_LOG_ERR: {
      printf("%i:%s\n", level, str);
    }
  }
  
	
}
	
void mqtt_settingup(){

	char *mqtthost = "10.0.0.51"; // Target host
	int mqttport = 1883; // MQTT broker's port
	bool clean_session = true;
  	mqtttopic = "/PLC1/Slot1";    //This is the publish topic
  	int keepalive = 60;
	
 	mosquitto_lib_init();
  	mosq = mosquitto_new(NULL, clean_session, NULL);
  	if(!mosq){
		fprintf(stderr, "Error: Out of memory.\n");
		exit(1);
	}
  
 	mosquitto_log_callback_set(mosq, mosq_log_callback); 
	//a callback function in the following form: void callback(struct mosquitto *mosq, void *obj, int level, const char *str)
	//Callback Parameters
	//mosq	the mosquitto instance making the callback.
	//obj	the user data provided in mosquitto_new
	//level	the log message level from the values: MOSQ_LOG_INFO MOSQ_LOG_NOTICE MOSQ_LOG_WARNING MOSQ_LOG_ERR MOSQ_LOG_DEBUG
	//str	the message string.

  
  	if(mosquitto_connect(mosq, mqtthost, mqttport, keepalive)){
		fprintf(stderr, "Unable to connect.\n");
		exit(1);
	}
  	int loop = mosquitto_loop_start(mosq);
	//This is part of the threaded client interface.  Call this once to start a new thread to process network traffic.  This provides an alternative to repeatedly calling mosquitto_loop yourself.
	//mosquitto_loop_start will returen the following value.
	//	MOSQ_ERR_SUCCESS	on success.
	//	MOSQ_ERR_INVAL	if the input parameters were invalid.
	//	MOSQ_ERR_NOT_SUPPORTED	if thread support is not available.
  	if(loop != MOSQ_ERR_SUCCESS){
   		fprintf(stderr, "Unable to start loop: %i\n", loop);
    		exit(1);
  	}
}

int mqtt_send(char *msg){
  return mosquitto_publish(mosq, NULL, mqtttopic, strlen(msg), msg, 0, 0); 
	//Publish MQTT to Broker, mosquitto_publish(mosq, mid, topic, payloadlen, payload, qos, retain)
	//Parameters
	//	mosq	a valid mosquitto instance.
	//	mid	pointer to an int.  If not NULL, the function will set this to the message id of this particular message.  This can be then used with the publish callback to determine when the message has been sent.  Note that although the MQTT protocol doesn’t use message ids for messages with QoS=0, libmosquitto assigns them message ids so they can be tracked with this parameter.
	//	topic	null terminated string of the topic to publish to.
	//	payloadlen	the size of the payload (bytes).  Valid values are between 0 and 268,435,455.
	//	payload	pointer to the data to send.  If payloadlen > 0 this must be a valid memory location.
	//	qos	integer value 0, 1 or 2 indicating the Quality of Service to be used for the message.
	//	retain	set to true to make the message retained.
}

int main(int argc, char *argv[])
{
	struct json_object *jobj; //Json object.
	time_t clk; //timestamp object
	mqtt_settingup();
	
	char *buf = malloc(64); // MQTT data buffer.
	
  	while(1){ 
		clk = time(NULL);
		jobj = json_object_new_object();
		json_object_object_add(jobj, "DI0", json_object_new_string("On"));
		json_object_object_add(jobj, "timestamp", json_object_new_string(ctime(&clk)));
 
    		sprintf(buf,json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY)); //Inject data into buffer.
    		
		int snd = mqtt_send(buf); //call Publish function to send MQTT data
    		if(snd != 0) printf("mqtt_send error=%i\n", snd);
    		sleep(1);
		json_object_put(jobj);
  	}
}
