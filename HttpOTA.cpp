#include "HttpOTA.h"

HttpOTA::HttpOTA(AsyncWebServer& server, String otaPath, Stream& logger)
    : _server(server), _otaPath(otaPath), _logger(logger), _prevTime(0) {
  auto onUpload = [&](AsyncWebServerRequest* request, String filename,
                      size_t index, uint8_t* data, size_t len, bool final) {
    if (!index) {
      _logger.printf("OTA> Update: %s\r\n", filename.c_str());
      // start with max available size
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        Update.printError(_logger);
      }
    }
    if (Update.write(data, len) == len) {
      time_t currentTime = millis();
      if (currentTime > _prevTime + 1000) {
        _prevTime = currentTime;
        _logger.printf("OTA> Uploading: %u bytes finished\r\n", index);
      }
    } else {
      Update.printError(_logger);
    }
    if (final) {
      Serial.printf("OTA> Uploaded: %s, %u B\n", filename.c_str(), index + len);
      if (Update.end(true)) {
        _logger.println("OTA> Update Success");
      } else {
        Update.printError(_logger);
      }
    }
  };

  _server.on(_otaPath.c_str(), HTTP_POST,
             [&](AsyncWebServerRequest* request) {
               request->send(200, "text/plain",
                             (Update.hasError()) ? "FAILED" : "SUCCESS");
               logger.println("OTA> Response sent, rebooting...");
               ESP.restart();
             },
             onUpload);

  server.onFileUpload(onUpload);
}
