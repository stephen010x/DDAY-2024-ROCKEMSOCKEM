#include "header.h"
#ifdef TARGET_BOARD_ROBOT
#include <Servo.h>

/* ROBOT TODO
 * -----------
 * Move Servos
 * Read Servos
 */

void waitstate();
void gamestate();
void endstate();
void process_message(MESSAGE*);
void process_request(MESSAGE*, uint8_t);
void write_hardware();
//void predict_hardware();

struct SLAVE_STATE {
  unsigned long frame;
  STATE_FLAGS flags;
} state;

struct SERVO_HANDLES {
  Servo right;
  Servo left;
  //Servo legs;
};

SERVO_HANDLES handles[2];
//SERVO_PACKET real[2];
SERVO_PACKET write;

/*struct {
  SERVO_HANDLES handles[2];
  SERVO_DATA real;
  SERVO_DATA write;
} servos;*/

int8_t joystick;

static void(*state_functions[3])() = {waitstate, gamestate, endstate};

void setup() {
  delay(2000); // power-up safety delay
  Serial_begin(9600);
  Serial_printf("\nRobot Board %d Started\n", (int)(TARGET_BOARD - BOARD_MASTER));
  wire_slaveInit(TARGET_BOARD);
  handles[0].right.attach(SERVO_RIGHT_1_PIN);
  handles[0].left.attach(SERVO_LEFT_1_PIN);
  handles[1].right.attach(SERVO_RIGHT_2_PIN);
  handles[1].left.attach(SERVO_LEFT_2_PIN);
  for (int i = 0; i < 2; i++) {
    write.servos[i].left = 0;
    write.servos[i].right = 0;
  }
}

unsigned long frame = 0;
void loop() {
  STARTFRAME();
  // jumps to current game state every loop
  state_functions[state.flags.state]();
  WAITFRAME(FRAME_DELAY);
  frame++;
}

void waitstate() {
  //predict_hardware();
  write_hardware();
}

void gamestate() {
  //predict_hardware();
  // joystick stuff here
  write_hardware();
}

void endstate() {}

void write_hardware() {
  handles[0].right.write(SERVO_ARMS_MAPOUT(write.servos[0].right));
  handles[0].left.write(180-SERVO_ARMS_MAPOUT(write.servos[0].left));
  handles[1].right.write(180-SERVO_ARMS_MAPOUT(write.servos[1].right));
  handles[1].left.write(SERVO_ARMS_MAPOUT(write.servos[1].left));
}

/*
void predict_hardware() {
  int diff;
  diff = (int)servos.write.right - (int)servos.real.right;
  if (ABS(diff) > SERVO_ARMS_SPEED) {
    servos.real.right += SERVO_ARMS_SPEED * SIGN(diff);
  } else {
    servos.real.right = servos.write.right;
  }
  diff = (int)servos.write.left - (int)servos.real.left;
  if (ABS(diff) > SERVO_ARMS_SPEED) {
    servos.real.left += SERVO_ARMS_SPEED * SIGN(diff);
  } else {
    servos.real.left = servos.write.left;
  }
  diff = (int)servos.write.legs - (int)servos.real.legs;
  if (ABS(diff) > SERVO_ARMS_SPEED) {
    servos.real.legs += SERVO_ARMS_SPEED * SIGN(diff);
  } else {
    servos.real.legs = servos.write.legs;
  }
  if (frame % 60 == 0)
    Serial_printf("Servos right: %d, left: %d\n", servos.real.right, servos.real.left);
}
*/

/*
void read_hardware() {
  servos.real.right = SERVO_ARMS_MAPIN(servos.handles.right.real());
  servos.real.left  = SERVO_ARMS_MAPIN(servos.handles.left.real());
  servos.real.legs  = SERVO_LEGS_MAPIN(servos.handles.legs.rea());
}
*/

void process_message(MESSAGE* msg_in) {
  switch (msg_in->type) {
    case MSG_STATE_FLAGS:
      state.flags = msg_in->flags;
      break;
    case MSG_SERVO_PACKET:
      write = msg_in->servopack;
      break;
    case MSG_JOYSTICK: // probably won't use this
      joystick = msg_in->joystick;
      break;
  }
}

void process_request(MESSAGE* msg_out, uint8_t request) {
  switch (request) {
    case MSG_SERVO_DATA:
      //msg_out->servos = servos.real;
      break;
  }
}

#endif