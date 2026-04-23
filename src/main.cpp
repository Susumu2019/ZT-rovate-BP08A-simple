#include <Arduino.h>
#include <Wire.h>
#include <M5Unified.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_NeoPixel.h>

// ===== 設定 =====
static constexpr uint8_t  SERVO_CH = 0,  WS2812_PIN = 6;
static constexpr uint16_t WS2812_NUM = 16, SERVO_MIN = 102, SERVO_MAX = 512;
static constexpr int      I2C_SDA = 2,   I2C_SCL = 1;

Adafruit_PWMServoDriver pca9685(0x40);
Adafruit_NeoPixel pixels(WS2812_NUM, WS2812_PIN, NEO_GRB + NEO_KHZ800);

// ボタン定義
static const uint8_t BTN_ANGLES[3] = {0, 90, 180};
static const char*   BTN_LABELS[3] = {"0", "90", "180"};
int bx[3], by0, bw, bh;

// 画面全体に3つのタッチボタンを描画する
void drawButtons() {
  const int m = 10;
  bw = (M5.Display.width()  - m * 4) / 3;
  bh =  M5.Display.height() - m * 2;
  by0 = m;
  M5.Display.clear(0x0000);
  M5.Display.setTextSize(2);
  for (int i = 0; i < 3; ++i) {
    bx[i] = m + i * (bw + m);
    M5.Display.fillRoundRect(bx[i], by0, bw, bh, 12, 0x03EF);// 水色(シアン)の塗りつぶし
    M5.Display.drawRoundRect(bx[i], by0, bw, bh, 12, 0xFFFF);// 白の枠線
    M5.Display.setTextColor(0xFFFF, 0x03EF);// 白の文字、水色(シアン)の背景
    M5.Display.setCursor(bx[i] + bw / 2 - strlen(BTN_LABELS[i]) * 6, by0 + bh / 2 - 8);// ボタンの中央にテキストを配置
    M5.Display.print(BTN_LABELS[i]);// ボタンの角度ラベルを中央に描画
  }
}

// 各デバイスを初期化し、起動時のサーボ角度(90度)とLED点灯を設定する
void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  pixels.begin();
  pixels.fill(pixels.Color(0, 180, 180));  // WS2812を水色(シアン)で全灯
  pixels.show();

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000);
  pca9685.begin();
  pca9685.setPWMFreq(50);// サーボのPWM周波数は50Hz
  pca9685.setPWM(SERVO_CH, 0, map(90, 0, 180, SERVO_MIN, SERVO_MAX));  // 90度(中立)

  drawButtons();
}

// タッチ入力を監視し、押されたボタンに対応する角度へサーボを移動する
void loop() {
  M5.update();
  if (M5.Touch.getCount() == 0)  { delay(5); return; }
  auto td = M5.Touch.getDetail(0);
  if (!td.wasClicked())          { delay(5); return; }

  for (int i = 0; i < 3; ++i) {
    if (td.x >= bx[i] && td.x < bx[i] + bw && td.y >= by0 && td.y < by0 + bh) {
      pca9685.setPWM(SERVO_CH, 0, map(BTN_ANGLES[i], 0, 180, SERVO_MIN, SERVO_MAX));
      break;
    }
  }
  delay(5);
}
