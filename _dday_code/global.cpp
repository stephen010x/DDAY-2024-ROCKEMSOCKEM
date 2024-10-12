#include "header.h"

/*
SLAVELIST slave_list {
  BOARD_SLAVE1,
  BOARD_SLAVE2,
};*/

#ifdef DEBUG_MODE

void Serial_printf(char* format, ...) {
  char str[256];
  va_list args;
  va_start(args, format);
  vsprintf(str, format, args);
  va_end(args);
  Serial.print(str);
}

// presumably only for debugging use
const MESSAGE NULL_MESSAGE = {0};

// I am thinking that these aren't worth it. Just use stringify

char* MSGTYPE_string(int type) {
  switch (type) {
    case MSG_ERROR:         return "ERROR";
    case MSG_NULL:          return "NULL";
    case MSG_REQUEST:       return "REQUEST";
    case MSG_STATE_FLAGS:   return "STATE FLAGS";
    case MSG_SERVO_DATA:    return "SERVO DATA";
    case MSG_SERVO_PACKET:  return "SERVO PACKET";
    case MSG_JOYSTICK:      return "JOYSTICK";
    case MSG_CONTROL_DATA:  return "CONTROL DATA";
    case MSG_HEARTS:        return "HEARTS";
    default:                return "UNKNOWN";
  }
}

char* TRANS_ERR_string(int type) {
  switch (type) {
    case TRANS_SUCCESS:     return "SUCCESS";
    case TRANS_OVERFLOW:    return "OVERFLOW";
    case TRANS_NACKADDRESS: return "NACKADDRESS";
    case TRANS_NACKDATA:    return "NACKDATA";
    case TRANS_OTHER:       return "OTHER";
    case TRANS_TIMEOUT:     return "TIMEOUT";
    default:                return "UNKNOWN";
  }
}

char* BOARD_TYPE_string(int type) {
  switch (type) {
    case BOARD_MASTER:    return "MASTER";
    case BOARD_ROBOT:     return "ROBOTS";
    //case BOARD_ROBOT_1:   return "ROBOT 1";
    //case BOARD_ROBOT_2:   return "ROBOT 2";
    case BOARD_CONTROL_1: return "CONTROL 1";
    case BOARD_CONTROL_2: return "CONTROL 2";
    default:              return "UNKNOWN";
  }
}

// rename to Serial_printMESSAGE?
void MESSAGE_print(MESSAGE* msg_ptr) {
  Serial.print("MESSAGE: ");
  if (msg_ptr->type == MSG_ERROR) {
    Serial.println("ERROR");
    return;
  } else if (msg_ptr->type < MSGTYPE_COUNT)
    Serial.print(MSGTYPE_string(msg_ptr->type));
  else
    Serial_printf("UNKNOWN_TYPE_%u ", msg_ptr->type);
  Serial.print(" {");
  switch (msg_ptr->type) {
  case MSG_NULL: break;
  case MSG_REQUEST:
    if (msg_ptr->request < MSGTYPE_COUNT)
      Serial.print(MSGTYPE_string(msg_ptr->request));
    else
      Serial_printf("UNKNOWN_TYPE_%u", msg_ptr->request);
    break;
  default:
    for (int i = 0; i < MESSAGE_SIZEOF(msg_ptr)-1/*WBUFF_SIZE-2*/; i++) {
      Serial_printf("%x", msg_ptr->string_[i]);
      Serial.print(", ");
    }
    Serial_printf("%x", msg_ptr->string_[WBUFF_SIZE-2]);
    break;
  }
  Serial_printf("} (rawsize: %d)\n", MESSAGE_SIZEOF(msg_ptr));
}

#endif