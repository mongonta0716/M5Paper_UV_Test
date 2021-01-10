#include <Arduino.h>
#include <M5EPD.h>
#include <m5epdlib.h>
#define LGFX_AUTODETECT
#include <LovyanGFX.hpp>
#include <WiFi.h>
#include <WiFiConfig.h>  // WiFiConfig.hにssidとpasswordを設定してください。起動時NTPで時刻同期します。

static LGFX gfx;
static LGFX_Sprite status_sp(&gfx);

uint8_t numberX = 16;                    // 市松模様のX軸の数
uint8_t numberY = 9;                     // 市松模様のY軸の数
uint16_t interval_display = 15000;       // 画面表示を書き換える間隔(msec)
uint16_t interval_status = 1000;         // Statusバーを書き換える間隔（msec）
uint8_t status_count_max = 10;           // Statusバーをクリアするカウント

uint32_t lastmillis_display = 0;
uint32_t lastmillis_status = 0;
uint8_t  status_count = 0;
bool drawMode = false; // 市松模様のパターン


void printTime() {
  //gfx.setTextSize(5);
  gfx.setCursor(0, 300);
  gfx.setFont(&fonts::lgfxJapanGothic_36);
  rtc_date_t dt = M5EPDTools.getDateFromRTC();
  rtc_time_t tm = M5EPDTools.getTimeFromRTC();
  gfx.startWrite();
  gfx.printf("Date: %4d/%2d/%2d\n", dt.year, dt.mon, dt.day);
  gfx.printf("Time: %2d:%2d:%2d\n", tm.hour, tm.min, tm.sec);
  gfx.println(M5EPDTools.getWeek());
  gfx.printf("稼働時間(分)：%d", M5EPDTools.getActiveMills()/(1000*60));
  gfx.endWrite();
  gfx.display();
}
void drawRects(bool inverse) {
  // 市松模様を描く
  uint16_t width  = gfx.width() / numberX; // / 2;
  uint16_t height = gfx.height() / numberY / 2;
  uint32_t color;
  gfx.startWrite();
  for (int i=0; i<numberX; i++) {
    for (int j=0; j<numberY; j++) {
      if ((i+j+inverse)%2 == 0) {
        color = TFT_BLACK;
      } else {
        color = TFT_WHITE;
      }
      gfx.fillRect(i*width, j*height, width, height, color);
    }
  }
  gfx.endWrite();
  gfx.display();
}

void drawStatusBar() {
  gfx.startWrite();
  if (status_count == status_count_max) {
    // Statusバーをクリア
    status_sp.fillSprite(TFT_WHITE);
    status_sp.setCursor(gfx.width() / 2, 0);
    status_count = 0;
  }
  status_sp.print("=");
  status_sp.pushSprite(0, gfx.height() - 20);
  gfx.endWrite();
  gfx.display();
  status_count++;
}

void setup() {
  gfx.init();
  M5.begin();
  gfx.clear(TFT_WHITE);
  gfx.setRotation(3);
  gfx.setEpdMode(epd_mode_t::epd_fast);

  M5EPDTools.setupTimeFromNTP(ssid, password);
  drawRects(true);
  printTime();
  lastmillis_display = millis();
  lastmillis_status = millis();

  // Initialize status_bar
  status_sp.setColorDepth(1);
  status_sp.createSprite(gfx.width(), 20);
  status_sp.fillSprite(TFT_WHITE);
  status_sp.setTextSize(2);
  status_sp.setTextColor(TFT_BLACK, TFT_WHITE);
  status_sp.setCursor(gfx.width() / 2, 0);
}


void loop() {
  if ((millis() - lastmillis_status) > interval_status) {
    // ステータスバーの更新
    drawStatusBar();
    lastmillis_status = millis();
  }
  if ((millis() - lastmillis_display) > interval_display){
    // 市松模様と時刻データの更新
    drawRects(drawMode);
    printTime();
    drawMode = !drawMode; // 市松模様を入れ替え
    lastmillis_display = millis();
  }
}