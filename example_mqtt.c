#include <stdio.h>
#include <mosquitto.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <json-c/json.h>

void mosq_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
	/* Pring all log messages regardless of level. */
  
  switch(level){
    //case MOSQ_LOG_DEBUG:
    //case MOSQ_LOG_INFO:
    //case MOSQ_LOG_NOTICE:
    case MOSQ_LOG_WARNING:
    case MOSQ_LOG_ERR: {
      printf("%i:%s\n", level, str);
    }
  }
  
	
}
	struct mosquitto *mosq = NULL;
	char *topic = NULL;
void mqtt_setup(){

	char *host = "10.0.0.51";
	int port = 1883;
	int keepalive = 60;
	bool clean_session = true;
  	topic = "/testtopic";    //This is topic 
  
 	mosquitto_lib_init();
  	mosq = mosquitto_new(NULL, clean_session, NULL);
  	if(!mosq){
		fprintf(stderr, "Error: Out of memory.\n");
		exit(1);
	}
  
 	mosquitto_log_callback_set(mosq, mosq_log_callback);
  
  	if(mosquitto_connect(mosq, host, port, keepalive)){
		fprintf(stderr, "Unable to connect.\n");
		exit(1);
	}
  	int loop = mosquitto_loop_start(mosq);
  	if(loop != MOSQ_ERR_SUCCESS){
   		fprintf(stderr, "Unable to start loop: %i\n", loop);
    		exit(1);
  	}
}

int mqtt_send(char *msg){
  return mosquitto_publish(mosq, NULL, topic, strlen(msg), msg, 0, 0);
}

int main(int argc, char *argv[])
{
	struct json_object *jobj;
	mqtt_setup();
	int h = -1000;
	
	char *buf = malloc(64);
	struct timeval te;
  	while(1){ 
		time_t clk = time(NULL);
		jobj = json_object_new_object();
		json_object_object_add(jobj, "DI0", json_object_new_string("On"));
		json_object_object_add(jobj, "timestamp", json_object_new_string(ctime(&clk)));
 
    		sprintf(buf,json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
    		
		int snd = mqtt_send(buf);
    		if(snd != 0) printf("mqtt_send error=%i\n", snd);
    		sleep(1);
		json_object_put(jobj);
  	}
}
