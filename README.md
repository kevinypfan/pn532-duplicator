# pn532-duplicator

D1 Mini (ESP8266) + PN532 (I2C 模式) 的 Mifare Classic 卡片複製器。
讀取來源卡片的 UID，再寫入到可改寫 block 0 的 CUID 卡片，並用內建 LED 指示狀態。

## 硬體接線

| PN532 | D1 Mini |
|-------|---------|
| SDA   | D2 (GPIO4) |
| SCL   | D1 (GPIO5) |
| VCC   | 5V |
| GND   | GND |

LED 使用 D1 Mini 內建 LED（GPIO2 / D4），低電位點亮。

> PN532 需切換到 **I2C 模式**（板上 DIP 開關 / 跳線）。

### 硬體實作筆記

實際組裝環境：

- **PN532 模組**：紅色 NFC 板，右側 2-pin DIP 切換開關需撥到 **I2C 模式**
  （SPI/I2C/HSU 三選一，務必設成 I2C，否則 `nfc.begin()` 偵測不到）。
- **主控板**：WeMos / LOLIN **D1 Mini Pro**（ESP8266，帶 IPEX 外接天線座，
  本機用 Rainsun 陶瓷天線）。一般 D1 Mini 亦適用。
- **連接方式**：母對母杜邦線，PN532 底部 `GND / VCC / SDA / SCL` 4 pin
  對接 D1 Mini，對應關係如上表（SDA→D2、SCL→D1、VCC→5V、GND→GND）。
- 供電：以 D1 Mini 的 5V 供 PN532 VCC；亦可改用 3V3，視模組而定。

## 編譯與燒錄

- Arduino IDE（或 arduino-cli）
- 安裝 ESP8266 board 套件，開發板選 **LOLIN(WeMos) D1 R2 & mini**
- 安裝函式庫：**Adafruit PN532**
- 開啟 `pn532-duplicator.ino` 編譯並上傳，序列埠監控設 `115200`

## 使用流程

1. 上電後等待序列埠出現「RFID 複製器就緒」。
2. 放上**來源卡片**，讀到 UID 後 LED 恆亮，提示放上目標卡。
3. 換上**目標 CUID 卡**（block 0 可寫的魔術卡），自動寫入。
4. 看到「完成!」即可換下一張；寫入失敗代表該卡非 CUID 卡。

目前僅支援 4-byte UID 的 Mifare Classic 卡片。

## 免責聲明

本專案僅供 RFID/NFC 技術學習與測試**自己擁有的卡片**之用途。
請勿用於複製未經授權的卡片或任何違法用途；使用者須自行承擔相關責任。

## 授權

MIT License，詳見 [LICENSE](LICENSE)。
