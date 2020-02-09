#ifndef TELNETMONITOR_H
#define TELNETMONITOR_H

#include <WiFi.h>

class TelnetMonitor {
 public:
  TelnetMonitor(WiFiServer& server, uint8_t maxClient, Stream& logger)
      : _server(server),
        _clients(std::vector<WiFiClient>(maxClient)),
        _maxClient(maxClient),
        _logger(logger) {}
  void loop();
  inline std::vector<WiFiClient>& getClients() { return _clients; };

 private:
  WiFiServer& _server;
  std::vector<WiFiClient> _clients;
  const uint8_t _maxClient;
  Stream& _logger;
};

#endif
