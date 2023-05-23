int water_sensor = 36; 
int val; 

void setup() {
  Serial.begin(9600); 
  pinMode(water_sensor, INPUT); 
  Serial.println("Water sensor ready!");
}

void loop() {
  val = analogRead(water_sensor); 
  if (val > 2500) {
    Serial.println("Water level is too high: " + (String) val); 
  } else if (val > 800) {
    Serial.println("Water level is medium: "  + (String) val); 
  } else {
    Serial.println("Water level is low: "  + (String) val); 
  }
  delay(300); 
}
