int inPin = 18 ; 
int val ; 

void setup() {
  Serial.begin(9600) ; 
  pinMode(inPin, INPUT); 
  Serial.println("Infra-red ready!"); 
}

void loop() {
  val = digitalRead(inPin);
  if (val == HIGH) {
    Serial.println("Somebody is moving!!!") ; 
  } 
  delay(500) ; 
}
