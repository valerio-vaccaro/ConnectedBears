const char firmware_name[]     = "PP_Neopixel_bear";
const char firmware_version[]  = "0.0.1";
const char source_filename[]   = __FILE__;
const char compile_date[]      = __DATE__ " " __TIME__;

#include "PP_WIFI.h"
#include "PP_MQTT.h"
#include "PP_NEOPIXEL.h"

void setup() {
  Serial.begin(115200);
  Serial.println(F("=================="));
  Serial.println(source_filename);
  Serial.println(compile_date);
  pp_wifi_setup();
  pp_mqtt_setup();
  if (! mqttclient.connected() ) {
    reconnect();
  }
  // Create JSON message
  StaticJsonBuffer<1000> jsonBuffer;
  char sens_buff[1000];
  JsonObject& root = jsonBuffer.createObject();
  root["Type"] = firmware_name;
  root["Version"] = firmware_version;
  root["Filename"] = source_filename;
  root["ChipId"] = String(ESP.getChipId(), HEX);
  root["Deploy"] = topic_deploy_send;
  root["Update"] = topic_update_receive;
  JsonArray& Pub = root.createNestedArray("Pub");
  Pub.add(topic_sensor_receive);
  JsonArray& Sub = root.createNestedArray("Sub");
  Sub.add(topic_sensor_send);
  root.printTo(sens_buff, 1000);
  Serial.print(F("Topic: "));
  Serial.println(topic_deploy_send);
  Serial.print(F("Message: "));
  Serial.println(sens_buff);
  if (! mqttclient.publish(topic_deploy_send, sens_buff)) {
    Serial.println(F("Send: Failed"));
  } else {
    Serial.println(F("Send: OK!"));
  }
  pp_neopixel_setup();
}

void loop() {
  pp_mqtt_loop();
  delay(100);
}


