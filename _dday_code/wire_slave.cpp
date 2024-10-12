#include "header.h"
#ifdef WIRE_MODE_SLAVE
#ifndef DISABLE_WIRE

void wire_slaveInit();
void __process_message(MESSAGE* msg_ptr);
void __process_request(MESSAGE* msg_ptr);
void __read_message(MESSAGE* msg_ptr);
void process_message(MESSAGE*);
void process_request(MESSAGE*, uint8_t);

MESSAGE MSGclaim;

void wire_slaveInit(int id) {
  #ifdef WIRE_TIMEOUT
  Wire.setWireTimeout(WIRE_TIMEOUT);
  #endif
  #ifdef WIRE_FREQUENCY
  Wire.setClock(WIRE_FREQUENCY);
  #endif
  Wire.begin(id);
  Wire.onReceive(wireReceiveEvent);
  Wire.onRequest(wireRequestEvent);
}

void wireReceiveEvent(int bytes) {
  //int error;
  MESSAGE msg;
  __read_message(&msg);
  #ifdef VERBOSE_WIRE_DEBUG
  Serial_print("Slave received message; ");
  MESSAGE_print(&msg);
  #endif
  __process_message(&msg);
}

void __read_message(MESSAGE* msg_ptr) {
  #ifdef DEBUG_MODE
  *msg_ptr = NULL_MESSAGE;
  #endif
  for (uint8_t* raw = msg_ptr->raw; Wire.available(); raw++)
    *raw = Wire.read();
}

// should I change this to return message type?
void __process_message(MESSAGE* msg_ptr) {
  process_message(msg_ptr);
  if (msg_ptr->type == MSG_REQUEST)
    __process_request(msg_ptr);
  /*switch (msg_ptr->type) {
  case MSG_ERROR: break;
  case MSG_NULL: break;
  case MSG_REQUEST:
    __process_request(msg_ptr);
    break;
  default: break;
  }*/
}

void __process_request(MESSAGE* msg_ptr) {
  process_request(&MSGclaim, msg_ptr->request);
  MSGclaim.type = msg_ptr->request;
  /*switch (msg_ptr->request) {
  case MSG_ERROR: break;
  case MSG_NULL: break;
  case MSG_REQUEST: break; // useful for slave requests to master
  default: break;
  }*/
}

void wireRequestEvent() {
  #ifdef VERBOSE_WIRE_DEBUG
  Serial_print("Slave Message Requested, ");
  Serial_print("Transmitting ");
  MESSAGE_print(&MSGclaim);
  Serial_println();
  #endif
  Wire.write(MSGclaim.raw, MESSAGE_SIZEOF(&MSGclaim));
  MSGclaim.type = MSG_NULL; // makes sure master only gets request once
}

#endif
#endif