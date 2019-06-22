#include <ESP8266WiFi.h>
#include<ArduinoJson.h>
#include <FirebaseArduino.h>
#include <Adafruit_ADS1015.h>
/// I2C ADC EXTENDER///////////////////////
Adafruit_ADS1115 ads;

const float multiplier = 0.0001875F;
float read_do_zero;
float read_do_air;
float do_volt;
float do_perct;


int i,n=3;

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
  calibrate_do();
  }

void get_do_val(){
  do_volt = ads.readADC_SingleEnded(1) * multiplier;
  float volt_100 = read_do_air - read_do_zero;
  float volt_new = do_volt - read_do_zero ; 
  float do_num = volt_new * 100;
  float do_perct = do_num / volt_100 ; 
  Serial.print("The Do2 percentage is : ");
  Serial.print(abs(do_perct));
  Serial.print("\t Do2 value is : ");
  Serial.println(abs(volt_new));
  Serial.print("\t Raw Do2 value is : ");
  Serial.println(abs(do_volt));
  delay(1000);
}


void calibrate_do(){
  read_do_zero = 0.21 ;
  read_do_air =  1.19;
  delay(1000);
  get_do_val();
}
