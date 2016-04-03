// ensure this library description is only included once
#ifndef ESP8266WIFICONNECT_H
#define ESP8266WIFICONNECT_H

#include "ESP8266WiFi.h"
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>

class ESP8266WifiConnect
{
  public:
    ESP8266WifiConnect();
    void begin();
    void handleRoot();
    
  private:
    int numRetries;
    byte ssidLength;
    byte passLength;
    ESP8266WebServer server;
    String ssid;
    String password;
    String apHTMLtable;
    bool finished;
    bool connected;
    const char* ssidAP = "WifiConnect";

    bool connectAP();
    bool selectAP();
    void scanNetwork();
    String getEncryptionString(uint8_t encryptionType);
    void readEEPROM();
    void writeEEPROM();
    void clearEEPROM();
};

#endif //ESP8266WIFICONNECT_H