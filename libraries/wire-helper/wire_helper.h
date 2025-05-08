#ifndef WIRE_HELPER_H
#define WIRE_HELPER_H


#define DEBUG_MODE



//#define DISABLE_WIRE
#define WIRE_TIMEOUT 1000e3      // MICROSECONDS
#define WIRE_FREQUENCY 10e3      // Hz
// 10KHz slow mode (unsure if uno supports)
// 100KHz default
// 400KHz fast mode (max for uno, probably)


#ifndef DISABLE_WIRE
  #include <Wire.h>
#endif
#include "Arduino.h"


#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"


#ifdef DEBUG_MODE
  #include <stdio.h>
#endif


#ifdef ADAFRUIT_WIRE
  //#undef WIRE_TIMEOUT
  #define WIRE_ADAFRUIT
#endif

#ifdef BUFFER_LENGTH /* defined in Wire.h */
  #define WBUFF_SIZE BUFFER_LENGTH
#else
  #define WBUFF_SIZE 32
#endif


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


#define NO_ERROR 0
#define ERR_SUCCESS NO_ERROR

// I wonder if I should have made them all structs.
enum MSGTYPE {
  MSG_ERROR = 255,
  MSG_NULL = 0,
  MSG_REQUEST,
  MSG_STATE_FLAGS,
  MSG_SERVO_DATA,
  MSG_SERVO_PACKET,
  MSG_JOYSTICK,
  MSG_CONTROL_DATA,
  MSG_HEARTS,
  MSGTYPE_COUNT // the number of message types
};

enum TRANSMISSION_ERR {
  TRANS_SUCCESS = 0,
  TRANS_OVERFLOW,
  TRANS_NACKADDRESS,
  TRANS_NACKDATA,
  TRANS_OTHER,
  TRANS_TIMEOUT
};

enum SERVO_INDEXES {
  SERVO_RIGHTARM = 0,
  SERVO_LEFTARM,
  SERVO_LEGS,
  SERVO_COUNT
};

enum GAME_STATES {
  STATE_WAIT_SETUP = 0,
  STATE_WAIT_LOOP,
  STATE_GAME_SETUP,
  STATE_GAME_LOOP,
  STATE_END,
};

// organize data by types, not by who they are for!
// data is actual datatypes
// packets are grouped unassociated data
// can send both packets and data

union STATE_FLAGS {
  struct {
    uint8_t state: 8; // currently the only flag, so might as well be 8 bits
  };
  uint8_t all;
};

union SERVO_DATA {
  struct {
    uint8_t right;
    uint8_t left;
    uint8_t legs;
  };
  int8_t all[3];
};

struct SERVO_PACKET {
  SERVO_DATA servos[2];
};

// no for sending, currently
union STRAIN_DATA {
  struct {
    uint8_t right;
    uint8_t left;
  };
  uint8_t all[2];
};

struct CONTROL_DATA {
  STRAIN_DATA strains;
  int8_t joystick;
};


// STATIC DATA MEMBER TYPES ONLY
// If not static, it will still be transmitted as static
union MESSAGE {
  struct {
    uint8_t type;
    union {
      uint8_t request;
      uint8_t hearts;
      int8_t joystick;
      STATE_FLAGS flags;
      SERVO_DATA servos;
      SERVO_PACKET servopack;
      CONTROL_DATA controls;
      char string_[WBUFF_SIZE-1];
      //char buffer[WBUFF_SIZE-1];
    };
  };
  uint8_t raw[WBUFF_SIZE];
};


#define FORMAT_MESSAGE(msg_ptr, msg_type) ((msg_ptr)->type = (msg_type))
#define FORMAT_REQUEST(msg_ptr, msg_request) do {  \
  (msg_ptr)->type = MSG_REQUEST;                      \
  (msg_ptr)->request = msg_request;                   \
} while(0)


#define PSEUDO_MSG (*(MESSAGE*){0})


// ALWAYS RETURN A STATIC SIZE, NEVER DYNAMIC!!!
// Rename to MSGTYPE_RAWSIZE?
#define MSGTYPE_SIZEOF(msg_type) ({       \
  uint8_t ret;                            \
  switch (msg_type) {                     \
  case MSG_ERROR:                         \
  case MSG_NULL:                          \
    ret = 0;                              \
    break;                                \
  case MSG_REQUEST:                       \
    ret = sizeof(uint8_t);                \
    break;                                \
  case MSG_STATE_FLAGS:                   \
    ret = sizeof(STATE_FLAGS);            \
    break;                                \
  case MSG_SERVO_DATA:                    \
    ret = sizeof(SERVO_DATA);             \
    break;                                \
  case MSG_SERVO_PACKET:                  \
    ret = sizeof(SERVO_PACKET);           \
    break;                                \
  case MSG_JOYSTICK:                      \
    ret = sizeof(int8_t);                 \
    break;                                \
  case MSG_CONTROL_DATA:                  \
    ret = sizeof(CONTROL_DATA);           \
    break;                                \
  case MSG_HEARTS:                        \
    ret = sizeof(uint8_t);                \
    break;                                \
  default:                                \
    ret = WBUFF_SIZE-1;                   \
  } ret + sizeof(PSEUDO_MSG.type);        \
})

#define MESSAGE_SIZEOF(msg_ptr) (MSGTYPE_SIZEOF((msg_ptr)->type))


#ifdef DEBUG_MODE
#include <stdarg.h>
void MESSAGE_print(MESSAGE* msg_ptr);
void Serial_printf(char*, ...);
char* MSGTYPE_string(int type);
char* TRANS_ERR_string(int type);
extern const MESSAGE NULL_MESSAGE;
#define Serial_begin(...) Serial.begin(__VA_ARGS__)
#define Serial_print(...) Serial.print(__VA_ARGS__)
#define Serial_println(...) Serial.println(__VA_ARGS__)
#else
#define Serial_begin(...)
#define Serial_print(...)
#define Serial_println(...)
#define Serial_printf(...)
#define MESSAGE_print(...)
#endif


//#ifdef WIRE_MODE_MASTER
void wire_masterInit();
int send_message(MESSAGE* msg_ptr, int device);
int pull_message(MESSAGE* msg_ptr, uint8_t msg_type, int device);
int wire_printerror(int error);
//#endif

//#ifdef WIRE_MODE_SLAVE
void wire_slaveInit(int);
void wireReceiveEvent(int);
void wireRequestEvent();
// these are defined by the programmer
void wire_process_message(MESSAGE*);
void wire_process_request(MESSAGE*, uint8_t);
//#endif

// just ignore these, but don't delete them
#define WIRE_MODE_MASTER
#define WIRE_MODE_SLAVE


#endif
