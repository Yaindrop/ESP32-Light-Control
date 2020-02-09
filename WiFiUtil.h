#include <WiFi.h>

class WiFiUtilClass {
 public:
  void syncConnect(Stream& logger);
};

extern WiFiUtilClass WiFiUtil;
