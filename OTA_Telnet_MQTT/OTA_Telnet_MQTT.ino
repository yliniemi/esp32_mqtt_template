#define HOSTNAME "NoloHuone"      // replace this with the name for this particular device. everyone deserves a unique name
#define OTA_ROUNDS 21             // this is how many seconds we waste waiting for the OTA during boot. sometimes people make mistakes in their code - not me - and the program freezes. this way you can still update your code over the air even if you have some dodgy code in your loop
#include <myCredentials.h>        // oh yeah. these is myCredentials.zip on the root of this repository. include it as a library and the edit the file with your onw ips and stuff

// #include <WiFi.h>
// #include <ESPmDNS.h>
// #include <WiFiUdp.h>
#include <EspMQTTClient.h>
#include "setupWifi.h"
#include "OTA.h"
#include "SerialOTA.h"

EspMQTTClient MQTTclient(
  //WIFI_SSID,
  //WIFI_PSK,
  MQTT_SERVER,      // MQTT Broker server ip
  1883,             // default unencrypted port is 1883
  MQTT_USERNAME,    // Can be omitted if not needed
  MQTT_PASSWORD,    // And this one too 
  HOSTNAME          // Client name that uniquely identify your device
);

void onConnectionEstablished()
{
  //MQTTclient.subscribe("feed/" + OTAhostname + "/#", [](const String & topic, const String & payload)
  MQTTclient.subscribe(String("feed/") + HOSTNAME + "/#", [](const String & topic, const String & payload)
  {
    Serial.println("From wildcard topic: " + topic + ", payload: " + payload);
    SerialOTA.println("From wildcard topic: " + topic + ", payload: " + payload);
  });
  MQTTclient.publish(String(HOSTNAME) + "/alive", "I just woke up");
}
//

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  
  setupWifi(WIFI_SSID, WIFI_PSK);
  
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  setupOTA(HOSTNAME, OTApassword, OTA_ROUNDS);

  setupSerialOTA(HOSTNAME);

  // setupMQTT(HOSTNAME, MQTT_SERVER);
  MQTTclient.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  String lastWillTopic = String(HOSTNAME) + "/lastwill";
  char lastWillTopicChar[lastWillTopic.length() + 1];
  lastWillTopic.toCharArray(lastWillTopicChar, lastWillTopic.length() + 1);
  SerialOTA.println(lastWillTopicChar);
  Serial.println(lastWillTopicChar);
  // MQTTclient.enableLastWillMessage(lastWillTopicChar, "What a world, what a world!");  // For some reason this line prevents MQTT connection altogether
}

void loop()
{
  ArduinoOTA.handle();  
  SerialOTAhandle();
  MQTTclient.loop();
  //char payload_char[100];
  static int loop_count = 0;
  loop_count++;
  Serial.print("loop count: ");
  Serial.println(loop_count);
  SerialOTA.print("loop count: ");
  SerialOTA.println(loop_count);
  //sprintf(payload_char, "%d", loop_count);
  String payloadStr = String(loop_count);
  String hostnameStr = HOSTNAME;
  MQTTclient.publish(String(HOSTNAME) + "/loop_count", payloadStr);
  delay(1000);
}
