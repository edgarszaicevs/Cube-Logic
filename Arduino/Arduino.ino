// ----------------------------------------------------------------------------
// CubeLogic
// версия 1.0
// (c) 2019 Дмитрий Дементьев <info@phpscript.ru>
// ----------------------------------------------------------------------------

#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif
#include "config.h"
#include "button.h"
#include "lcd.h"
#include "ubloxgps.h"
#include "accel.h"
#include "httpserver.h"
#include "I2Cdev.h"

HTTPSERVER srv;
LCD        lcd;
MPU6050    accelgyro;
BUTTON     button;

Metering metering;
Metering meterings[10];
DataGPS  fullDataGPS;
DataAcc  fullDataAcc;

unsigned long lastScreenUpdate = 0; // Частота обновления дисплея
float latitude  = 0.0;              // Ширина
float longitude = 0.0;              // Долгота
float meteringTime  = 0.0;          // Время замера
bool onlySpeedMeter = false;        // Режим спидометра
unsigned long startMillis   = 0;    // Начало отсчета
unsigned long startITOW     = 0;    // Начало отсчета из GPS
unsigned long currentMillis = 0;    // Текущее время
bool start        = false;          // Начало замера
bool meteringSave = false;          // Сохранён ли последний замер
double startLat  = 0.0;           
double startLon  = 0.0;
double finishLat = 0.0;
double finishLon = 0.0;
int btnValue = 1;                   // Текущая кнопка
int16_t ax, ay, az, tmp;            // Значения акселерометра

void setup() {
  
  pinMode(2, INPUT);
  accelgyro.initialize();
  switch (maxG) {
    case 16: accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16); break;
    case 8:  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_8);  break;
    case 4:  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_4);  break;
    case 2:  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);  break;
  } 
  accelgyro.setXAccelOffset(accelOffsetX);
  accelgyro.setYAccelOffset(accelOffsetY);
  accelgyro.setZAccelOffset(accelOffsetZ);
  Serial.begin(9600);
  serial.begin(115200);
  delay(500);
  metering = {0.0, 0.0, 0.0, 0.0};
  gpsSetup();
  srv.createAP();
  lcd.init();
  lcd.splashScreen();
  delay(3000);
}

void loop() {
  
  currentMillis = millis(); // Текущее время в миллисекундах
  int msgTypeGPS = processGPS();
  int msgTypeACC = accelgyro.testConnection();
  
  /**
  * Калибровка гироскопа 
  */
  if (accelCalibration == true) {
     accelgyro.getAcceleration(&ax, &ay, &az);
     Serial.print("X=");
     Serial.print(ax);
     Serial.print("\t");
     Serial.print("Y=");
     Serial.print(ay);
     Serial.print("\t");
     Serial.print("Z=");
     Serial.println(az);
  }

  /**
  * Кнопка
  */
  int b = button.checkButton();
  if (b == 1) { // Нажатие на кнопку (выбор режима)
    if (btnValue != 1) btnValue = 1; else btnValue = 2;
  }
  if (b == 3) { // Удержание кнопки (обнуление замеров)
    resetValues();
  }
  if (b == 2) { } // Двойное нажатие на кнопку
  if (b == 4) { } // Долгое удержание кнопки

  /**
   * Получение данных
   */
  if (msgTypeGPS == MT_NAV_PVT) {
    fullDataGPS.numSV = ubxMessage.navPvt.numSV;
    fullDataGPS.gSpeedKm = ubxMessage.navPvt.gSpeed * 0.0036;   
    sprintf(fullDataGPS.gpsTime, "%02d:%02d:%02d", ubxMessage.navPvt.hour, ubxMessage.navPvt.minute, ubxMessage.navPvt.second);
    latitude = (float)(ubxMessage.navPvt.lat / 10000000.0f);
    longitude = (float)(ubxMessage.navPvt.lon / 10000000.0f);
    dtostrf(latitude, 8, 6, fullDataGPS.bufLatitude);
    dtostrf(longitude, 8, 6, fullDataGPS.bufLongitude);
    fullDataGPS.hAcc = ubxMessage.navPvt.hAcc / 1000.0f;
  }
  if (msgTypeACC == 1) {
    sprintf(fullDataAcc.gpsTime, "%02d:%02d:%02d", ubxMessage.navPvt.hour, ubxMessage.navPvt.minute, ubxMessage.navPvt.second);
    accelgyro.getAcceleration(&ax, &ay, &az);
    tmp = accelgyro.getTemperature() / 340.00 + deltaTemp;
    sprintf(fullDataAcc.tempC, "%02d°C", tmp);
    float del;
    switch (maxG) {
      case 16: del = 2047.99;  break;
      case 8:  del = 4095.99;  break;
      case 4:  del = 8191.99;  break;
      case 2:  del = 16383.99; break;
    }
    if (filterKalman == true) {
      ax = kalmanFilter(ax);
      ay = kalmanFilter(ay);
      az = kalmanFilter(az);
    }
    fullDataAcc.accZnX = abs(ax) / del;
    fullDataAcc.accZnY = abs(ay) / del;
    fullDataAcc.accZnZ = abs(az) / del;
    if (fullDataAcc.accZnXMax < fullDataAcc.accZnX) fullDataAcc.accZnXMax = fullDataAcc.accZnX;
    if (fullDataAcc.accZnYMax < fullDataAcc.accZnY) fullDataAcc.accZnYMax = fullDataAcc.accZnY;
    if (fullDataAcc.accZnZMax < fullDataAcc.accZnZ) fullDataAcc.accZnZMax = fullDataAcc.accZnZ;
    fullDataAcc.valX = mapToFloat(fullDataAcc.accZnX, 0.0, float(maxG), 0, 6);
    fullDataAcc.valY = mapToFloat(fullDataAcc.accZnY, 0.0, maxG, 0, 6);
  }
 
  /**
  * Замер скорости
  */
  if (fullDataGPS.gSpeedKm > 0 && btnValue == 1) { // Если движемся и выбрана кнопка "1"
    meteringTime = (float)(ubxMessage.navPvt.iTOW - startITOW) / 1000;   // Время замера
    if (!start) { // Если до этого небыло старта
      start = true;
      meteringSave = false;
      startMillis = millis();
      startITOW = ubxMessage.navPvt.iTOW;
      metering = {0.0, 0.0, 0.0, 0.0};
      startLat = latitude;
      startLon = longitude;
      finishLat = 0.0;
      finishLon = 0.0;
      onlySpeedMeter = false;
    } else if (meteringTime > 99) { // Если замер длится дольше 99 секунд
      onlySpeedMeter = true;
    }
    if (!onlySpeedMeter) { // Замеры разгона
      if (0.0 == metering.accel30 && fullDataGPS.gSpeedKm >= 30) { // Разгон до 30км/ч
        metering.accel30 = meteringTime;
      } else if (0.0 == metering.accel60 && fullDataGPS.gSpeedKm >= 60) { // Разгон до 60км/ч
        metering.accel60 = meteringTime;
      } else if (0.0 == metering.accel100 && fullDataGPS.gSpeedKm >= 100) { // Разгон до 100км/ч
        metering.accel100 = meteringTime;
      }
    }
  } else if (start && 0 == fullDataGPS.gSpeedKm) { // Если была ложная тревога
    resetValues();
  }

  /**
   * Если замер завершен
   */
  if (!meteringSave && 0.0 != metering.accel100 && btnValue == 1) { // И выбрана кнопка "1"
    Metering *meteringsNew = new Metering[10]; //  Последний результат всегда сверху
    memcpy(meteringsNew, meterings, sizeof(meterings[0]) * 10);
    meterings[0] = metering;
    for (int i = 0; i < 9; i++) {
      meterings[i + 1] = meteringsNew[i];
    }
    delete[] meteringsNew;
    srv.setData(meterings, (int)(sizeof(meterings) / sizeof(meterings[0])));
    meteringSave = true;
  }

  /**
   * Экран
   */
  unsigned long now = millis();
  if (now - lastScreenUpdate > 100) {
    if (btnValue == 1) { // Если выбрана кнопка "1" (GPS)
      if (0 == fullDataGPS.numSV) { // Если нет спутников
        lcd.gpsScreen();
      } else {
        lcd.updateSpeed(&fullDataGPS, &metering);
      }
    }
    if (btnValue == 2) { // Если выбрана кнопка "2" (ACC)
      if (msgTypeACC != 1) { // Если нет дынных
        lcd.accScreen();
      } else {
        lcd.updateAccel(&fullDataAcc);
      }
    }
    lastScreenUpdate = now;
  }

  /**
   * Ждём подключения (только если не делаем замер)
   */
  if (!start || onlySpeedMeter) {
    srv.serverHandle();
  }
}

void resetValues() {
    start = false;
    meteringSave = false;
    startMillis = 0;
    meteringTime = 0;
    startLat = 0.0;
    startLon = 0.0;
    finishLat = 0.0;
    finishLon = 0.0;
    fullDataAcc.accZnX = 0.0;
    fullDataAcc.accZnY = 0.0;
    fullDataAcc.accZnZ = 0.0;
    fullDataAcc.accZnXMax = 0.0;
    fullDataAcc.accZnYMax = 0.0;
    fullDataAcc.accZnZMax = 0.0;
    lcd.resetScreen();
    delay(1000);
}
