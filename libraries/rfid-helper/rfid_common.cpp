#include "rfid_helper.h"


#ifdef DEBUG_MODE

void Serial_printf(char* format, ...) {
  char str[256];
  va_list args;
  va_start(args, format);
  vsprintf(str, format, args);
  va_end(args);
  Serial.print(str);
}

#endif
