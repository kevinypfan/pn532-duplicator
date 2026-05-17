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
