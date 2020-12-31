# Arduino Mega & OLED ile Tuğla Kırma (Brick Breaker) Oyunu

Bu proje, Arduino Mega kullanılarak Tinkercad üzerinde simüle edilmiş, 128x64 SSD1306 OLED ekran ve Potansiyometre kontrolü ile çalışan retro bir Tuğla Kırma (Brick Breaker) oyunudur. Ayrıca oyuncunun skoru, 2 haneli yedi segmentli (7-Segment) bir ekran üzerinde anlık olarak multiplexing yöntemiyle gösterilmektedir.

# Özellikler
- **Potansiyometre Kontrolü:** Palet, analog potansiyometre yardımıyla hassas ve akıcı bir şekilde hareket ettirilir.
- **Dinamik Zorluk (Seviye Sistemi):** Tüm tuğlalar kırıldığında seviye atlanır ve topun hızı %20 oranında artar.
- **Çift Ekran Entegrasyonu:** Oyun alanı SSD1306 OLED ekranda çizilirken, skor **SevSeg** kütüphanesi kullanılarak gecikmesiz (non-blocking) bir şekilde 7 segmentli ekrana yansıtılır.
- **Rastgele Harita Üretimi:** Oyun her başladığında veya seviye atlandığında tuğlalar rastgele dizilir.
- **Can Düşme Mekaniği:** Kırılan tuğlalardan %10 şansla ekstra can düşebilir.

---

## 🛠️ Kullanılan Donanımlar / Komponentler
- 1 x Arduino Mega 2560
- 1 x SSD1306 128x64 I2C OLED Ekran
- 1 x 2 Haneli 7 Segment Ekran (Ortak Anot)
- 1 x Potansiyometre
- Bağlantı Kabloları

---

## 📐 Devre Şeması (Pin Bağlantıları)

### OLED Ekran Bağlantıları
| OLED Pin | Arduino Mega Pini |
| :--- | :--- |
| GND | GND |
| VCC / Vin | 5V |
| Clk (SCL) | Pin 21 (SCL) |
| Data (SDA) | Pin 20 (SDA) |

### Potansiyometre Bağlantıları
| Potansiyometre Pin | Arduino Mega Pini |
| :--- | :--- |
| Sol Pin | GND |
| Orta Pin (Sinyal) | A0 (Analog Giriş) |
| Sağ Pin | 5V |

### 2 Haneli 7 Segment Ekran Bağlantıları
| Segment/Digit Pin | Arduino Mega Pini |
| :--- | :--- |
| Digit 1 (Anot) | Pin 2 |
| Digit 2 (Anot) | Pin 3 |
| Segment A | Pin 6 |
| Segment B | Pin 7 |
| Segment C | Pin 8 |
| Segment D | Pin 9 |
| Segment E | Pin 10 |
| Segment F | Pin 11 |
| Segment G | Pin 12 |
| Segment DP (Nokta) | Pin 5 |

---

