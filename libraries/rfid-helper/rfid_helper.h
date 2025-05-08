#ifndef RFID_H
#define RFID_H


#define DEBUG_MODE
#define RFID_ENABLE


// for statically sized arrays
#define LENGTHOF(array) (sizeof(array)/sizeof(*(array)))
#define STRINGIFY(name) #name

#define MIN(A, B) ({typeof(A) a = (A); typeof(B) b = (B); a <= b ? a : b;})
#define MAX(A, B) ({typeof(A) a = (A); typeof(B) b = (B); a > b ? a : b;})
#define MAP(val, mini, maxi, mino, maxo) (((float)(val)-(mini))*((float)((maxo)-(mino))/((maxi)-(mini)))+mino)
#define MAPCAP(val, mini, maxi, mino, maxo) MAX(MIN(MAP(val, mini, maxi, mino, maxo), maxo), mino)
#define ABS(val) ({typeof(val) n = (val); n > 0 ? n : -n;})
#define SIGN(val) ((val) > 0 ? 1 : -1)

#define XCHG(A, B) do { \
  typeof(A) temp = A;   \
  A = B;                \
  B = temp;             \
} while(0)


#ifdef DEBUG_MODE
#include <stdarg.h>
void Serial_printf(char*, ...);
#define Serial_begin(...) Serial.begin(__VA_ARGS__)
#define Serial_print(...) Serial.print(__VA_ARGS__)
#define Serial_println(...) Serial.println(__VA_ARGS__)
#else
#define Serial_begin(...)
#define Serial_print(...)
#define Serial_println(...)
#define Serial_printf(...)
#endif


#ifndef RFID_DISABLE
void RFID_init();
int8_t RFID_scancard();
int8_t RFID_closescan();
int8_t RFID_readblock_keyA(void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len);
int8_t RFID_readblock_keyB(void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len);
int8_t RFID_writeblock_keyA(const void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len);
int8_t RFID_writeblock_keyB(const void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len);
#endif


#endif
