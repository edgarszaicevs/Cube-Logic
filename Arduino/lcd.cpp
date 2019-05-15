#include "lcd.h"

LCD::LCD() {
  // Connect the OLED to Wemos pins D1 and D2
  Wire.begin(4, 5);
  Wire.beginTransmission(0x3c);
  uint8_t error = Wire.endTransmission();
  if (error == 0) {
    isset = true;
    display = new SH1106(0x3c, 4, 5);
  } else {
    isset = false;
  }
}

void LCD::init() {
  if (!isset) {
    return;
  }
  display->init();
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
}

void LCD::splashScreen() {
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(Orbitron_Light_30);
  display->drawString(64, 6, "CUBE");
  display->drawString(64, 34, "logic");
  display->display();
}

void LCD::gpsScreen() {
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  display->drawString(64, 16, "GPS");
  display->drawString(64, 30, "searching...");
  display->display();
}

void LCD::accScreen() {
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  display->drawString(64, 16, "ACCEL");
  display->drawString(64, 30, "connecting...");
  display->display();
}

void LCD::resetScreen() {
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  display->drawString(64, 16, "reseting...");
  display->display();
}

void LCD::updateSpeed(DataGPS* fullData, Metering* metering) {
  display->clear();
  display->drawVerticalLine(7, 2, 8);
  display->drawVerticalLine(6, 2, 8);
  display->drawVerticalLine(4, 5, 5);
  display->drawVerticalLine(3, 5, 5);
  display->drawVerticalLine(1, 8, 2);
  display->drawVerticalLine(0, 8, 2);
  display->drawHorizontalLine(0, 11, 128);
  display->drawHorizontalLine(0, 54, 128);

  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(11, 0, (String)fullData->numSV);

  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(42, 0, (String)fullData->hAcc);

  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, fullData->gpsTime);

  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_16);
  dtostrf(metering->accel30, 3, 1, buf30);
  display->drawString(122, 10, (String)buf30);

  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_16);
  dtostrf(metering->accel60, 3, 1, buf60);
  display->drawString(122, 24, (String)buf60);

  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_16);
  dtostrf(metering->accel100, 3, 1, buf100);
  display->drawString(122, 38, (String)buf100);

  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0, 54, fullData->bufLatitude);

  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 54, fullData->bufLongitude);

  char gpsSpeed[3];
  sprintf(gpsSpeed, "%03d", fullData->gSpeedKm);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(Orbitron_Light_30);
  display->drawString(45, 20, gpsSpeed);
  display->display();
}

void LCD::updateAccel(DataAcc* fullData) {
  display->clear();
  display->drawVerticalLine(8, 17, 12);
  display->drawVerticalLine(9, 17, 12);
  display->drawHorizontalLine(5, 16, 8);
  display->drawHorizontalLine(5, 17, 8);
  display->drawHorizontalLine(5, 29, 8);
  display->drawHorizontalLine(5, 30, 8);
  display->drawHorizontalLine(3, 40, 12);
  display->drawHorizontalLine(3, 41, 12);
  display->drawVerticalLine(2, 37, 8);
  display->drawVerticalLine(3, 37, 8);
  display->drawVerticalLine(15, 37, 8);
  display->drawVerticalLine(16, 37, 8); 

  display->drawHorizontalLine(0, 11, 128);
  display->drawHorizontalLine(0, 54, 128);

  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0, 0, (String)fullData->tempC);
  
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, fullData->gpsTime);

  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  char accX[3];
  sprintf(accX, "%.2fG", fullData->accZnXMax);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_16);
  display->drawString(22, 14, accX);

  char accY[3];
  sprintf(accY, "%.2fG", fullData->accZnYMax);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_16);
  display->drawString(22, 31, accY);

  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0, 54, (String)fullData->accZnX);

  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(65, 54, (String)fullData->accZnY);

  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 54, (String)fullData->accZnZ);

  for (int i=0; i<fullData->valX; i++) {
    int k = 10 * i;
    display->fillRect(70 + k, 21, 4, 4);
  }
  for (int i=0; i<fullData->valY; i++) {
    int k = 10 * i;
    display->fillRect(70 + k, 38, 4, 4);
  }
  display->display();
}
