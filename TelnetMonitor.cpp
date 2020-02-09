#include "TelnetMonitor.h"

void TelnetMonitor::loop() {
  uint8_t i;
  if (WiFi.status() == WL_CONNECTED) {
    if (_server.hasClient()) {
      for (i = 0; i < _maxClient; i++) {
        if (!_clients[i] || !_clients[i].connected()) {
          if (_clients[i]) _clients[i].stop();
          _clients[i] = _server.available();
          if (!_clients[i]) _logger.println("Telnet> Available broken");
          _logger.printf("Telnet> Client %hhu connected from ", i);
          _logger.println(_clients[i].remoteIP());
          break;
        }
      }
      if (i >= _maxClient) {
        _server.available().stop();
      }
    }
    for (i = 0; i < _maxClient; i++) {
      if (_clients[i] && !_clients[i].connected()) {
        _clients[i].stop();
      }
    }
  } else {
    for (i = 0; i < _maxClient; i++) {
      if (_clients[i]) {
        _clients[i].stop();
      }
    }
  }
}
