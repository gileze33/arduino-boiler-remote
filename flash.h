#ifndef FLASH_DEFS_H
#define FLASH_DEFS_H

void flashSetup(int size);
bool flashGetBool(int index);
void flashSetBool(int index, bool value);

#endif