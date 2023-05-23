int trigPin = 17; 
int echoPin = 16; 

void setup() {
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  Serial.begin(9600); 
}

void loop() {
  unsigned long d = ping()/58; // Calculate distance
  Serial.print("Closest object: ");
  Serial.print(d); 
  Serial.println("cm"); 
  delay(1000); 
}

unsigned long ping(){
  digitalWrite(trigPin, HIGH);  // create 10 microseconds duration pulse 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 
  return pulseIn(echoPin, HIGH); // pulseIn function is built-in. Calculate the duration from LOW to HIGH at echoPin. 
}
