#include "WiFiUtil.h"

void WiFiUtilClass::syncConnect(Stream& logger) {
  logger.print("WiFi> Start Connecting to ");
  logger.print(WiFi.SSID());
  int trials = 0;
  wl_status_t prevStatus = WL_DISCONNECTED, status;
  do {
    status = WiFi.status();
    if (trials > 0 && status == prevStatus) {
      logger.print(".");
      WiFi.begin();
    } else {
      logger.println();
      logger.print("WiFi> Status: ");
      prevStatus = status;
      switch (status) {
        case WL_NO_SHIELD:
          logger.print("No Shield");
          break;
        case WL_IDLE_STATUS:
          logger.print("Idle Status");
          break;
        case WL_NO_SSID_AVAIL:
          logger.print("No SSID Available");
          break;
        case WL_SCAN_COMPLETED:
          logger.print("Scan Completed");
          break;
        case WL_CONNECTED:
          logger.print("Connected");
          break;
        case WL_CONNECT_FAILED:
          logger.print("Connect Failed");
          break;
        case WL_CONNECTION_LOST:
          logger.print("Connection Lost");
          break;
        case WL_DISCONNECTED:
          logger.print("Disconnected");
          break;
        default:
          break;
      }
    }
    trials++;
    delay(1000);
  } while (status != WL_CONNECTED);
  logger.println();
  logger.print("WiFi> Current IP address ");
  logger.println(WiFi.localIP());
}

WiFiUtilClass WiFiUtil;