#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "ZigbeeCore.h"
#include "ep/ZigbeeLight.h"
#include "heat.h"
#include "flash.h"

#define BUTTON_PIN            9  // ESP32-C6/H2 Boot button
#define ZIGBEE_LIGHT_ENDPOINT 10 // service identifier in the network
#define FLASH_SIZE            1  // we only want to store 1 bool (last state) in flash
#define FLASH_LAST_STATE      0  // store last state in index 0
#define REPEAT_EMIT_MINS      10 // boiler receiver requires us to repeat our signal every X mins or it thinks the thermostat is lost and turns the heat off

int lastEmitTimeMs = -1;

ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_LIGHT_ENDPOINT);

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Starting...");

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  zbLight.setManufacturerAndModel("Boon-Williams", "BoilerRC");
  zbLight.onLightChange(onZigbeeValueChange);
  Zigbee.addEndpoint(&zbLight);

  if (!Zigbee.begin(ZIGBEE_END_DEVICE)) {
    Serial.println("Failed to start Zigbee. Rebooting...");
    ESP.restart();
  }

  flashSetup(FLASH_SIZE); 
  setupHeat();

  Serial.println("Setup complete");
}

void loop() {
  emitLastKnownStateIfNeeded();

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
}

void onZigbeeValueChange(bool value) {
  demandHeat(value);

  flashSetBool(FLASH_LAST_STATE, value);
  lastEmitTimeMs = millis();
}

// emit last known state, ensuring to do it at least once on boot
void emitLastKnownStateIfNeeded() {
  int now = millis();
  int offsetSinceLastEmit = now - lastEmitTimeMs;

  if (lastEmitTimeMs == -1 || offsetSinceLastEmit >= (60000 * REPEAT_EMIT_MINS)) {
    bool lastState = flashGetBool(FLASH_LAST_STATE);

    Serial.printf("Emitting last known state: %s\n", lastState ? "true" : "false");
    demandHeat(lastState);

    lastEmitTimeMs = now;
  }
}

