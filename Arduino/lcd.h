#ifndef LCD_H
#define LCD_H

#include <SH1106.h> // 1.3 OLED (SSD1306.h for 0.96 OLED)
#include "font.h"
#include "httpserver.h"

struct DataGPS {
  unsigned int numSV;  // Спутники
  float hAcc;          // Точность
  char gpsTime[9];     // Время
  char bufLatitude[10];
  char bufLongitude[10];
  unsigned int gSpeedKm = 0; // Скорость
};

struct DataAcc {
  char gpsTime[9]; // Время
  char tempC[5];   // Температура
  float accZnX;    // Значения
  float accZnY;
  float accZnZ;
  float accZnXMax; // Максимумы
  float accZnYMax;
  float accZnZMax;
  int valX;        // Прогресс
  int valY;
};

class LCD {
  private:
    SH1106 *display; 
    char buf30[5], buf60[5], buf100[5];
  public:
    bool isset;
    LCD();
    void init();
    void splashScreen();
    void gpsScreen();
    void accScreen();
    void resetScreen();
    void updateSpeed(DataGPS* fullData, Metering* metering);
    void updateAccel(DataAcc* fullData);
};

#endif
