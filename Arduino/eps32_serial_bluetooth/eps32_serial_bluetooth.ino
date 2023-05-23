#include <BluetoothSerial.h>
BluetoothSerial SerialBT; 

void setup() {
  Serial.begin(115200); 
  SerialBT.begin("ESP32_BT_1.2"); 
  //SerialBT.begin("ESP32_BT"); 
}

void loop() {
  // receive Serial from microUSB
  if (Serial.available()){
    SerialBT.write(Serial.read()); 
  }
  
  // receive Serial from BlueTooth
  if (SerialBT.available()){
    String s = SerialBT.readString(); 
    if (s == "Hello") {
      SerialBT.println("OK, I am alive"); 
    }
    else {
      Serial.println(s); 
    }
  }

  
  
  delay(50); 
}
