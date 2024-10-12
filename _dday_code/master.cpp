#include "header.h"
#ifdef TARGET_BOARD_MASTER
#define USE_AIRLIFT
#include <FastLED.h>
#include <AdafruitIO_WiFi.h>
#include <arduino-timer.h>

/* MASTER TODO
 * -----------
 * Determine hits
 * Accumulate hit scores
 * Light LEDs
 * Play music
 * RFID
 * Update Dashboard (wifi)
 */

// IMPORTANT NOTE!
// Indicate player side by robot action
// Winner raises arms

// NOTE:
// Music files must follow this naming convention:
// 00001.mp3 ~ 00255.mp3

#define CYCLE_PERIOD 5000  // milliseconds
//#define HUE_OFF_STEP (1<<16 * 1000 / CYCLE_PERIOD / FRAME_FREQUENCY)
//#define HUE_LED_STEP (1<<16 / LED_COUNT)
#define HUE_OFF_STEP 8000
#define HUE_LED_STEP 500
#define WIFI_DELAY 60*1000/DATA_RATE    // in milliseconds

void waitstate_setup();
void waitstate_loop();
void gamestate_setup();
void gamestate_loop();
void endstate();
bool frame(void*);
void reset_game();
void set_gamestate(uint8_t newstate);
void game_get_data();
void game_send_data();
void set_led_mode(int id);
void update_leds();
void leds_reset();
void login_check();
void play_song(uint8_t songid);
void read_scoreboard();
void update_scoreboard();
void playerlogic(int i);
void print_scoreboard();
void update_hearts();
void AIO_handle_FEED_highplayer_1(AdafruitIO_Data *data);
void AIO_handle_FEED_highplayer_2(AdafruitIO_Data *data);
void AIO_handle_FEED_highplayer_3(AdafruitIO_Data *data);
void AIO_handle_FEED_highscore_1(AdafruitIO_Data *data);
void AIO_handle_FEED_highscore_2(AdafruitIO_Data *data);
void AIO_handle_FEED_highscore_3(AdafruitIO_Data *data);

AdafruitIO_WiFi Aio(AIO_USERNAME, AIO_KEY, WIFI_SSID, WIFI_PASS, SPIWIFI_SS, SPIWIFI_ACK, SPIWIFI_RESET, NINA_GPIO0, &SPI);
AdafruitIO_Feed* FEED_highplayer[3] = {
  Aio.feed(FEED_1ST_PLACE),
  Aio.feed(FEED_2ND_PLACE),
  Aio.feed(FEED_3RD_PLACE),
};
AdafruitIO_Feed* FEED_highscore[3] {
  Aio.feed(FEED_1ST_SCORE),
  Aio.feed(FEED_2ND_SCORE),
  Aio.feed(FEED_3RD_SCORE),
};
AdafruitIO_Feed* FEED_player[2] {
  Aio.feed(FEED_PLAYER1_CURRENT),
  Aio.feed(FEED_PLAYER2_CURRENT),
};
AdafruitIO_Feed* FEED_score[2] {
  Aio.feed(FEED_PLAYER1_SCORE),
  Aio.feed(FEED_PLAYER2_SCORE),
};

Timer<8> timer;
uint8_t random_hue;

bool scoreboard_change[3];

enum PLAYER_STATES {
  PLAYER_NULL = 0,
  PLAYER_LOSE,
  PLAYER_WIN
};

// needed separate for formatting
union PLAYER_NAME {
  struct {
    char first[16];
    char last[16];
  };
  char full[32];
};

// Needs it's own struct for scoreboard
struct PLAYER_DATA {
  uint16_t score;
  PLAYER_NAME name;
} scoreboard[3];

struct PLAYER {
  struct {
    uint8_t state: 6; // probably not even used
    uint8_t enabled: 1;
    uint8_t logged_in: 1;
  };
  int health;
  PLAYER_DATA data;
} players[2];

struct MASTER_STATE {
  unsigned long frame;
  STATE_FLAGS flags;
} state;

//SERVO_DATA servos[2];
/*struct {
  SERVO_DATA read;
  SERVO_DATA write;
} servos[2];*/

//SERVO_PACKET read;
SERVO_PACKET write;

CONTROL_DATA controls[2];

union {
  char whole[32];
  char parts[2][16];
} static const ricklink = {RFID_RICK_LINK};

CRGB leds[LED_COUNT];

uint8_t keyA[6];
uint8_t keyB[6];

unsigned long led_frame;
int led_mode;

int feed_returns;

enum LED_MODE {
  LEDS_OFF = 0,
  LEDS_RAINBOW_SPIN,
  LEDS_RAINBOW,
  LEDS_DASHED_SPIN,
  LEDS_DASHED,
  LEDS_BLINKING_GREEN,
  LEDS_RANDOM_SOLID,
  //LEDS_BLINK_TWICE,
};

enum SONG_ID {
  //SONG_NONE = 0,
  SONG_1,
  SONG_2,
  SONG_3,
  SONG_4,
};

CRGB hearts[2][HEART_LED_COUNT];

void setup() {
  ///* DEBUG */ Wire.setClock(10000);
  delay(2000); // power-up safety delay
  randomSeed(micros());
  Serial_begin(9600);
  Serial_println("\nMaster Board Started");
  RFID_init();
  wire_masterInit();
  for (byte i = 0; i < LENGTHOF(keyA); i++) keyA[i] = 0xFF;
  for (byte i = 0; i < LENGTHOF(keyA); i++) keyB[i] = 0x00;
  FastLED.addLeds<LED_TYPE, LED_STRIP_PIN, COLOR_ORDER>(leds, LED_COUNT);
  FastLED.addLeds<LED_TYPE, HEART_LED_STRIP_1_PIN, COLOR_ORDER>(hearts[0], HEART_LED_COUNT);
  FastLED.addLeds<LED_TYPE, HEART_LED_STRIP_2_PIN, COLOR_ORDER>(hearts[1], HEART_LED_COUNT);
  // set audio pins
  pinMode(AUDIO_PIN_0, OUTPUT);
  pinMode(AUDIO_PIN_1, OUTPUT);
  pinMode(AUDIO_PIN_2, OUTPUT);
  /*play_song(SONG_1);
  delay(5000);
  play_song(SONG_2);
  delay(5000);
  play_song(SONG_3);
  delay(5000);
  play_song(SONG_4);
  delay(5000);*/
  //* setup Aio handlers
  FEED_highplayer[0]->onMessage(AIO_handle_FEED_highplayer_1);
  FEED_highplayer[1]->onMessage(AIO_handle_FEED_highplayer_2);
  FEED_highplayer[2]->onMessage(AIO_handle_FEED_highplayer_3);
  FEED_highscore[0]->onMessage(AIO_handle_FEED_highscore_1);
  FEED_highscore[1]->onMessage(AIO_handle_FEED_highscore_2);
  FEED_highscore[2]->onMessage(AIO_handle_FEED_highscore_3);
  //*/////////////
  Serial_printf("HUE_OFF_STEP %d\n", HUE_OFF_STEP);
  Serial_printf("HUE_LED_STEP %d\n", HUE_LED_STEP);
  Serial_printf("FRAME_DELAY %d\n", FRAME_DELAY);
  // reset game
  reset_game();
  // connect to wifi
  Serial_println("Connecting to Adafruit IO...");
  #ifndef NO_WIFI
  Aio.connect();
  //#ifndef NO_BLOCKING
  for (int i = 0; Aio.status() < AIO_CONNECTED; i++) {
    if (i % 10 == 0) {
      Serial_println("Re-attempting connection...");
      Aio.connect();
    }
    delay(200);
  }
  //#endif
  #endif
  Serial_printf("\n%s\n", Aio.statusText());
  // restore scoreboard
  scoreboard[0] = (PLAYER_DATA){0, ". . ."};
  scoreboard[1] = (PLAYER_DATA){0, ". . ."};
  scoreboard[2] = (PLAYER_DATA){0, ". . ."};
  #ifdef RESET_SCOREBOARD
  update_scoreboard();
  #endif
  read_scoreboard();
  timer.every(FRAME_DELAY, frame);
}

void (*statefuncts[])() {waitstate_setup, waitstate_loop, gamestate_setup, gamestate_loop, endstate};

void loop() {
  timer.tick();
}

bool frame(void* _) {
  statefuncts[state.flags.state]();
  return true;
}

void waitstate_setup() {
  random_hue = random(0x00, 0xFF);
  Serial_println("\nStarting wait state...");
  state.frame = 0;
  play_song(SONG_3);
  set_led_mode(LEDS_RANDOM_SOLID);
  leds_reset();
  set_gamestate(STATE_WAIT_LOOP);
}

void waitstate_loop() {
  //STARTFRAME();
  #ifndef NO_WIFI
  Aio.run();
  #endif
  update_leds();
  login_check();
  state.frame++;
  //Serial_print(".");
  //WAITFRAME(FRAME_DELAY);
  if (players[0].logged_in && players[1].logged_in) {
    set_gamestate(STATE_GAME_SETUP);
  }
}

void gamestate_setup() {
  delay(5000);
  Serial_println("\nStarting game state...");
  state.frame = 0;
  //while (true) {
  //set_gamestate(STATE_GAME);
  //}
  ///* DEBUG */ print_scoreboard();
  play_song(SONG_2);
  set_led_mode(LEDS_DASHED_SPIN);
  game_get_data();
  controls[0] = {0};
  controls[1] = {0};
  set_gamestate(STATE_GAME_LOOP);
}

void gamestate_loop() {
    //STARTFRAME();
    #ifndef NO_WIFI
    Aio.run();
    #endif
    update_leds();
    game_get_data();
    for (int i = 0; i < LENGTHOF(players); i++) {
      playerlogic((i + state.frame) % LENGTHOF(players));
    }
    game_send_data();
    if (players[0].state == PLAYER_WIN || players[1].state == PLAYER_WIN) {
      delay(500);
      set_gamestate(STATE_END);
    }
    update_hearts();
    state.frame++;
    //WAITFRAME(FRAME_DELAY);
}

bool punchact[2] = {false, false};
bool punchend(void* i) {
  punchact[(int)i] = false;
  return false;
}

void playerlogic(int i) {
  //static int counter[2] = 0;
  static STRAIN_DATA laststrains[2] = {1};
  //timer.tick();
  int j = (i+1) % 2;
  if (state.frame % (50/5) == 0) {
    Serial_printf("strain %d: %d, %d\n", i+1, controls[i].strains.right, controls[i].strains.left);
  }

  //Serial_printf("P%d health: %d\n", j+1, players[j].health);

  if (punchact[i] == false) {
    //uint8_t leftstrain = (laststrains[i].left == 0) ? controls[i].strains.left : 0;
    //uint8_t rightstrain = (laststrains[i].right == 0) ? controls[i].strains.right : 0;

    uint8_t leftstrain = controls[i].strains.left;
    uint8_t rightstrain = controls[i].strains.right;

    if (leftstrain || rightstrain) {
      punchact[i] = true;
      uint8_t maxstrain = MAX(leftstrain, rightstrain);
      timer.in(MAX((int)maxstrain-80,0), punchend, *(void**)&i);
      //timer.in(5000, punchend, *(void**)&i);
      players[j].health -= maxstrain / DAMAGE_DIVIDER;
      players[i].data.score += (int)maxstrain*2;
      players[j].data.score += (int)maxstrain;
      Serial_printf("player %d health %d\n", j+1, players[j].health);
      Serial_printf("p%d score: %d (%d)\n", i+1, ADJUST_SCORE(players[i].data.score), players[i].data.score);
      Serial_printf("p%d score: %d (%d)\n", j+1, ADJUST_SCORE(players[j].data.score), players[j].data.score);
    }
    if (players[j].health <= 0 /*&& read.servos[i].right == 0 && read.servos[i].left == 0*/) {
      players[i].state = PLAYER_WIN;
      Serial_printf("Player %d won!\n", i+1);
    }
    
    write.servos[i].right = rightstrain ? 255 : 0; //(int)leftstrain * 2;
    write.servos[i].left = leftstrain ? 255 : 0; //(int)rightstrain * 2;
    //Serial_println();

    laststrains[i] = controls[i].strains;
  }
}

void update_hearts() {
  for (int i = 0; i < 2; i++) {
    int health = MAX(players[i].health, 0);
    int full = health*HEART_LED_COUNT/256;
    int rem = health*HEART_LED_COUNT%256;
    //Serial_printf("full: %d, rem: %d\n", full, rem);
    //Serial_printf("health: %d\n", health);
    for (int j = 0; j < HEART_LED_COUNT; j++) {
      if (j < full)
        hearts[i][HEART_LED_COUNT-j-1] = HEART_COLOR;
      else
        hearts[i][HEART_LED_COUNT-j-1] = HRGB(0x000000);
      //Serial_printf("(%d,%d,%d) ", hearts[i][j].r, hearts[i][j].g, hearts[i][j].b);
    }
    //Serial_println();
    CRGB dim = HEART_COLOR;
    dim.r = (int)dim.r*rem/256;
    dim.g = (int)dim.g*rem/256;
    dim.b = (int)dim.b*rem/256;
    hearts[i][HEART_LED_COUNT-full-1] = dim;
  }
  FastLED.show();
}

void endstate() {
  set_led_mode(LEDS_BLINKING_GREEN);
  update_leds();
  Serial_println("\nStarting end state...");
  play_song(SONG_1);
  //leds_reset();
  update_hearts();
  Serial_println("Old Scoreboard:");
  print_scoreboard();
  // stuff here
  update_scoreboard();
  #ifdef NO_WIFI
    delay(5000);
  #endif
  update_hearts();
  reset_game();
  update_hearts();
}

void reset_game() {
  MESSAGE msg;
  //play_song(0);
  feed_returns = 0;
  players[0] = {0};
  players[1] = {0};
  players[0].health = 256;
  players[1].health = 256;
  set_gamestate(STATE_WAIT_SETUP);
  FORMAT_MESSAGE(&msg, MSG_SERVO_PACKET);
  msg.servopack = {0};
  send_message(&msg, BOARD_ROBOT);
  set_led_mode(LEDS_OFF);
  update_hearts();
  //FORMAT_MESSAGE(&msg, MSG_HEARTS);
  //msg.hearts = 5;
  //wire_printerror(send_message(&msg, BOARD_CONTROL_1)); // DEBUG
  //send_message(&msg, BOARD_CONTROL_1); // NO DEBUG
  //send_message(&msg, BOARD_CONTROL_2);
  Serial_println("Game reset");
}

void game_get_data() {  
  MESSAGE msg;
  // request servo data from robots
  /*if (pull_message(&msg, MSG_SERVO_PACKET, BOARD_ROBOT) == NO_ERROR)
    read.servos = msg.servopack;*/
  // request strain gauge and joystick data from controls
  if (pull_message(&msg, MSG_CONTROL_DATA, BOARD_CONTROL_1) == NO_ERROR)
    controls[0] = msg.controls;
  if (pull_message(&msg, MSG_CONTROL_DATA, BOARD_CONTROL_2) == NO_ERROR)
    controls[1] = msg.controls;
  //* DEBUG */if (pull_message(&msg, MSG_CONTROL_DATA, BOARD_CONTROL_1) == NO_ERROR)
  //* DEBUG */  controls[0] = msg.controls;
  //* DEBUG */if (pull_message(&msg, MSG_SERVO_DATA, BOARD_ROBOT_1) == NO_ERROR)
  //* DEBUG */  servos[0] = msg.servos;
  //FORMAT_REQUEST(&msg, MSG_SERVO_DATA);
  //float start = micros()/1000.0;
  //send_message(&msg, BOARD_ROBOT_1);
  //float end = micros()/1000.0;
  //Serial_printf("time: %.1f\n", end-start);
}

void game_send_data() {
  MESSAGE msg;
  // send joystick data to robot boards
  /*FORMAT_MESSAGE(&msg, MSG_JOYSTICK);
  msg.joystick = controls[0].joystick;
  send_message(&msg, BOARD_ROBOT_1);
  FORMAT_MESSAGE(&msg, MSG_JOYSTICK);
  msg.joystick = controls[1].joystick;
  send_message(&msg, BOARD_ROBOT_2);*/
  FORMAT_MESSAGE(&msg, MSG_SERVO_PACKET);
  msg.servopack = write;
  send_message(&msg, BOARD_ROBOT);
}

void set_gamestate(uint8_t newstate) {
  //MESSAGE msg;
  state.flags.state = newstate;
  //FORMAT_MESSAGE(&msg, MSG_STATE_FLAGS);
  //msg.flags = state.flags;
  Serial_printf("Changing gamestate to mode %d\n", newstate);
  // Needs to guarentee that state change is sent
  // before continuing game!
  /*#ifndef NO_BLOCKING
  while (send_message(&msg, BOARD_ROBOT))
    delay(10);
  #else
  wire_printerror(send_message(&msg, BOARD_ROBOT));
  #endif*/
  //Serial_println("Gamemode change successful");
}

void leds_rainbow_spin(unsigned long frame) {
  CRGB rgb;
  CHSV hsv = HHSV(0x00FFFF);
  //CHSV hsv = HHSV(0x008888);
  //CHSV hsv = HHSV(0x006622);
  uint16_t hue = frame * HUE_OFF_STEP;
  //if (state.frame % 10 == 0)
  //  Serial_printf("hue %0x\n", hue);
  for (int i = 0; i < LED_COUNT; i++) {
    hsv.hue = hue >> 8;
    hsv2rgb_rainbow(hsv, rgb); // reference struct-out abuse!
    leds[i] = rgb;
    hue += HUE_LED_STEP;
  }
  FastLED.show();
}

void leds_dashed_spin(unsigned long frame) {
  CRGB rgb;
  CHSV hsv = HHSV(0x00FFFF);
  hsv.hue = random_hue + ((frame * HUE_OFF_STEP/20)>>8);
  hsv2rgb_rainbow(hsv, rgb); // reference struct-out abuse!
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i] = ((i + frame*5) % 20 > 10) ? rgb : LED_OFF;
  }
  FastLED.show();
}

void leds_blinking_green(unsigned long frame) {
  CRGB color = ((frame % 4) < 2) ? HRGB(0x11FF11) : LED_OFF;
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i] = color;
  }
  FastLED.show();
}

void leds_random_solid(unsigned long frame) {
  CRGB rgb;
  CHSV hsv = HHSV(0x00FFFF);
  hsv.hue = random_hue;
  hsv2rgb_rainbow(hsv, rgb);
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i] = rgb;
  }
  FastLED.show();
}

void set_led_mode(int id) {
  led_mode = id;
  led_frame = 0;
  switch (id) {
    case LEDS_OFF:
      leds_reset();
      break;
  }
}

void update_leds() {
  switch (led_mode) {
    case LEDS_RAINBOW_SPIN:
      leds_rainbow_spin(led_frame);
      break;
    case LEDS_RAINBOW:
      leds_rainbow_spin(0);
      break;
    case LEDS_DASHED_SPIN:
      leds_dashed_spin(led_frame);
      break;
    case LEDS_DASHED:
      leds_dashed_spin(0);
      break;
    case LEDS_BLINKING_GREEN:
      leds_blinking_green(led_frame);
      break;
    case LEDS_RANDOM_SOLID:
      leds_random_solid(0);
      break;
  }
  led_frame++;
}

void leds_reset() {
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i] = LED_OFF;
  }
  FastLED.show();
}

void login_check_layer() {
  //Serial_print(".");
  //static uint8_t login_counter = 0;
  PLAYER_NAME name;
  //login_counter &= 0b1;
  int playerindex = players[0].logged_in;
  PLAYER* player_ptr = &players[playerindex];
  int error = RFID_scancard();
  //Serial_println(error);
  if (error)
    return;
  #ifdef RFID_WRITE_TESTNAME
  char testname[2][16] = RFID_TESTNAME;
  RFID_writeblock_keyA(testname[0], &keyA, RFID_FIRSTNAME_BLOCK, 16);
  RFID_writeblock_keyA(testname[1], &keyA, RFID_LASTNAME_BLOCK, 16);
  #endif
  // read first name
  if (RFID_readblock_keyA(name.last, &keyA, RFID_LASTNAME_BLOCK, 16))
    return;
  // read last name
  if (RFID_readblock_keyA(name.first, &keyA, RFID_FIRSTNAME_BLOCK, 16))
    return;
  // write rick blocks
  RFID_writeblock_keyA(ricklink.parts[0], &keyA, RFID_RICK_BLOCK1, 16);
  RFID_writeblock_keyA(ricklink.parts[1], &keyA, RFID_RICK_BLOCK2, 16);
  // format name
  for (int i = 0; i < LENGTHOF(name.full); i++)
    if (name.full[i] == ' ')
      name.full[i] = '\0';
  snprintf(player_ptr->data.name.full, 32, "%s %s", name.first, name.last);
  player_ptr->data.name.full[31] = '\0';
  //Serial_printf("same? %s ,%s, %s\n", players[0].data.name.full, players[1].data.name.full, strncmp(players[0].data.name.full, players[1].data.name.full, 32) ? "true" : "false");
  if (!players[0].logged_in || strncmp(players[0].data.name.full, players[1].data.name.full, 32) != 0 || DOUBLE_LOGIN) {
    player_ptr->logged_in = true;
    Serial_printf("Player \"%s\" logged in as Player %d\n", player_ptr->data.name.full, playerindex+1);
    MESSAGE msg = {0};
    FORMAT_MESSAGE(&msg, MSG_SERVO_PACKET);
    memset(&msg.servopack.servos[playerindex], 0xFF, sizeof(msg.servopack.servos[playerindex]));
    #ifdef NO_BLOCKING
      send_message(&msg, BOARD_ROBOT);
    #else 
      while (send_message(&msg, BOARD_ROBOT));
    #endif
  } else {
    Serial_printf("Duplicate player detected");
  }
}

void login_check() {
  //Serial_println("doing login checks");
  login_check_layer();
  // defer end card read
  RFID_closescan();
  //Serial_println("ending login checks");
}

#define MUSIC_MODE 2
void play_song(uint8_t songid) {
  #if MUSIC_MODE == 2
  digitalWrite(AUDIO_PIN_0, HIGH);
  digitalWrite(AUDIO_PIN_1, HIGH);
  digitalWrite(AUDIO_PIN_2, HIGH);
  #endif
  // combination mode 1: 0:0:1
  switch (songid) {
    #if MUSIC_MODE == 1
      case 1:
        digitalWrite(AUDIO_PIN_0, )
        break;
      case 0:
      default:
        digitalWrite(AUDIO_PIN_0, )
        break;
    #elif MUSIC_MODE == 2
      case SONG_1: // verified
        digitalWrite(AUDIO_PIN_0, LOW);
        delay(10);
        //digitalWrite(AUDIO_PIN_0, HIGH);
        break;
      case SONG_2: // verfied
        digitalWrite(AUDIO_PIN_1, LOW);
        delay(10);
        //digitalWrite(AUDIO_PIN_1, HIGH);
        break;
      case SONG_3:
        digitalWrite(AUDIO_PIN_2, LOW);
        delay(10);
        //digitalWrite(AUDIO_PIN_2, HIGH);
      case SONG_4:
        digitalWrite(AUDIO_PIN_3, LOW);
        delay(10);
        //digitalWrite(AUDIO_PIN_3, HIGH);
        break;
      default:
        break;
    #endif
  }
}

void read_scoreboard() {
  // non-blocking functions
  // doesn't matter when we get them
  #if !defined(NO_WIFI) && !defined(RESET_SCOREBOARD)
  for (int i = 0; i < LENGTHOF(FEED_highscore); i++) {
    FEED_highplayer[i]->get();
    FEED_highscore[i]->get();
  }
  delay(WIFI_DELAY*6);
  //while (feed_returns < 6)
  //  delay(10); // this is really stupid that this doesn't work
  /*#ifdef DEBUG_MODE
  delay(5000);
  for (int i = 0; i < LENGTHOF(scoreboard); i++) {
    Serial_printf("%dst place: %s\n\tscore: %d\n", i+1, scoreboard[i].name.full, scoreboard[i].score);
  }
  #endif*/
  #endif
}

void update_scoreboard() {
  PLAYER_DATA playerbuff[2] = {
    players[0].data,
    players[1].data,
  };
  playerbuff[0].score = ADJUST_SCORE(playerbuff[0].score);
  playerbuff[1].score = ADJUST_SCORE(playerbuff[1].score);
  // update local scoreboard
  // 0 is 1st place, 2 is 3rd place
  for (int i = 0; i < LENGTHOF(playerbuff); i++)
    for (int j = 0; j < LENGTHOF(scoreboard); j++)
      if (playerbuff[i].score > scoreboard[j].score) {
        XCHG(playerbuff[i], scoreboard[j]);
        scoreboard_change[j] = true;
      } else {
        scoreboard_change[j] = false;
      }
  // send scoreboard to feeds
  #ifndef NO_WIFI
  for (int i = 0; i < LENGTHOF(FEED_highplayer); i++) {
    //if (scoreboard_change[i]) {
      FEED_highplayer[i]->save(scoreboard[i].name.full);
      FEED_highscore[i]->save(scoreboard[i].score);
      
    //}
  }
  for (int i = 0; i < LENGTHOF(FEED_player); i++) {
    FEED_player[i]->save(players[i].data.name.full);
    FEED_score[i]->save(ADJUST_SCORE(players[i].data.score));
  }
  delay(WIFI_DELAY*10);
  #endif
  Serial_println("New Scoreboard:");
  print_scoreboard();
}

void print_scoreboard() {
  #ifdef DEBUG_MODE
  Serial_println("Scoreboard updated");
  for (int i = 0; i < LENGTHOF(scoreboard); i++) {
    Serial_printf("%dst place: %s\n\tscore: %d\n", i+1, scoreboard[i].name.full, scoreboard[i].score);
  }
  #endif
}

void AIO_handle_FEED_highplayer(AdafruitIO_Data *data, int index) {
  strncpy(scoreboard[index].name.full, data->value(), 32);
  Serial_printf("%dst place name received: %s\n", index+1, scoreboard[index].name.full);
  feed_returns++;
}
void AIO_handle_FEED_highscore(AdafruitIO_Data *data, int index) {
  scoreboard[index].score = atoi(data->value());
  Serial_printf("%dst place score received: %d\n", index+1, scoreboard[index].score);
  feed_returns++;
}
void AIO_handle_FEED_highplayer_1(AdafruitIO_Data *data) {AIO_handle_FEED_highplayer(data, 0);}
void AIO_handle_FEED_highplayer_2(AdafruitIO_Data *data) {AIO_handle_FEED_highplayer(data, 1);}
void AIO_handle_FEED_highplayer_3(AdafruitIO_Data *data) {AIO_handle_FEED_highplayer(data, 2);}
void AIO_handle_FEED_highscore_1(AdafruitIO_Data *data) {AIO_handle_FEED_highscore(data, 0);}
void AIO_handle_FEED_highscore_2(AdafruitIO_Data *data) {AIO_handle_FEED_highscore(data, 1);}
void AIO_handle_FEED_highscore_3(AdafruitIO_Data *data) {AIO_handle_FEED_highscore(data, 2);}

#endif