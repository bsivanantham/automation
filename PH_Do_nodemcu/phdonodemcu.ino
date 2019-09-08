#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <FirebaseArduino.h>
#include <Adafruit_ADS1015.h>

#define Offset 0.00 //deviation compensate
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth 40 //times of collection

#define FIREBASE_HOST "ldr-value.firebaseio.com"
#define WIFI_SSID "Robic Rufarm"
#define WIFI_PASSWORD "Rr@201920"

Adafruit_ADS1115 ads(0x48);

const float multiplier = 0.0001875F;
float read_do_zero;
float read_do_air;
float do_volt;
float do_perct;

int i,n=3;

int pHArray[ArrayLenth]; //Store the average value of the sensor feedback
int pHArrayIndex=0;

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
    delay(1000);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST);
}

void loop() 
{
  calibrate_do();
  get_ph_val();

  if (Firebase.failed()) {
    Serial.println("Firebase Pushing /sensor/failed:");
    Serial.println(Firebase.error()); 
    }
  else{
      Serial.print("Firebase Pushed /sensor/ ");
      }
delay(2000);
}

void get_ph_val(){
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue,voltage;
  if(millis()-samplingTime > samplingInterval){
    pHArray[pHArrayIndex++]=ads.readADC_SingleEnded(0);
    if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
    voltage = (avergearray(pHArray, ArrayLenth)*0.1875)/1000;
    pHValue = (12.571429 * voltage) - 17.571429;
    Serial.print("Voltage value ="); Serial.println(voltage);
    Serial.print(" pH value: ");
    Serial.println(pHValue,2);
    if(pHValue > 0 && pHValue< 14)
    {
      Firebase.setFloat ("/devices/12334/pH", pHValue);
    }
    else
    {
      Firebase.setString("/devices/12334/pH", "calibrating...");
    }
    printTime=millis();
  }  
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
  if(volt_new > 0)
  {
    Firebase.setFloat ("/devices/12334/do", volt_new);
  }
  {
    Firebase.setString("/devices/12334/dp", "calibrating...");
  }
  delay(100);
}

void calibrate_do(){
  read_do_zero = 0.21 ;
  read_do_air =  1.19;
  delay(100);
  get_do_val();
}

double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){ //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
    min = arr[0];max=arr[1];
  }
  else{
    min=arr[1];max=arr[0];
  }
  for(i=2;i<number;i++){
    if(arr[i]<min){
      amount+=min; //arr<min
      min=arr[i];
    }else {
      if(arr[i]>max){
        amount+=max; //arr>max
        max=arr[i];
      }else{
        amount+=arr[i]; //min<=arr<=max
      }
    }
  }
  avg = (double)amount/(number-2);
 }
 return avg;
}
