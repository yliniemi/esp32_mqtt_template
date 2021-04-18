#include "OTA.h"

int OTArounds = 10;  // this is the default number of seconds wasted at boot time to ensure working over the air updates

void setupOTA(const char *hostname, const char *password)
{
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  
  ArduinoOTA.setPassword(password);
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      static int previousPercentage = -1;
      int percentage = progress / (total / 100);
      // this if statement makes it so that each percentage is printed only once
      if (percentage != previousPercentage)
      {
        Serial.printf("Progress: %u%%\r\n", percentage);
        //SerialOTA.printf("Progress: %u%%\r\n", percentage);  // uncommenting this risks OTA
        previousPercentage = percentage;
      }
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
  
  ArduinoOTA.begin();
  
  // this loop is here so that if your program freezes you can still update over the air
  for (int i = 1; i < OTArounds; i++)
  {
    Serial.print(i);
    Serial.println("st round of initial OTA");
    delay(1000);
    ArduinoOTA.handle();
  }
}

void setupOTA(const char *hostname, const char *password, int rounds)
{
  OTArounds = rounds;
  setupOTA(hostname, password);
}
