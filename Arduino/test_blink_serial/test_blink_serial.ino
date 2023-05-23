
int led = 9 ; 
int brightness = 0 ; 
int fadeAmount = 1 ; 

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); 
  Serial.begin(9600); 
  pinMode(led, OUTPUT); 
}

// the loop function runs over and over again forever
void loop() {
  analogWrite(led, brightness);  
  brightness += fadeAmount;
  if (brightness >= 255 || brightness <= 0) {
    fadeAmount = -fadeAmount ;
  }
  delay(500); 

  int sensorValue = analogRead(A0); 
  Serial.println(sensorValue); 
  
}
