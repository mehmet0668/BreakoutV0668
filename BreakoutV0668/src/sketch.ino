#include "SevSeg.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH        128 // OLED ekran genişliği, piksel cinsinden
#define SCREEN_HEIGHT       64  // OLED ekran yüksekliği, piksel cinsinden

// I2C ile bağlı bir SSD1306 ekranı için bildiri (SDA, SCL pinleri)
#define OLED_RESET          -1   // Tinkercad üzerinde RESET pini olmadığı için -1 yapıldı
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define PALET_WIDTH         20  // Palet genişliği, piksel cinsinden
#define PALET_HEIGHT        2   // Palet yüksekliği, piksel cinsinden
#define BALL_SIZE           4   // Topun çapı, piksel cinsinden
#define BRICK_WIDTH         10  // Tuğla genişliği, piksel cinsinden
#define BRICK_HEIGHT        8   // Tuğla yüksekliği, piksel cinsinden
#define MAP_WIDTH           4   // Oyun alanı genişliği (tuğla sayısı)
#define MAP_HEIGHT          4   // Oyun alanı yüksekliği (tuğla sayısı)
#define POTENTIOMETER_PIN   A0
#define HEART_WIDTH         8   // Kalp simgesinin genişliği, piksel cinsinden
#define HEART_HEIGHT        8   // Kalp simgesinin yüksekliği, piksel cinsinden

#define PALET_SPEED         2   // Paletin hareket hızı
#define BASE_BALL_SPEED     2   // Başlangıç ​​top hızı
#define BALL_SPEED_INC      0.2 // Her seviyede top hızının artış yüzdesi
#define CAN_DROP_PROBABILITY 10 // %10 olasılıkla can düşürme olasılığı

int8_t paletX = (SCREEN_WIDTH - PALET_WIDTH) / 2; // Paletin başlangıç ​​konumu
int8_t ballX = SCREEN_WIDTH / 2 - BALL_SIZE / 2;   // Topun başlangıç ​​konumu X
int8_t ballY = SCREEN_HEIGHT - PALET_HEIGHT - BALL_SIZE - 1; // Topun başlangıç ​​konumu Y

int8_t ballSpeedX = BASE_BALL_SPEED; // Topun X yönündeki hızı
int8_t ballSpeedY = -BASE_BALL_SPEED; // Topun Y yönündeki hızı

bool bricks[MAP_WIDTH][MAP_HEIGHT]; // Tuğlaların durumunu tutan matris
bool heartAvailable = false; // Can elemanının mevcut olup olmadığını belirten bayrak
int lives = 3; // Can sayısı
int score = 0; // Skor
int level = 1; // Seviye

SevSeg sevseg; // Yedi segment kontrolcü nesnesi

unsigned long previousMillis = 0;
const long interval = 20; // delay(20) yerine non-blocking (gecikmesiz) zamanlama

void setup() {
  byte numDigits = 2;
  byte digitPins[] = {2, 3};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 5};
  bool resistorsOnSegments = false; 
  byte hardwareConfig = COMMON_ANODE; 
  bool updateWithDelays = false; 
  bool leadingZeros = false; 
  bool disableDecPoint = false; 

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
               updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90); 

  pinMode(POTENTIOMETER_PIN, INPUT); 

  Serial.begin(9600);

  // KRİTİK DÜZELTME: Tinkercad OLED adresi 0x3C olarak güncellendi
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 baslatilamadi!"));
    for (;;) ; // Ekran bulunamazsa burada kalır
  }
  
  randomSeed(analogRead(A1)); // Boş bir analog pinden seed almak daha iyidir

  resetGame();
}

void resetGame() {
  for (int x = 0; x < MAP_WIDTH; x++) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
      bricks[x][y] = random(2); 
    }
  }

  display.clearDisplay();
  drawMap();
  drawPalet(paletX);
  drawBall(ballX, ballY);
  drawScore();
  drawLives();
  display.display();
  
  sevseg.setNumber(score); // Yedi segment ekran ilk değerini ver
}

void loop() {
  // Yedi segment ekranın stabil çalışması için her döngüde yenilenmesi şarttır!
  sevseg.refreshDisplay(); 

  // delay(20) kullanmak yerine millis() kullanarak oyun hızını ayarlıyoruz
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Paletin hareketini oku
    readPaletInput();

    // Topu hareket ettir
    moveBall();

    // Çarpışmaları kontrol et
    checkCollisions();

    // Oyun alanını ve nesneleri ekrana çiz
    display.clearDisplay();
    drawMap();
    drawPalet(paletX);
    drawBall(ballX, ballY);
    drawScore();
    drawLives();
    display.display();
  }
}

void readPaletInput() {
  int potValue = analogRead(POTENTIOMETER_PIN);
  int paletPosition = map(potValue, 0, 1023, 0, SCREEN_WIDTH - PALET_WIDTH);
  
  if (paletPosition >= 0 && paletPosition <= SCREEN_WIDTH - PALET_WIDTH) {
    paletX = paletPosition;
  }
}

void moveBall() {
  ballX += ballSpeedX;
  ballY += ballSpeedY;
}

void checkCollisions() {
  if (ballX <= 0 || ballX >= SCREEN_WIDTH - BALL_SIZE) {
    ballSpeedX = -ballSpeedX;
  }
  if (ballY <= 0) {
    ballSpeedY = -ballSpeedY;
  }

  if (ballX + BALL_SIZE >= paletX && ballX <= paletX + PALET_WIDTH && ballY + BALL_SIZE >= SCREEN_HEIGHT - PALET_HEIGHT) {
    ballSpeedY = -ballSpeedY;
  }

  for (int x = 0; x < MAP_WIDTH; x++) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
      if (bricks[x][y] && ballX + BALL_SIZE >= x * BRICK_WIDTH && ballX <= (x + 1) * BRICK_WIDTH && ballY + BALL_SIZE >= y * BRICK_HEIGHT && ballY <= (y + 1) * BRICK_HEIGHT) {
        bricks[x][y] = false; 
        ballSpeedY = -ballSpeedY; 
        score++; 
        sevseg.setNumber(score); // Skor değiştiğinde 7 segmenti bilgilendir
        if (random(100) < CAN_DROP_PROBABILITY) { 
          heartAvailable = true; 
        }
      }
    }
  }

  if (heartAvailable && ballX + BALL_SIZE / 2 >= paletX && ballX + BALL_SIZE / 2 <= paletX + PALET_WIDTH && ballY + BALL_SIZE >= SCREEN_HEIGHT - PALET_HEIGHT) {
    lives++;
    heartAvailable = false;
  }

  if (ballY >= SCREEN_HEIGHT) {
    lives--; 
    if (lives == 0) {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 20);
      display.println(F("GAME OVER"));
      display.display();
      
      // Game over durumunda ekranın donup kalmaması için kısa bir döngü
      unsigned long startWait = millis();
      while(millis() - startWait < 2000) {
         sevseg.refreshDisplay(); // Beklerken ekranların sönmesini engeller
      }
      
      score = 0;
      level = 1; 
      lives = 3;
      ballSpeedX = BASE_BALL_SPEED; 
      ballSpeedY = -BASE_BALL_SPEED;
      resetGame();
    } else {
      resetBall(); 
    }
  }

  bool allBricksDestroyed = true;
  for (int x = 0; x < MAP_WIDTH; x++) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
      if (bricks[x][y]) {
        allBricksDestroyed = false;
        break;
      }
    }
    if (!allBricksDestroyed) break;
  }

  if (allBricksDestroyed) {
    level++;
    ballSpeedX *= (1 + BALL_SPEED_INC); 
    ballSpeedY *= (1 + BALL_SPEED_INC);
    resetGame();
  }
}

void resetBall() {
  ballX = SCREEN_WIDTH / 2 - BALL_SIZE / 2;   
  ballY = SCREEN_HEIGHT - PALET_HEIGHT - BALL_SIZE - 1; 
  ballSpeedX = BASE_BALL_SPEED; 
  ballSpeedY = -BASE_BALL_SPEED;
}

void drawMap() {
  for (int x = 0; x < MAP_WIDTH; x++) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
      if (bricks[x][y]) {
        display.fillRect(x * BRICK_WIDTH, y * BRICK_HEIGHT, BRICK_WIDTH, BRICK_HEIGHT, SSD1306_WHITE);
      }
    }
  }
}

void drawPalet(int8_t x) {
  display.fillRect(x, SCREEN_HEIGHT - PALET_HEIGHT, PALET_WIDTH, PALET_HEIGHT, SSD1306_WHITE);
}

void drawBall(int8_t x, int8_t y) {
  display.fillCircle(x + BALL_SIZE / 2, y + BALL_SIZE / 2, BALL_SIZE / 2, SSD1306_WHITE);
}

void drawScore() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 2);
  display.print(F("Level: "));
  display.println(level);
  display.setCursor(2, 12);
  display.print(F("Score: "));
  display.println(score);
}

void drawLives() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(70, 2);
  display.print(F("Lives: "));
  display.println(lives);
}