#include <LiquidCrystal.h>

int encoderPin1 = 8;
int encoderPin2 = 9;
 
volatile int lastEncoded = 0;
volatile long encoderValue = 0;
 
long lastencoderValue = 0;
 
int lastMSB = 0;
int lastLSB = 0;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin (9600);
 
  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);
 
  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on
 
  attachInterrupt(0, updateEncoder, CHANGE); 
  attachInterrupt(1, updateEncoder, CHANGE);
  
  lcd.begin(16, 2);
  lcd.clear();
}

void loop() {
  lcd.setCursor(0,0); // Sets the cursor to col 0 and row 0
  lcd.print("EncoderValue: "); // Prints Sensor Val: to LCD
  lcd.print(encoderValue); // Prints value on Potpin1 to LCD
  lcd.setCursor(0,1); // Sets the cursor to col 1 and row 0
  lcd.print("KeyPadValue: "); // Prints Sensor Val: to LCD
  lcd.print(encoderValue); // Prints value on Potpin1 to LCD
  delay(1000);
}

void updateEncoder(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit
 
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
 
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;
 
  lastEncoded = encoded; //store this value for next time
}
