#ifndef HTTPOTA_H
#define HTTPOTA_H

#include <ESPAsyncWebServer.h>
#include <Update.h>
#include <time.h>

class HttpOTA {
 public:
  HttpOTA(AsyncWebServer& server, String otaPath = "/ota/",
          Stream& logger = Serial);

 private:
  AsyncWebServer& _server;
  String _otaPath;
  Stream& _logger;
  time_t _prevTime;
};

#endif
