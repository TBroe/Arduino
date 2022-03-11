/*
This example creates a client object that connects and transfers
data using always SSL.

It is compatible with the methods normally related to plain
connections, like client.connect(host, port).

Written by Arturo Guadalupi
last revision November 2015

*/

#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

StaticJsonDocument<192> doc;

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "www.transport.opendata.ch";    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");  printWiFiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  lcd.init();
  lcd.clear();         
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("tram");
  lcd.setCursor(0,1);
  lcd.print("8");
  lcd.setCursor(5,0);
  lcd.print("time");
  lcd.setCursor(11,0);
  lcd.print("delay");
  

}

void loop() {
  WiFiSSLClient client;
  if (client.connect(server, 443)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /v1/connections?from=Klusplatz&to=Bellevue&limit=1&fields[]=connections/from/departure HTTP/1.1");
    client.println("Host: www.transport.opendata.ch");
    client.println("Connection: close");
    client.println();
    delay(5000);
       
    
  }
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    String res = client.readStringUntil('\n');
    if (res.startsWith("{") ) {
      //Serial.print(res);
      
    DeserializationError error = deserializeJson(doc, res);
    
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    const char* departure_ = doc["connections"][0]["from"]["departure"];
    String departure = String(departure_);
    String time_only = departure.substring(11,16);
    String delay_ = departure.substring(19,22);
    Serial.print(time_only);
    lcd.setCursor(5,1);   //Set cursor to character 2 on line 0
    lcd.print(time_only);
    lcd.setCursor(11,1);   //Set cursor to character 2 on line 0
    lcd.print(delay_);
    }
    }


  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
    delay(60000); //wait for one minute
    // do nothing forevermore:
    //while(true);  
    //StaticJsonDocument<200> filter;
    //filter["connections"][0]["from"]["station"]["departure"] = true;
    // Deserialize the document
    //StaticJsonDocument<400> doc;
  }
  delay(60000);
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  }
