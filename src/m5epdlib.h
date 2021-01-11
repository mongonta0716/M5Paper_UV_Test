#ifndef _M5EPDLIB_H_
#define _M5EPDLIB_H_

#include <M5EPD.h>
#include <WiFi.h>
#include <lgfx.h>

static const int JST = 3600 * 9;
static const char *wd[7] = {"Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"};

const lgfx::U8g2font status_bar_font = fonts::lgfxJapanGothic_20;

class M5EPDLib
{
  private:
    rtc_time_t RTCTime;
    rtc_date_t RTCDate;
    uint32_t power_on_time;
    LGFX *_gfx;
    LGFX_Sprite *_status_bar;
    LGFX_Sprite *_power_icon;
    uint8_t _height; // height of status bar
    uint16_t _x;     // x-axis of status bar
    uint16_t _y;     // y-axis of status bar
    boolean _upside;

    uint8_t count_power;
    uint8_t count_power_max = 10;
    float pwdeg;    // PowerIconの針の角度
    float pwx;      // PowerIconの針外側X座標
    float pwy;      // PowerIconの針外側Y座標
    uint32_t opwx;  // PowerIconの針外側X実座標
    uint32_t opwy;  // PowerIconの針外側Y実座標

  public:
    M5EPDLib();
    void setupTimeFromNTP(const char* ssid, const char* password);
    bool connectWiFi(const char* ssid, const char* password);
    rtc_date_t getDateFromRTC();
    rtc_time_t getTimeFromRTC();
    String getWeek();
    uint32_t getActiveMills();
    void createSprite(LGFX *gfx, uint8_t height_of_bar, boolean upside);
    void drawString(String str);
    void drawDateTime();
    void drawVBattery();
    void drawPowerIcon();
};

extern M5EPDLib M5EPDTools;

#endif