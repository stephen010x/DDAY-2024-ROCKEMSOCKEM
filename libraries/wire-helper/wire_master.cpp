#include "wire_helper.h"

#ifdef WIRE_MODE_MASTER
#ifndef DISABLE_WIRE

//void wire_masterInit();
//int send_message(MESSAGE* msg_ptr, int device);
//int pull_message(MESSAGE* msg_ptr, uint8_t msg_type, int device);
static int __send_message(MESSAGE* msg_ptr, int device);
static int __request_message(uint8_t msg_type, int device);
static int __get_message(MESSAGE* msg, uint8_t msg_type, int device);
static int wire_errorhandler(int error);
//int wire_printerror(int error);

void wire_masterInit() {
  #ifdef WIRE_TIMEOUT
  #ifdef WIRE_ADAFRUIT
  Wire.setTimeout(WIRE_TIMEOUT);
  Serial_printf("timeout: %d\n", WIRE_TIMEOUT);
  //#endif
  #else
  //#ifdef WIRE_TIMEOUT
  Wire.setWireTimeout(WIRE_TIMEOUT);
  #endif
  #endif
  #ifdef WIRE_FREQUENCY
  Wire.setClock(WIRE_FREQUENCY);
  #endif
  //Wire.begin(TARGET_BOARD);
  Wire.begin();
}

// send message to slave
int send_message(MESSAGE* msg_ptr, int device) {
  return wire_errorhandler(__send_message(msg_ptr, device));
}

// request and receive message from slave
int pull_message(MESSAGE* msg_ptr, uint8_t msg_type, int device) {
  int error = __request_message(msg_type, device);
  if (error != 0) 
    return wire_errorhandler(error);
  __get_message(msg_ptr, msg_type, device);
  return 0;
}

static int __send_message(MESSAGE* msg_ptr, int device) {
  delay(33);
  #ifdef VERBOSE_WIRE_DEBUG
  Serial_printf("Sending message to %s; ", BOARD_TYPE_string(device));
  MESSAGE_print(msg_ptr);
  #endif
  Wire.beginTransmission(device);
  Wire.write(msg_ptr->raw, MESSAGE_SIZEOF(msg_ptr)); // returns # of bytes written
  return Wire.endTransmission();                     // returns error // blocking function
}

static int __request_message(uint8_t msg_type, int device) {
  MESSAGE request;
  FORMAT_REQUEST(&request, msg_type);
  return __send_message(&request, device);
}

static int __get_message(MESSAGE* msg_ptr, uint8_t msg_type, int device) {
  int size = Wire.requestFrom(device, (int)MSGTYPE_SIZEOF(msg_type)); // returns # of bytes read // blocking function WBUFF_SIZE
  #ifdef VERBOSE_WIRE_DEBUG
  Serial_printf("Requested message; received %d bytes of data\n", size);
  #endif
  #ifdef DEBUG_MODE
  *msg_ptr = NULL_MESSAGE; // makes it easier to debug faulty messages
  #endif
  for (uint8_t* raw = msg_ptr->raw; Wire.available(); raw++)
    *raw = Wire.read();
  #ifdef DEBUG_MODE
  #ifdef VERBOSE_WIRE_DEBUG
  if (size > 0) {
    Serial_print("Message Received; ");
    MESSAGE_print(msg_ptr);
    Serial_println();
  }
  #endif
  #endif
  return 0;
}

static int wire_errorhandler(int error) {
  #if defined(VERBOSE_WIRE_DEBUG) || defined(WIRE_PRINT_ERRORS)
    wire_printerror(error);
  #endif
  return error;
  // this doesn't do anything, so returning early
  // to ensure this all gets optimized out
  // but still here in case I ever do anything with it
  switch (error) {
  case TRANS_SUCCESS:
    return 0;
  case TRANS_OVERFLOW:
  case TRANS_NACKADDRESS:
  case TRANS_NACKDATA:
  case TRANS_OTHER:
  case TRANS_TIMEOUT:
  default: break;
  };
  //Serial_printf("<Wire Error> %s\n", TRANS_ERR_string(error));
  return error;
}

int wire_printerror(int error) {
  if (error != TRANS_SUCCESS)
    Serial_printf("<Wire Error> %s\n", TRANS_ERR_string(error));
  return error;
}

#endif
#endif
