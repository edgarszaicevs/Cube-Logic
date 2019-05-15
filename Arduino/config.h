#ifndef CONFIG_H
#define CONFIG_H

const bool accelCalibration = false;  // Режим калибровки гиросерсера, вывод в Serial
const uint8_t  maxG = 2;              // Максимальное значение G (16, 8, 4, 2)
const uint8_t  deltaTemp = 22;        // Корректировка температуры
const uint16_t  accelOffsetX = 1300;  // Калибровка акселерометра 
const uint16_t  accelOffsetY = 3117;  //   (добиться показаий близких к 0 0 16384)
const uint16_t  accelOffsetZ = 789;   //   (2G: 16384, 4G: 8192, 8G: 4096, 16G: 2048)

const char *httpSid  = "CUBELOGIC";    // Название сети WiFi (стартовая страница http://192.168.4.1)
const char *httpPass = "1234567890";   // Пароль для подключения

const bool filterKalman = false;       // Фильтр Кальмана
const float varVolt = 78.9;            // Среднее отклонение (ищем в excel)
const float varProcess = 0.5;          // Сскорость реакции на изменение (подбирается вручную)

#endif
