int light_sensor = 4 ; 
int val; 

void setup() {
  pinMode(light_sensor, INPUT); 
  Serial.begin(9600); 
}

void loop() {
  val = analogRead(light_sensor); 
  Serial.print("light_sensor got value: "); 
  Serial.println(val);
  delay(1000); 
}
