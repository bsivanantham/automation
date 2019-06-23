#include <ESP8266WiFi.h>
#include<ArduinoJson.h>
#include <FirebaseArduino.h>
#include <Adafruit_ADS1015.h>
/// I2C ADC EXTENDER///////////////////////
Adafruit_ADS1115 ads;

const float multiplier = 0.0001875F;

float pH_val;
float m;
float c;
float read_do_zero;
float read_do_air;
float do_volt;
float do_perct;


int i,n=3;

float stds[3] = {4 , 7 , 9.2};


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
  calibrate();
  }

void calibrate(){
 float sig_x = 0 , sig_y = 0 , sig_xy = 0 , sig_x2 = 0;
 float volts[3];
 volts[0] = 3.45 ;
 volts[1] = 2.38 ;
 volts[2] = 1.40 ;
  for(i=0;i<n;i++){
     sig_y = sig_y + volts[i];
     sig_x = sig_x + stds[i];
     sig_x2 = sig_x2 + (stds[i] * stds[i]);
     sig_xy = sig_xy + (stds[i] * volts[i]);
  }
  float m_num = (n * sig_xy) - (sig_x * sig_y);
  float denom = (n * sig_x2) - (sig_x * sig_x);
  m = m_num / denom;

  float c_num = (sig_x2 * sig_y) - (sig_x * sig_xy);
  float denom1 = (n * sig_x2) - (sig_x * sig_x);
  c = c_num / denom1;
  Serial.println("m ,c values : \t");
  Serial.print(m);
  Serial.print("\t\t");
  Serial.print(c);
  delay(2000);
  get_pH_val();
  
}

void get_pH_val(){
  Serial.println("\n\n");
  Serial.println("Place the probe in the solution...\n");
  delay(10000);
  Serial.println("Wait for voltage to stabilize...\n");
  delay(60000);
  float read_pH_volt = ads.readADC_SingleEnded(0) * multiplier ;
  float pH_val1 = read_pH_volt - c;
  pH_val = pH_val1 / m;
  Serial.print("The pH value is : \t");
  Serial.println(pH_val);
}
