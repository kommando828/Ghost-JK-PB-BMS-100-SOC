// JKModbusSlave.h
// Inspired by the ModbusRTUSlave library by C. M. Bulliner
// https://github.com/CMB27/ModbusRTUSlave

#ifndef JKModbusSlave_h
#define JKModbusSlave_h

#define MODBUS_RTU_SLAVE_BUF_SIZE 1024
#define NO_DE_PIN 255
#define NO_ID     0

#define RESPONSE_ERROR          -3
#define RESPONSE_NOT_MY_ADDRESS -2
#define RESPONSE_NO_DATA        -1

#include "Arduino.h"
#ifdef __AVR__
  #include <SoftwareSerial.h>
#endif

class JKModbusSlave {
  public:
    JKModbusSlave(HardwareSerial& serial, uint8_t dePin = NO_DE_PIN);
    #ifdef __AVR__
    JKModbusSlave(SoftwareSerial& serial, uint8_t dePin = NO_DE_PIN);
    #endif
    #ifdef HAVE_CDCSERIAL
    JKModbusSlave(Serial_& serial, uint8_t dePin = NO_DE_PIN);
    #endif

    #ifdef ESP32
    void begin(uint8_t id, unsigned long baud, uint32_t config = SERIAL_8N1,
               int8_t rxPin = -1, int8_t txPin = -1, bool invert = false,
               bool listenOnly = false, bool printDebug = false, bool printPassthrough = false);
    #else
    void begin(uint8_t id, unsigned long baud, uint32_t config = SERIAL_8N1,
               bool listenOnly = false, bool printDebug = false, bool printPassthrough = false);
    #endif

    int16_t poll();

  private:
    HardwareSerial *_hardwareSerial;
    #ifdef __AVR__
    SoftwareSerial *_softwareSerial;
    #endif
    #ifdef HAVE_CDCSERIAL
    Serial_ *_usbSerial;
    #endif
    Stream  *_serial;

    uint8_t  _dePin;
    uint8_t  _buf[MODBUS_RTU_SLAVE_BUF_SIZE];
    uint16_t _numBytes = 0;
    uint8_t  _id;

    unsigned long _charTimeout;
    unsigned long _frameTimeout;
    #ifdef ARDUINO_ARCH_RENESAS
    unsigned long _flushCompensationDelay;
    #endif

    bool _JKBMS_LISTEN_ONLY;       // do not transmit on RS485
    bool _JKBMS_PRINT_DEBUG;       // print bus traffic in human-readable form
    bool _JKBMS_PRINT_PASSTHROUGH; // echo raw bus bytes to USB serial

    void _frame1();
    void _frame2();
    void _frame3();

    bool     _readRequest();
    void     _writeResponse(uint16_t len);
    void     _clearRxBuffer();

    void     _calculateTimeouts(unsigned long baud, uint32_t config);
    uint16_t _crc(uint16_t len);
    uint16_t _chksum(uint16_t len);
    uint16_t _div8RndUp(uint16_t value);
    uint16_t _bytesToWord(uint8_t high, uint8_t low);
};

#endif
