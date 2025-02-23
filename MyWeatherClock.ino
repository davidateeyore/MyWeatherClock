//  MyWeatherClock.ino
//  dt 01DEC2024.. fancy up my CYD clock!!
//  add barosensor lib from Freetronics
//  try sparkfun MS5637 lib - barosensor doesn't work with ESP32!! dt 03DEC2024
//
//  add humidity detector?? dt 06DEC2024
//  DHT22???

#include "DHT.h"
#include <WiFi.h>
#include "time.h"
#include "esp_sntp.h"
#include <TFT_eSPI.h>
#include <Wire.h>
#include "SparkFun_MS5637_Arduino_Library.h"

// custom I2C ports for ESP32 on CN1?..
#define I2C_SDA 27
#define I2C_SCL 22
// IO for DHT11
#define DHTPIN 17 // on a spare connector for CYD.. used for blue rgb led.. 
//
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// instantiate baro sensor..
MS5637 BaroSensor;
    float pressure;
    float temperature;
// instantiate humidity sensor..
DHT HumSensor(DHTPIN, DHTTYPE);    


// A library for interfacing with LCD displays
//
// Can be installed from the library manager (Search for "TFT_eSPI")
//https://github.com/Bodmer/TFT_eSPI

TFT_eSPI tft = TFT_eSPI();
  int x = 5;
  int y = 10;
  //int fontNum = 2;
  int fontNum = 3;

const char *ssid = "xxxx";
const char *password = "yyyyyyyy";
// won't connect to hidden SSIDs... from arduino.cc reference pages.

const char *ntpServer1 = "10.28.174.11";  // when cottage is back on line..
const char *ntpServer2 = "au.pool.ntp.org";
//
const char *ntpServer3 = "time.nist.gov";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

const char *time_zone = "AEST-10AEDT,M10.1.0,M4.1.0/3";  // TimeZone rule for Melbourne, AU including daylight adjustment rules (optional)
IPAddress ip;                    // the IP address of your device...
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
void printHMS() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  Serial.println(&timeinfo, "%H:%M:%S");
}
// Callback function (gets called when time adjusts via NTP)
void timeavailable(struct timeval *t) {
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}
void ShowHMS() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    tft.println("pls wait...)");
    return;
  }
   tft.println(&timeinfo, "%H:%M:%S");
}
void ShowDMY() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    tft.println("pls wait...)");
    return;
  }
   tft.setTextSize(3);
   tft.println(&timeinfo, "%A,   "); // extra spaces to flush longer day names! dt 01NOV2024
   tft.println(&timeinfo, " %d %B %Y" );
}

// SETUP starts here!!!!

void setup() {
  Serial.begin(9600);
  HumSensor.begin();
  Wire.begin(I2C_SDA, I2C_SCL); // it really helps to tell wire.begin if you change from the default i2c pins!! dt 03DEC2024
  if (BaroSensor.begin() == false)
  {
    Serial.println("MS5637 sensor did not respond. Please check wiring.");
  }
  // Start the tft display and set it to black
  tft.init();
  tft.setRotation(1); //This is the display in landscape

  // Clear the screen before writing to it
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(5, 5);
  tft.setTextSize(2);
  tft.println("My Weather Clock!!"); // Left Aligned
//
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10,35);
  tft.println("The Temp is: ");
//
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10,55);
  tft.println("Barometer is: ");
//  
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10,75);
  tft.println("Humidity is: ");
 

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawCentreString("The Time is:", x, y, fontNum);
  // now for the time connect..
  Serial.begin(115200);
  // First step is to configure WiFi STA and connect in order to get the current time and date.
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);

  /**
   * NTP server address could be acquired via DHCP,
   *
   * NOTE: This call should be made BEFORE esp32 acquires IP address via DHCP,
   * otherwise SNTP option 42 would be rejected by default.
   * NOTE: configTime() function call if made AFTER DHCP-client run
   * will OVERRIDE acquired NTP server address
   */
  esp_sntp_servermode_dhcp(1);  // (optional)

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  Serial.println(" CONNECTED");
  //print the local IP address
  ip = WiFi.localIP();
  Serial.println(ip);

  // set notification call-back function
  sntp_set_time_sync_notification_cb(timeavailable);

  /**
   * This will set configured ntp servers and constant TimeZone/daylightOffset
   * should be OK if your time zone does not need to adjust daylightOffset twice a year,
   * in such a case time adjustment won't be handled automagically.
   */
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  /**
   * A more convenient approach to handle TimeZones with daylightOffset
   * would be to specify a environment variable with TimeZone definition including daylight adjustmnet rules.
   * A list of rules for your zone could be obtained from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
   */
  configTzTime(time_zone, ntpServer1, ntpServer2, ntpServer3);
}

void loop() {

  float temperature = BaroSensor.getTemperature();
  float pressure = BaroSensor.getPressure();
  float humidval = HumSensor.readHumidity(); 
    Serial.println();
    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.print(" Pressure:    ");
    Serial.print(pressure, 3);
    Serial.println();
    Serial.print(" Humidity:    ");
    Serial.print(humidval, 3);
    Serial.println();
    //
  tft.setTextSize(2);  
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(170,35);  
  tft.print(temperature, 1);
  tft.println(" deg.C");
  //
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(170,55);
  tft.print(pressure, 2);
  tft.println(" mBar");
  //
  tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK);
  tft.setCursor(170,75);
  tft.print(humidval);
  tft.println(" %");
  // 
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(6);
  delay(1000);
  //
  // position clock in middle of screen..
  tft.setCursor(15,125);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  // draw text??
  ShowHMS();  // show time on screen
  tft.setCursor(15,185);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  ShowDMY();  // show day, date..
  // serial check of time..
  printHMS();  // it will take some time to sync time :)
}
