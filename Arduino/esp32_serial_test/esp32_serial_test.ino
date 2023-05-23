#include <SimpleDHT.h>

int pinDHT11 = 15; 
SimpleDHT11 dht; 
String s ; 
int ledpin = 2 ; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  pinMode(ledpin, OUTPUT); 
}

void loop() {
  // put your main code here, to run repeatedly:
  byte temp = 0 ; 
  byte humi = 0 ; 
  int err = SimpleDHTErrSuccess; 
  Serial.println("Strat to poll sensor DHT11...") ; 
  err = dht.read(pinDHT11, &temp, &humi, NULL);
  if (err != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); 
    Serial.println(err); 
    delay(1000); 
    return; 
  } 
  else {
    Serial.println("Successfully read:");
    digitalWrite(ledpin, HIGH); 
    Serial.print("Humi = ") ; 
    Serial.print((float)humi); 
    Serial.print("%, Temp = "); 
    Serial.print((float)temp); 
    Serial.println("C "); 
  }
  delay(1000); 
  digitalWrite(ledpin, LOW); 
  delay(1000); 
  
}
