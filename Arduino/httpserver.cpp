#include "httpserver.h"

ESP8266WebServer server(80);  // Создаем веб сервер на 80 порту

HTTPSERVER::HTTPSERVER() { }

void HTTPSERVER::serverHandle() {
  server.handleClient(); // Ждёи подлючения
}

void HTTPSERVER::setData(Metering* meterings, size_t n) {
  _meterings = meterings;
  _n = n;
}

void HTTPSERVER::createAP() {
  WiFi.softAP(httpSid, httpPass); // Создаём точку WiFi
  delay(500);
  server.on("/", [this]() {  // Указываем по каким роутам какие методы запускать
    handleRoot();
  });
  server.on("/style.css", [this]() {
    handleStyle();
  });
  server.on("/results", [this]() {
    handleResults();
  });
  server.begin();
}

void HTTPSERVER::handleRoot() { // Метод формирует стартовую страницу http://192.168.4.1
  server.send(200, "text/html", data_indexHTML);
}

void HTTPSERVER::handleStyle() {
  server.send(200, "text/css", data_styleCSS);
}

void HTTPSERVER::handleResults() {
  StaticJsonBuffer<2000> jsonBuffer;
  JsonArray& arrayJ = jsonBuffer.createArray();
  for (int i = 0; i < _n; i++) {
    if (0.0 != _meterings[i].accel30) {
      JsonObject& object = jsonBuffer.createObject();
      dtostrf(_meterings[i].accel30, 3, 1, buf30);
      object["a30"] = (String)buf30;
      dtostrf(_meterings[i].accel60, 3, 1, buf60);
      object["a60"] = (String)buf60;
      dtostrf(_meterings[i].accel100, 3, 1, buf100);
      object["a100"] = (String)buf100;
      arrayJ.add(object);
    }
  }

  char buffer[512];
  arrayJ.printTo(buffer, sizeof(buffer));
  server.send(200, "text/html", buffer);
}
