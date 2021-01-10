#ifndef _M5EPDLIB_H_
#define _M5EPDLIB_H_

#include <M5EPD.h>
#include <WiFi.h>

static const int JST = 3600 * 9;
static const char *wd[7] = {"Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"};

// RTC_SLOW_ATTR bool ntpDataFlag = false;


class M5EPDLib
{
  public:
    M5EPDLib();
    void setupTimeFromNTP(const char* ssid, const char* password);
    bool connectWiFi(const char* ssid, const char* password);
    rtc_date_t getDateFromRTC();
    rtc_time_t getTimeFromRTC();
    String getWeek();
    uint32_t getActiveMills();
  private:
    time_t t;
    struct tm *tm;
    rtc_time_t RTCTime;
    rtc_date_t RTCDate;
    uint32_t power_on_time;
};

extern M5EPDLib M5EPDTools;

#endif