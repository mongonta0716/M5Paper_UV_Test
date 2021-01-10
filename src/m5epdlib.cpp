#include "m5epdlib.h"

M5EPDLib::M5EPDLib(){ power_on_time = millis(); };

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
}

M5EPDLib M5EPDTools;