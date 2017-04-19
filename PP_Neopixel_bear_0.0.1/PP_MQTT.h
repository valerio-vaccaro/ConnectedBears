#ifndef pp_mqtt_h
#define pp_mqtt_h

#include "PP_NEOPIXEL.h"

int r,g,b;

/***
 * libraries for PubSub MQTT implementation and json management 
 ***/
#include <PubSubClient.h>
#include <ArduinoJson.h>

/***
 * MQTT server confihguration
 ***/
#define SERVER      "my_mqtt_server"
#define SERVERPORT  1883
#define USERNAME    ""
#define KEY         ""

/***
 * MQTT standard queues
 ***/
char topic_deploy_send[100];
char topic_sensor_send[100];
char topic_sensor_receive[100];
char topic_update_receive[100];

/***
 * Create an ESP8266 WiFiClient class to connect to the MQTT server.
 ***/
WiFiClient client;
PubSubClient mqttclient(client);


/***
 * MQTT receiver callback
 ***/
void mqttReceiveCallback(char* topic, byte* payload, unsigned int length) {
  Serial.println(F("=================="));
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println(F("=================="));
  Serial.println(F("Kind of message:"));

  /* update message */
  if (strcmp(topic, topic_update_receive) == 0) {
    updateFirmware();
  }
  /* received message */
  else if (strcmp(topic, topic_sensor_receive) == 0) {
    char inData[1000];
    Serial.print("payload: ");
    for(int i=0; i<length; i++){
      Serial.print((char)payload[i]);
       inData[i] = (char)payload[i];
     }
     Serial.println();
     StaticJsonBuffer<1000> jsonBuffer_r;
     JsonObject& root_r = jsonBuffer_r.parseObject(inData);
     r=root_r["R"];
     g=root_r["G"];
     b=root_r["B"];
     pp_neopixel_change_color(r, g, b);
     
     // Once connected, publish an announcement...
     // Create JSON message
     StaticJsonBuffer<500> jsonBuffer2;
     char sens_buff2[500];
     JsonObject& root2 = jsonBuffer2.createObject();
     root2["Type"] = firmware_name;
     root2["Version"] = firmware_version;
     root2["Filename"] = source_filename;
     //root2["CompilationTime"] = compile_date;
     root2["ChipId"] = String(ESP.getChipId(), HEX);
     root2["Deploy"] = topic_deploy_send;
     root2["Update"] = topic_update_receive;
     JsonObject& d = root2.createNestedObject("d");
     d["R"] = r;
     d["G"] = g;
     d["B"] = b;
     root2.printTo(sens_buff2, 500);
     
     Serial.print(F("Topic: "));
     Serial.println(topic_sensor_send);
     Serial.print(F("Message: "));
     Serial.println(sens_buff2);
     if (! mqttclient.publish(topic_sensor_send, sens_buff2)) {
       Serial.println(F("Send: Failed"));
     } else {
       Serial.println(F("Send: OK!"));
     } 
  }
  /*default*/
  else {
    
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttclient.connect("ESP8266Client2")) {
      Serial.println("connected");
      // esubscribe
      mqttclient.subscribe(topic_sensor_receive);
      mqttclient.subscribe(topic_update_receive);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttclient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void pp_mqtt_setup() {
  mqttclient.setServer(SERVER, SERVERPORT);
  mqttclient.setCallback(mqttReceiveCallback);
  String buf0 = String("iot/T/esp8266/I/") + String(ESP.getChipId(), HEX) + String("/D/sensor/F/json");
  buf0.toCharArray(topic_sensor_send, 100);
  String buf1 = String("iot/T/esp8266/I/") + String(ESP.getChipId(), HEX) + String("/D/deploy/F/json");
  buf1.toCharArray(topic_deploy_send, 100);
  String buf2 = String("iot/T/esp8266/I/") + String(ESP.getChipId(), HEX) + String("/C/sensor/F/json");
  buf2.toCharArray(topic_sensor_receive, 100);
  String buf3 = String("iot/T/esp8266/I/") + String(ESP.getChipId(), HEX) + String("/C/update/F/json");
  buf3.toCharArray(topic_update_receive, 100);
}

void pp_mqtt_loop() {
  if (! mqttclient.connected() ) {
    reconnect();
  }
  mqttclient.loop();
}

#endif
