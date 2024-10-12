#include "header.h"
#ifdef TARGET_BOARD_CONTROL
#include <GyverHX711.h>
//#include <FastLED.h>

/* CONTROL TODO
 * -----------
 * Read Strain guage
 * Read potentiometer
 * Update Heart meter
 */

void process_message(MESSAGE*);
void process_request(MESSAGE*, uint8_t);
//void display_hearts();
void read_hardware();
void tare();

struct STRAIN_HANDLES {
  GyverHX711 right;
  GyverHX711 left;
} hstrains = {
  // HX_GAIN128_A, HX_GAIN32_B, HX_GAIN64_A
  GyverHX711(STRAIN_RIGHT_SCK_PIN, STRAIN_RIGHT_DT_PIN, HX_GAIN64_A),
  GyverHX711(STRAIN_LEFT_SCK_PIN, STRAIN_LEFT_DT_PIN, HX_GAIN64_A),
};

CONTROL_DATA controls;

static int count = 0;

void setup() {
  delay(2000); // power-up safety delay
  Serial_begin(9600);
  Serial_printf("\nControl Board %l Started\n", (int)(TARGET_BOARD - BOARD_ROBOT_2));
  wire_slaveInit(TARGET_BOARD);
  controls = {0};
}

void loop() {
  STARTFRAME();
  read_hardware();
  //display_hearts();
  if (count % 50/2 == 0) {
    //Serial_printf("strain: %d, %d\n", controls.strains.right, controls.strains.left);
  }
  WAITFRAME(FRAME_DELAY);
  count++;
}

void tare() {}

//long left;
void read_hardware() {
  //if (!(hstrains.left.available() && hstrains.right.available()))
  //  return;
  //uint8_t rightstrain = STRAIN_MAPIN(STRAIN_CALIBRATE_RIGHT(hstrains.right.read()));
  //uint8_t leftstrain = STRAIN_MAPIN(STRAIN_CALIBRATE_LEFT(hstrains.left.read()));
  //uint8_t rightstrain = STRAIN_CALIBRATE_RIGHT(hstrains.right.read());
  //uint8_t leftstrain = STRAIN_CALIBRATE_LEFT(hstrains.left.read());
  //controls.strains.right = MAX(rightstrain, controls.strains.right);
  //controls.strains.left = MAX(leftstrain, controls.strains.left);
  //left = MAX(left, STRAIN_MAPIN(STRAIN_CALIBRATE_LEFT(hstrains.left.read())));
  //controls.strains.right = rightstrain;
  //controls.strains.left = leftstrain;
  //controls.strains = {0};
  uint8_t leftstrain = STRAIN_MAPIN(STRAIN_CALIBRATE_LEFT(hstrains.left.read()));
  uint8_t rightstrain = STRAIN_MAPIN(STRAIN_CALIBRATE_RIGHT(hstrains.right.read()));
  controls.strains.left = MAX(leftstrain, controls.strains.left);
  controls.strains.right = MAX(rightstrain, controls.strains.right);
  controls.joystick = JOYSTICK_MAPIN(analogRead(POTENTIOMETER_PIN));
  //heartcount = (count >> 6) % (HEART_LED_COUNT+1);
  if (count % 10 == 0) {
    //Serial_printf("right:%d left:%d\n", rightstrain, leftstrain);
    Serial_print(STRAIN_CALIBRATE_RIGHT(hstrains.right.read()));
    Serial_print(", ");
    Serial_println(STRAIN_CALIBRATE_LEFT(hstrains.left.read()));
    //Serial_println(analogRead(POTENTIOMETER_PIN));
    /*
    Serial_print("right: ");
    Serial_print(STRAIN_CALIBRATE_RIGHT(hstrains.right.read())); //-(hstrains.right.read()+134000)*1100/100000);
    Serial_print(", left: ");
    Serial_println(STRAIN_CALIBRATE_LEFT(hstrains.left.read())); //(hstrains.left.read()+2357000)*1100/100000);//*/
    //Serial_printf("strain real: %d, %d\nstrain calib: %d, %d\nstrain final: %d, %d\n", hstrains.right.read(), hstrains.left.read(), 
    //STRAIN_CALIBRATE_RIGHT(hstrains.right.read()), STRAIN_CALIBRATE_LEFT(hstrains.left.read()),
    //controls.strains.right, controls.strains.left);
  }
  //if (count % 10 == 0)
  //  Serial_println(hstrains.left.read());
  //if (count % (1<<6) == 0)
  //  Serial_printf("hearts: %d\n", heartcount);
}

/*void display_hearts() {
  int i;
  for (i = 0; i < MIN(heartcount, HEART_LED_COUNT); i++) {
    leds[i] = HEART_COLOR;
  }
  for (; i < HEART_LED_COUNT; i++) {
    leds[i] = LED_OFF;
  }
  FastLED.show();
}*/

void process_message(MESSAGE* msg_in) {
  Serial_println("Message in:");
  MESSAGE_print(msg_in);
  switch (msg_in->type) {
    case MSG_HEARTS:
      //heartcount = msg_in->hearts;
      break;
  }
}

void process_request(MESSAGE* msg_out, uint8_t request) {
  Serial_println("Message out:");
  switch (request) {
    case MSG_CONTROL_DATA:
      msg_out->controls = controls;
      controls.strains = {0};
      break;
  }
  MESSAGE_print(msg_out);
}

#endif