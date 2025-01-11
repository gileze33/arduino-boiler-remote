#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "ZigbeeCore.h"
#include "ep/ZigbeeLight.h"
#include "heat.h"

#define BUTTON_PIN            9  // ESP32-C6/H2 Boot button
#define ZIGBEE_LIGHT_ENDPOINT 10 // service identifier in the network

ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_LIGHT_ENDPOINT);

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Starting...");

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  zbLight.setManufacturerAndModel("Boon-Williams", "BoilerRC");
  zbLight.onLightChange(demandHeat);
  Zigbee.addEndpoint(&zbLight);

  if (!Zigbee.begin(ZIGBEE_END_DEVICE)) {
    Serial.println("Failed to start Zigbee. Rebooting...");
    ESP.restart();
  }

  setupHeat();

  Serial.println("Setup complete");
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(100);  // Key debounce handling
    int startTime = millis();
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(50);
      if ((millis() - startTime) > 5000) {
        Serial.printf("Resetting Zigbee to factory settings, reboot.\n");
        Zigbee.factoryReset();
      }
    }
  }
  delay(100);



  // delete below here

  // delay(5000);
  // Serial.println("About to send on signal");
  // delay(1000);
  // demandHeat(true);
  // Serial.println("Sent on signal");
  
  // delay(5000);
  // Serial.println("About to send off signal");
  // delay(1000);
  // demandHeat(false);
  // Serial.println("Sent off signal");
}

