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
#define WIFI_PASSWORD "Robicruf@rm12345"

Adafruit_ADS1115 ads;

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
    pHArray[pHArrayIndex++]=ads.readADC_SingleEnded(2) * multiplier;
    if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
    voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
    pHValue = 3.5*voltage+Offset;
    samplingTime=millis();
  }
  if(millis() - printTime > printInterval){ //Every 800 milliseconds, print a numerical, convert the state of the LED indicato
    Serial.print("Voltage:");
    Serial.print(voltage,2);
    Serial.print(" pH value: ");
    Serial.println(pHValue,2);
    Firebase.setFloat ("PHValue", pHValue);
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
  Firebase.setFloat ("DOValue", volt_new);
  delay(1000);
}

void calibrate_do(){
  read_do_zero = 0.21 ;
  read_do_air =  1.19;
  delay(1000);
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
