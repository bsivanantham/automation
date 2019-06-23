#include <ESP8266WiFi.h>
#include<ArduinoJson.h>
#include <FirebaseArduino.h>
#include <Adafruit_ADS1015.h>
/// I2C ADC EXTENDER///////////////////////
Adafruit_ADS1115 ads;

const float multiplier = 0.0001875F;

float pH_val;


#define FIREBASE_HOST "ldr-value.firebaseio.com"
#define WIFI_SSID "Robic Rufarm"
#define WIFI_PASSWORD "Robicruf@rm12345"

void setup()
{
  Serial.begin(115200);
    ads.setGain(GAIN_TWOTHIRDS);
  ads.begin();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST);
}

void loop() 
{
  get_pH_val();
  }

void get_pH_val(){
  Serial.println("\n\n");
  Serial.println("Place the probe in the solution...\n");
  delay(10000);
  Serial.println("Wait for voltage to stabilize...\n");
  delay(60000);
  float read_pH_volt = ads.readADC_SingleEnded(0) * multiplier ;
  Serial.print("The pH value is : \t");
  Serial.println(read_pH_volt);
}
