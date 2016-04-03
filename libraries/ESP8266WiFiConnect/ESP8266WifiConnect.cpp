#include "ESP8266WifiConnect.h"

void ESP8266WifiConnect::handleRoot()
{
  if (server.method() == HTTP_GET && !connected)
  {
    String s = "<html><body>Select from the AP listed below<br>";
    s += apHTMLtable;
    s += "<form method=\"POST\" action=\"/\">";
    s += "SSID:<br>";
    s += "<input type=\"text\" length=32 name=\"ssid\"><br>";
    s += "Password:<br>";
    s += "<input type=\"password\" length=32 name=\"pass\"><br>";
    s += "<input type=\"submit\">";
    s += "</form></body></html>\r\n\r\n";
    
    server.send(200, "text/html", s.c_str());
    delay(10);
    Serial.println("Sending 200");
  }
  else if (server.method() == HTTP_GET && connected)
  {
    String s = "<html><body>Connected to AP ";
    s += ssid;
    s += "</body></html>\r\n\r\n";
    
    server.send(200, "text/html", s.c_str());
    delay(10);
    Serial.println("Sending 200");
    
    finished = true;
  }
  else
  {
    ssid = server.arg("ssid");
    ssidLength = ssid.length();
    password = server.arg("pass");
    passLength = password.length();
    Serial.println(ssid);
    Serial.println(password);
    
    String s = "<html><meta http-equiv=\"refresh\" content=\"10;url=http://192.168.4.1/\"/>Trying to connect in 10 sec. to AP ";
    s += ssid;
    s += "</html>\r\n\r\n";
    
    server.send(200, "text/html", s.c_str());
    delay(10);
    Serial.println("Sending 200");
    
    connected = connectAP();
  }
}

ESP8266WifiConnect::ESP8266WifiConnect()
{
  // Init values
  numRetries = 10;
  ssidLength = 0;
  passLength = 0;
  ssid = "";
  password = "";
  apHTMLtable = "";
  server = ESP8266WebServer(80);
  finished = false;
  connected = false; 
}

void ESP8266WifiConnect::begin()
{
  // Open serial port for debug output
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  
  // Read EEPROM data
  EEPROM.begin(512);
  delay(10);
  readEEPROM();
  
  // Connect to the AP
  int r = 0;
  while (r < numRetries && !connected)
  {
    connected = connectAP();
    if(connected)
    {
      Serial.println("Connected!");
      break;
    }
    else
    {
      Serial.println("Not connected!");
      selectAP();
    }
    
    r++;
  }
}

bool ESP8266WifiConnect::connectAP()
{
  // Init return value
  bool result = false;
  
  if(ssidLength == 0)
    return result;
  
  // Connect to the AP
  Serial.print("Connecting to AP ");
  Serial.println(ssid);
  WiFi.begin(ssid.c_str(), password.c_str());
  
  int r = 0;
  while (r < numRetries)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      result = true;
      Serial.println("");
      Serial.println("WiFi connected");
      break;
    }
    
    r++;
    delay(500);
    Serial.print(".");
  }
  
  return result;
}

bool ESP8266WifiConnect::selectAP()
{
  // Scan for existing networks
  scanNetwork();
  
  // Start the server
  std::function<void()> fpHandleRoot = std::bind(&ESP8266WifiConnect::handleRoot, this);
  server.on("/", fpHandleRoot);
  server.begin();
  
  // Wait for a client to connect
  finished = false;
  while (!finished)
  {
    server.handleClient();
  }
  
  // Stop the server
  server.close();
  
  // Write SSID and password to the EEPROM
  writeEEPROM();
  
  return true;
}

void ESP8266WifiConnect::scanNetwork()
{  
  // Init return value
  apHTMLtable = "";
  
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  Serial.println("Setting WiFi mode to STA");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  // WiFi.scanNetworks will return the number of networks found
  Serial.println("Scanning network");
  int numNetworks = WiFi.scanNetworks();
  
  if(numNetworks > 0)
  {
    // Create a HTML table for each network found
    apHTMLtable += "<table>";
    apHTMLtable += "<tr>";
    apHTMLtable += "<th>SSID</th>";
    apHTMLtable += "<th>Encryption</th>";
    apHTMLtable += "<th>Signal strength (dBm)</th>";
    apHTMLtable += "</tr>"; 
    for (int i = 0; i < numNetworks; i++)
    {
      apHTMLtable += "<tr>";
      apHTMLtable += "<td>";
      apHTMLtable += WiFi.SSID(i);
      apHTMLtable += "</td>";
      apHTMLtable += "<td>";
      apHTMLtable += getEncryptionString(WiFi.encryptionType(i));
      apHTMLtable += "</td>";
      apHTMLtable += "<td>";
      apHTMLtable += WiFi.RSSI(i);
      apHTMLtable += "</td>";
      apHTMLtable += "</tr>";
    }
    apHTMLtable += "</table>";
  }
  else
    apHTMLtable = "No networks found!";
  
  // Set WiFi to soft AP mode 
  delay(100);
  Serial.println("Setting WiFi to Soft AP");
  WiFi.softAP(ssidAP);
}

String ESP8266WifiConnect::getEncryptionString(uint8_t encryptionType)
{
  String type;
  
  if (encryptionType == ENC_TYPE_WEP)
    type = "WEP";
  else if(encryptionType == ENC_TYPE_TKIP)
    type = "WPA2-TKIP";
  else if(encryptionType == ENC_TYPE_CCMP)
    type = "WPA2-AES";
  else if(encryptionType == ENC_TYPE_NONE)
    type = "None";
  else if(encryptionType == ENC_TYPE_AUTO)
    type = "Auto";
  else
    type = "Unknown";
  
  return type;
}

void ESP8266WifiConnect::readEEPROM()
{
  // Init iterator
  int it = 0;
  ssid = "";
  password = "";
  
  // Read the length variable of the SSID data
  ssidLength = byte(EEPROM.read(it));
  it++;
  
  // Read the SSID data
  for (int i = 0; i < ssidLength; i++)
  {
    ssid += char(EEPROM.read(it));
    it++;
  }
  
  // Read the length variable of the password data
  passLength = byte(EEPROM.read(it));
  it++;
  
  // Read the password datai
  for (int i = 0; i < passLength; i++)
  {
    password += char(EEPROM.read(it));
    it++;
  }
  
  Serial.print("ssid: ");
  Serial.println(ssid);
  Serial.print("password: ");
  Serial.println(password);
  Serial.print("ssid length: ");
  Serial.println(ssidLength);
  Serial.print("password length: ");
  Serial.println(passLength);
}

void ESP8266WifiConnect::writeEEPROM()
{
  // Init iterator
  int it = 0;
  
  // First clear the EEPROM
  clearEEPROM();
  
  // Write the length variable of the SSID data
  ssidLength = ssid.length();
  EEPROM.write(it, ssidLength);
  it++;
  Serial.print("ssidLength: ");
  Serial.println(ssidLength);
  
  // Write the SSID data
  for (int i = 0; i < ssidLength; i++)
  {
    EEPROM.write(it, ssid[i]);
    it++;
    Serial.println(ssid[i]);
  }
  
  // Write the length variable of the password data
  passLength = password.length();
  EEPROM.write(it, passLength);
  it++;
  Serial.print("passLength: ");
  Serial.println(passLength);
  
  // Write the password data
  for (int i = 0; i < passLength; i++)
  {
    EEPROM.write(it, password[i]);
    it++;
    Serial.println(password[i]);
  }
  
  // Commit
  EEPROM.commit();
  Serial.println("Finished writing EEPROM");
}

void ESP8266WifiConnect::clearEEPROM()
{
  int dataLength = ssidLength + passLength + 2;
  for (int i = 0; i < dataLength; ++i)
    EEPROM.write(i, 0);
  
  // Commit
  EEPROM.commit();
  Serial.println("Finished clearing EEPROM");
}
