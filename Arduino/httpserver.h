#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "data.h"

// Cтруктура результатов
struct Metering {
  float accel30;
  float accel60;
  float accel100;
  float quarter;
};

class HTTPSERVER {
  private:
    Metering* _meterings;
    size_t _n;
    char buf30[5], buf60[5], buf100[5];
    char *httpSid, *httpPass;
  public:
    HTTPSERVER();
    void createAP();
    void serverHandle();
    void setData(Metering* meterings, size_t n);
    void handleRoot();
    void handleStyle();
    void handleResults();
};

#endif
