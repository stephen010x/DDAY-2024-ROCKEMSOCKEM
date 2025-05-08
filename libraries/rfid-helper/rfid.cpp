#include "rfid_helper.h"
#ifdef RFID_ENABLE
#include <SPI.h>
#include <MFRC522.h>

MFRC522 rfid(RFID_SDA_PIN, RFID_RST_PIN);

void RFID_init();
int8_t RFID_scancard();
int8_t RFID_closescan();
int8_t RFID_readblock_keyA(void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len);
int8_t RFID_readblock_keyB(void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len);
int8_t _RFID_readblock(void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len, uint8_t keytype);
int8_t RFID_writeblock_keyA(const void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len);
int8_t RFID_writeblock_keyB(const void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len);
int8_t _RFID_writeblock(const void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len, uint8_t keytype);

void RFID_init() {
  SPI.begin();
  rfid.PCD_Init();
}

int8_t RFID_scancard() {
  // Check if new card present on the sensor/reader.
  if (!rfid.PICC_IsNewCardPresent())
      return -1;
  // Select one of the cards
  if (!rfid.PICC_ReadCardSerial())
      return -2;
  #ifdef DEBUG_MODE
  Serial_println("\n...Card Detected...");
  // print UID
  Serial_print("Card UID:");
  for (byte i = 0; i < rfid.uid.size; i++)
    Serial_printf("%0x ", rfid.uid.uidByte[i]);
  // print SAK id
  Serial_printf("CARD SAK: %uhh", rfid.uid.sak);
  // print PICC type
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial_printf("%s\n", rfid.PICC_GetTypeName(piccType));
  #endif
  return 0;
}

int8_t RFID_closescan() {
  delay(500);
  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  return 0;
}

int8_t RFID_readblock_keyA(void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len) {
  return _RFID_readblock(buffer, key, block, len, MFRC522::PICC_CMD_MF_AUTH_KEY_A);
}

int8_t RFID_readblock_keyB(void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len) {
  return _RFID_readblock(buffer, key, block, len, MFRC522::PICC_CMD_MF_AUTH_KEY_B);
}

int8_t _RFID_readblock(void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len, uint8_t keytype) {
  MFRC522::StatusCode status;
  uint8_t pseudo_len = 18;
  uint8_t tempbuffer[18];
  status = rfid.PCD_Authenticate(keytype, block, (MFRC522::MIFARE_Key*)key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial_printf("RFID Block Access Denied: %s\n", rfid.GetStatusCodeName(status));
    return -1;
  }
  status = rfid.MIFARE_Read(block, tempbuffer, &pseudo_len);
  if (status != MFRC522::STATUS_OK) {
    Serial_printf("RFID Read failed: %s\n", rfid.GetStatusCodeName(status));
    return -2;
  }
  memcpy(buffer, tempbuffer, len);
  return 0;
}

int8_t RFID_writeblock_keyA(const void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len) {
  return _RFID_writeblock(buffer, key, block, len, MFRC522::PICC_CMD_MF_AUTH_KEY_A);
}

int8_t RFID_writeblock_keyB(const void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len) {
  return _RFID_writeblock(buffer, key, block, len, MFRC522::PICC_CMD_MF_AUTH_KEY_B);
}

int8_t _RFID_writeblock(const void* buffer, uint8_t (*key)[6], uint8_t block, uint8_t len, uint8_t keytype) {
  MFRC522::StatusCode status;
  status = rfid.PCD_Authenticate(keytype, block, (MFRC522::MIFARE_Key*)key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial_printf("RFID Block Access Denied: %s\n", rfid.GetStatusCodeName(status));
    return -1;
  }
  status = rfid.MIFARE_Write(block, *(byte**)&buffer, len); // dumb cast needed
  if (status != MFRC522::STATUS_OK) {
    Serial_printf("RFID Write failed: %s\n", rfid.GetStatusCodeName(status));
    return -2;
  }
  return 0;
}

#endif
