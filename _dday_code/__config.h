#ifndef __CONFIG_H
#define __CONFIG_H

/*    BOARDS
 *    --------
 *    BOARD_MASTER
 *    BOARD_ROBOT
 *    BOARD_CONTROL_1
 *    BOARD_CONTROL_2
 */

#define TARGET_BOARD BOARD_MASTER
// DEBUG_MODE on Uno adds +11% of program, and
// +24% of dynamic memory since last test
//#define DEBUG_MODE
//#define VERBOSE_WIRE_DEBUG
//#define WIRE_PRINT_ERRORS
//#define RESET_SCOREBOARD
//#define NO_BLOCKING
//#define NO_WIFI
#define DOUBLE_LOGIN false
#define FRAME_FREQUENCY 10      // per second

#define DAMAGE_DIVIDER 20//50

//#define DISABLE_WIRE
#define WIRE_TIMEOUT 1000e3      // MICROSECONDS
#define WIRE_FREQUENCY 10e3      // Hz
// 10KHz slow mode (unsure if uno supports)
// 100KHz default
// 400KHz fast mode (max for uno, probably)

#define LED_TYPE WS2812B
#define COLOR_ORDER GRB         // GRB ordering is typical for WS2812B
#define BUILTIN_LED_PIN LED_BUILTIN
//#define EEPROM_BOARD BOARD_ROBOT_1

// add one to DAMAGE_DIVIDER to account for overkill points
#define MIN_SCORE (256*DAMAGE_DIVIDER)
#define MAX_SCORE (3*256*(DAMAGE_DIVIDER+1))
#define ADJUST_SCORE(n) MIN(MAX((n - MIN_SCORE) * 1000 / (MAX_SCORE - MIN_SCORE), 0), 1000)


/////////////////////////////
///// MASTER DEFINITIONS ///
///////////////////////////
#define AIO_USERNAME ""
#define AIO_KEY ""
#define WIFI_SSID ""
#define WIFI_PASS ""
#define DATA_RATE 30            // per minute

#define FEED_1ST_PLACE "1st-place"
#define FEED_1ST_SCORE "1st-player-score"
#define FEED_2ND_PLACE "2nd-place-name"
#define FEED_2ND_SCORE "2nd-place-score"
#define FEED_3RD_PLACE "3rd-place-name"
#define FEED_3RD_SCORE "3-player-score"
#define FEED_PLAYER1_CURRENT "current-Player-1"
#define FEED_PLAYER2_CURRENT "current-player-2"
#define FEED_PLAYER1_SCORE "score-player-1"
#define FEED_PLAYER2_SCORE "score-player-2"

#define LED_STRIP_PIN 7
#define LED_COUNT (150-31)

/*#define AUDIO_PIN_0 5
#define AUDIO_PIN_1 3
#define AUDIO_PIN_2 6
#define AUDIO_PIN_3 4
#define BUSY_PIN*/
//#define AUDIO_PIN_2 5
//#define AUDIO_PIN_3 6

#define AUDIO_PIN_0 4
#define AUDIO_PIN_1 5
#define AUDIO_PIN_2 6
#define AUDIO_PIN_3 13

//#define RFID_WRITE_TESTNAME // KEEP THIS DISABLED!!!
#define RFID_TESTNAME {"Sir", "R2D2"}
#define RFID_RST_PIN 9
#define RFID_SDA_PIN 10
#define RFID_FIRSTNAME_BLOCK 4
#define RFID_LASTNAME_BLOCK 1
#define RFID_RICK_BLOCK1 5
#define RFID_RICK_BLOCK2 6
#define RFID_RICK_LINK "https://youtu.be/dQw4w9WgXcQ"

#define HEART_LED_STRIP_1_PIN 11
#define HEART_LED_STRIP_2_PIN 12
#define HEART_LED_COUNT 5
#define HEART_COLOR HRGB(0xBF00FF)
//#define HEART_COLOR HRGB(0xFF1F1F)

/* RFID USED PINS (ADAFRUIT M4)
 * --------------
 * 24 (MISO), 26 (MOSI), and 25 (SCK)
*/


////////////////////////////
///// ROBOT DEFINITIONS ///
//////////////////////////
#define SERVO_RIGHT_1_PIN 4
#define SERVO_LEFT_1_PIN 5
#define SERVO_RIGHT_2_PIN 2
#define SERVO_LEFT_2_PIN 3

#define SERVO_ARMS_MAPIN(n)  ((int8_t)MAPCAP(n, 0, 45, 0x00, 0xFF))
#define SERVO_ARMS_MAPOUT(n) ((int)MAPCAP(n, 0x00, 0xFF, 0, 45))

//#define SERVO_ARMS_SPEED 10   // Hex degrees per frame

//////////////////////////////
///// CONTROL DEFINITIONS ///
////////////////////////////
#define POTENTIOMETER_PIN A1

#define STRAIN_1_LEFT_SCK_PIN 8
#define STRAIN_1_LEFT_DT_PIN 7
#define STRAIN_1_RIGHT_SCK_PIN 5
#define STRAIN_1_RIGHT_DT_PIN 6

#define STRAIN_2_LEFT_SCK_PIN 7
#define STRAIN_2_LEFT_DT_PIN 8
#define STRAIN_2_RIGHT_SCK_PIN 5
#define STRAIN_2_RIGHT_DT_PIN 6

/*#define STRAIN_1_CALIBRATE_RIGHT(n) (-((float)(n)+134000)*1100/100000)
#define STRAIN_1_CALIBRATE_LEFT(n) (((float)(n)+2357000)*1100/100000)
#define STRAIN_2_CALIBRATE_RIGHT(n) ((float)(n)*(-0.0309) - 23862)
#define STRAIN_2_CALIBRATE_LEFT(n) (((float)(n)*(-0.0076) - 3005+67)*5)*/

#define STRAIN_1_CALIBRATE_RIGHT(n) ((float)(n)*(-0.0468)-6370)
#define STRAIN_1_CALIBRATE_LEFT(n) ((float)(n)*(0.0439)+103414)
#define STRAIN_2_CALIBRATE_RIGHT(n) ((float)(n)*(-0.0435)-33420)
#define STRAIN_2_CALIBRATE_LEFT(n) ((float)(n)*(-0.0434)-16838)

#define JOYSTICK_MAPIN(n) ((int8_t)MAPCAP(n, PLACEHOLDER, PLACEHOLDER,-0x80, 0x7F))
// have our range for strain map to the smallest threshold for a registered punch
#define STRAIN_MAPIN(n)   ((uint8_t)MAPCAP(n, 500, 4000, 0x00, 0xFF))
//#define STRAIN_MAPIN(n)   ((uint8_t)MAPCAP(n, 200, 4000, 0x00, 0xFF))

#endif
