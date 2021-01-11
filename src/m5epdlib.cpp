#include "m5epdlib.h"

M5EPDLib::M5EPDLib() {
  power_on_time = millis();
  count_power = 0;
};

void M5EPDLib::setupTimeFromNTP(const char* ssid, const char* password){
  M5.RTC.begin();
  this->connectWiFi(ssid, password);
  configTime(9*3600L, 0, "ntp.nict.jp", "time.google.com");
  struct tm timeinfo;
  Serial.println("Get Time from NTP");
  while (!getLocalTime(&timeinfo)) {
    Serial.println(".");
    delay(1000);
  }
  this->t = time(NULL);
  this->tm = localtime(&t);

  RTCTime.hour = tm->tm_hour;
  RTCTime.min = tm->tm_min;
  RTCTime.sec = tm->tm_sec;
  M5.RTC.setTime(&RTCTime);

  RTCDate.year = tm->tm_year + 1900;
  RTCDate.mon = tm->tm_mon + 1;
  RTCDate.day = tm->tm_mday;
  RTCDate.week = tm->tm_wday;
  M5.RTC.setDate(&RTCDate);
  Serial.println("NTPTime Set");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

rtc_date_t M5EPDLib::getDateFromRTC() {
  M5.RTC.getDate(&RTCDate);
  return RTCDate;
}

rtc_time_t M5EPDLib::getTimeFromRTC() {
  M5.RTC.getTime(&RTCTime);
  return RTCTime;
}

String M5EPDLib::getWeek() {
  M5.RTC.getDate(&RTCDate);
  return wd[RTCDate.week];
}

uint32_t M5EPDLib::getActiveMills() {
  return (millis() - power_on_time);
}



bool M5EPDLib::connectWiFi(const char* ssid, const char* password) {

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  return true;
}

void M5EPDLib::createSprite(LGFX *gfx, uint8_t height, boolean upside) {
  _gfx = gfx;
  _x = height; // パワーアイコン描画のため
  _height = height;
  _upside = upside;
  if (upside) {
    // ステータスバーを上に表示
    _y = 0;
  } else {
    // ステータスバーを下に表示
    _y = _gfx->height() - _height;
  }
  _status_bar = new LGFX_Sprite(_gfx);
  _status_bar->setColorDepth(1);
  _status_bar->createSprite(_gfx->width() - _height, _height);
  _power_icon = new LGFX_Sprite(_gfx);
  _power_icon->setColorDepth(1);
  _power_icon->createSprite(_height, _height);
  _gfx->startWrite();
  _status_bar->fillSprite(TFT_BLACK);
  _status_bar->setFont(&status_bar_font);
  _status_bar->setTextDatum( middle_center );
  //_status_bar->setTextSize(2);
  _status_bar->setTextColor(TFT_WHITE, TFT_BLACK);
  //_status_bar->drawString("StatusBar", _gfx->width() / 2, _height / 2);
  _status_bar->pushSprite(_x, _y);
  _power_icon->fillSprite(TFT_BLACK);
  _power_icon->fillCircle(_height / 2, _height / 2, _height / 2, TFT_WHITE);
  //_power_icon->drawRect(_height / 2, 0, 2, _height / 2, TFT_BLACK);
  _power_icon->pushSprite(0, _y);
  _gfx->endWrite();
  _gfx->display();

}

void M5EPDLib::drawDateTime() {
  char str[17];
  _gfx->startWrite();
  sprintf(str, "%04d/%02d/%02d %02d:%02d", RTCDate.year, RTCDate.mon, RTCDate.day, RTCTime.hour, RTCTime.min);
//  _status_bar->setTextDatum(textdatum_t::middle_center);
  // 右に日付・時間を描画
  _status_bar->drawString(str, _gfx->width() - 100, _height / 2);
  _status_bar->pushSprite(_x, _y);
  _gfx->endWrite();
  _gfx->display();
}

void M5EPDLib::drawString(String str) {
  _gfx->startWrite();
  _status_bar->drawString(str, _gfx->width() / 2, _height / 2);
  _status_bar->pushSprite(_x, _y);
  _gfx->endWrite();
  _gfx->display();
}

void M5EPDLib::drawVBattery() {
  M5.BatteryADCBegin();
  uint32_t BatteryV = M5.getBatteryVoltage();
  uint32_t BatteryR = M5.getBatteryRaw();
  char str[30];
  sprintf(str, "Bat:%d mV/%d mV", BatteryV, BatteryR);
  _gfx->startWrite();
  _status_bar->drawString(str, 150, _height / 2);
  _status_bar->pushSprite(_x, _y);
  _gfx->endWrite();
  _gfx->display();
}

void M5EPDLib::drawPowerIcon() {
  // 針の角度を計算
  pwdeg = count_power * (360 / (float)count_power_max);

  pwx = cos((pwdeg-90)*0.0174532925);
  pwy = sin((pwdeg-90)*0.0174532925);
  opwx = pwx*(_height / 2) + (_height / 2);
  opwy = pwy*(_height / 2) + (_height / 2);

  _gfx->startWrite();
  _power_icon->drawLine(opwx, opwy, (_height / 2), (_height / 2), TFT_BLACK);
//  _power_icon->pushSprite(400,400);
  _power_icon->pushSprite(0, _y);
//  _status_bar->pushSprite(0, _y);
  count_power++;
  if (count_power > (count_power_max - 1)) {
    _power_icon->fillCircle(_height / 2, _height / 2, _height / 2, TFT_WHITE);
    count_power = 0;
  }
  _gfx->endWrite();
  _gfx->display();
}

M5EPDLib M5EPDTools;