#include <EEPROM.h>

/** @public */
void flashSetup(int size) {
  EEPROM.begin(size);
}

/** @public */
bool flashGetBool(int index) {
  int value = EEPROM.read(index);

  if (value == 1) {
    return true;
  }
  else {
    return false;
  }
}

/** @public */
void flashSetBool(int index, bool value) {
  int valueToWrite = 0;
  if (value) {
    valueToWrite = 1;
  }

  EEPROM.write(index, valueToWrite);
  EEPROM.commit();
}