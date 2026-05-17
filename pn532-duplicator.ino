/**************************************************************************/
/*!
    @file     pn532-duplicator.ino
    @author   Modified for RFID Card Duplicator
    @license  MIT (see LICENSE)

    RFID Card Duplicator for D1 Mini + PN532 (I2C Mode)

    功能:
    1. 讀取來源卡片的完整資料 (UID + All Sectors)
    2. 將資料寫入目標 CUID 卡片
    3. 使用內建 LED 指示狀態

    硬體接線:
    - PN532 SDA -> D1 Mini D2 (GPIO4)
    - PN532 SCL -> D1 Mini D1 (GPIO5)
    - PN532 VCC -> D1 Mini 5V
    - PN532 GND -> D1 Mini GND
    - LED: D1 Mini 內建 LED (GPIO2/D4)
*/
/**************************************************************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

// If using the breakout with SPI, define the pins for SPI communication.
//#define PN532_SCK  (2)
//#define PN532_MOSI (3)
//#define PN532_SS   (4)
//#define PN532_MISO (5)

// D1 Mini I2C 接腳: SDA=D2(GPIO4), SCL=D1(GPIO5) - 自動使用
// PN532 I2C 模式 - 不需要 IRQ 和 RESET 接腳

// Uncomment just _one_ line below depending on how your breakout or shield
// is connected to the Arduino:

// Use this line for a breakout with a SPI connection:
//Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
//Adafruit_PN532 nfc(PN532_SS);

// D1 Mini 使用 I2C 連接:
Adafruit_PN532 nfc(-1, -1);

// ===== 常數定義 =====
#define LED_PIN 2  // D1 Mini 內建 LED

// Mifare Classic 預設金鑰
uint8_t defaultKeyA[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

// ===== 狀態機 =====
enum State {
  STATE_READ,   // 讀取模式
  STATE_WRITE   // 寫入模式
};

State currentState = STATE_READ;

// ===== 資料緩衝區 =====
uint8_t sourceUID[7];           // 來源卡片 UID (最多 7 bytes)
uint8_t sourceUIDLength;        // UID 長度
bool cardDataValid = false;     // 資料是否有效

// ===== LED 控制函數 (提前宣告) =====
void ledOn() {
  digitalWrite(LED_PIN, LOW); // 低電位點亮
}

void ledOff() {
  digitalWrite(LED_PIN, HIGH); // 高電位熄滅
}

void ledBlink(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    ledOn();
    delay(delayMs);
    ledOff();
    delay(delayMs);
  }
}

void setup(void) {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Wire.begin(4, 5);
  Wire.setClock(100000);
  Wire.setClockStretchLimit(1500);
  delay(200);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("PN532 錯誤!");
    ledBlink(10, 200);
    while (1) delay(1000);
  }

  Serial.println("RFID 複製器就緒");
  Serial.print("PN532 v"); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  Serial.println("等待來源卡片...");

  currentState = STATE_READ;
}

bool readCard() {
  uint8_t uid[7];
  uint8_t uidLength;

  if (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    return false;
  }

  if (uidLength != 4) {
    Serial.println("錯誤: 只支援 4 byte UID");
    return false;
  }

  Serial.print("來源 UID: ");
  nfc.PrintHex(uid, uidLength);

  memcpy(sourceUID, uid, uidLength);
  sourceUIDLength = uidLength;

  return true;
}

bool writeCard() {
  uint8_t uid[7];
  uint8_t uidLength;
  uint8_t block0[16];

  if (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    return false;
  }

  if (uidLength != 4) {
    Serial.println("錯誤: 只支援 4 byte UID");
    return false;
  }

  Serial.print("目標 UID: ");
  nfc.PrintHex(uid, uidLength);

  if (!nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 0, 0, defaultKeyA) ||
      !nfc.mifareclassic_ReadDataBlock(0, block0)) {
    Serial.println("讀取失敗!");
    return false;
  }

  memcpy(block0, sourceUID, 4);
  block0[4] = block0[0] ^ block0[1] ^ block0[2] ^ block0[3];

  if (!nfc.mifareclassic_WriteDataBlock(0, block0)) {
    Serial.println("寫入失敗! (不是CUID卡?)");
    return false;
  }

  Serial.println("寫入成功!");
  return true;
}

void loop(void) {
  if (currentState == STATE_READ) {
    if (readCard()) {
      cardDataValid = true;
      ledOn();
      currentState = STATE_WRITE;
      Serial.println(">> 請放上目標 CUID 卡");
      delay(2000);
    }
    delay(100);
  }
  else if (currentState == STATE_WRITE) {
    if (!cardDataValid) {
      Serial.println("錯誤: 無來源資料");
      ledBlink(5, 200);
      currentState = STATE_READ;
      return;
    }

    if (writeCard()) {
      ledOff();
      cardDataValid = false;
      currentState = STATE_READ;
      Serial.println(">> 完成! 可繼續下一張\n");
      delay(2000);
    } else {
      delay(500);
    }
  }
}

