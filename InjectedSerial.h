#ifndef INJECTEDSERIAL_H
#define INJECTEDSERIAL_H

#include <functional>
#include "HardwareSerial.h"
#include "Stream.h"
#include "esp32-hal.h"

#define EMPTY_FN [](const uint8_t* buffer, size_t size) {}
typedef std::function<void(const uint8_t*, size_t)> WriteFunction;

class InjectedSerial : public Stream {
 public:
  InjectedSerial(HardwareSerial& serial)
      : _serial(serial), writeHandler(EMPTY_FN) {}
  InjectedSerial(HardwareSerial& serial, WriteFunction fn)
      : _serial(serial), writeHandler(fn) {}
  inline void setWriteFn(WriteFunction fn) { writeHandler = fn; }
  inline void begin(unsigned long baud, uint32_t config = SERIAL_8N1,
                    int8_t rxPin = -1, int8_t txPin = -1, bool invert = false,
                    unsigned long timeout_ms = 20000UL) {
    _serial.begin(baud, config, rxPin, txPin, invert, timeout_ms);
  }
  inline void end() { _serial.end(); }
  inline void updateBaudRate(unsigned long baud) {
    _serial.updateBaudRate(baud);
  }
  inline int available(void) { return _serial.available(); }
  inline int availableForWrite(void) { return _serial.availableForWrite(); }
  inline int peek(void) { return _serial.peek(); }
  inline int read(void) { return _serial.read(); }
  inline void flush(void) { return _serial.flush(); }
  inline size_t write(uint8_t c) {
    this->writeHandler(&c, 1);
    return _serial.write(c);
  }
  inline size_t write(const uint8_t* buffer, size_t size) {
    this->writeHandler(buffer, size);
    return _serial.write(buffer, size);
  }
  inline size_t write(const char* s) { return write((uint8_t*)s, strlen(s)); }
  inline size_t write(unsigned long n) { return write((uint8_t)n); }
  inline size_t write(long n) { return write((uint8_t)n); }
  inline size_t write(unsigned int n) { return write((uint8_t)n); }
  inline size_t write(int n) { return write((uint8_t)n); }
  inline uint32_t baudRate() { return _serial.baudRate(); }
  inline operator bool() const { return true; }

  inline size_t setRxBufferSize(size_t new_size) {
    return _serial.setRxBufferSize(new_size);
  }
  inline void setDebugOutput(bool en) { return _serial.setDebugOutput(en); }

 private:
  HardwareSerial& _serial;
  WriteFunction writeHandler;
};

#endif
