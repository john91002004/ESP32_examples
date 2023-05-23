/**** Temperature and Humidity Sensors ****/
// Designed by Central Weather Bureau MSC (中央氣象局衛星中心)
// This program is implemented and was tested on Arduino Uno board.
// It functions as a sensor of temperature and humidity through two different components.
// It can display its sensor data on LCD.
// It can transmit its data through Ethernet to any other terminal within the same LAN.
// While using Ethernet to transmit data under the communication protocol N8.1.
// Whole module requires: 
//    -Microprocessor: Arduino Uno R3 *1
//    -Ethernet Board: W5100 *1
//    -Adaptor: 9V/1A DC *1
//    -Resistors: 220ohm *1 / 330ohm *1 / 4.7kohm *1
//    -LCD:  1602A *1 
//    -Temperature Sensor: DS18B20 *1
//    -Humidity Sensor: DHT11 *1   (actually DHT11 is double-duty for temperature and humidity, but its temperature resolution is not enough)
//    -Buttons*3 
//    -Other materials: Pin headers, Dupont lines, Hexagon copper pillars, Screws and Screw nuts. (depends on the circuits you design)
/* Library included */
#include <DHT.h>
#include <LiquidCrystal.h>       //調用Arduino自帶的LiquidCrystal庫
#include <OneWire.h>      //調用Arduino單總線庫
#include <DallasTemperature.h>  //調用ds18b20溫度傳感器庫
#include <SPI.h>        // ethernet library
#include <Ethernet.h>
/* Time parameters */        
#define LONG_PRESS 500    // long press holding time
#define FLASH_PERIOD 40    // flash period while adjusting 
/* Mode encoded in binary */
#define DISPLAY_MODE0 B0000           // display temperature and humidity   
#define IP_DISPLAY_MODE B1000
#define IP_ADJUST_MODE0 B0100         // ip adjust     
#define IP_ADJUST_MODE1 B0101
#define IP_ADJUST_MODE2 B0110
#define IP_ADJUST_MODE3 B0111     
#define ADDRESS_DISPLAY_MODE B1100
#define ADDRESS_ADJUST_MODE0 B1101     // address adjust 
#define MAC_DISPLAY_MODE0 B0010
#define MAC_ADJUST_MODE0 B1010
#define SERVERPORT_DISPLAY_MODE0 0011
#define SERVERPORT_ADJUST_MODE0 1011
//#define TEST_MODE B1111
/* Button and Sensor inputs */
#define ONE_WIRE_BUS 2    // Dallas DS18b20 data input pin
#define button_A A1         
#define button_B A2
#define button_C A0
#define dht_pin  A5       // DHT11 input pin
/* Ethernet settings */
byte ip[] = {192,168,178,49} ;
byte pre_ip[] = {192,168,178,49} ; 
byte mac[] = { 0xB8, 0xCA, 0x3A, 0xB5, 0xDF, 0xE3 };  // 192.168.178.49 eht0 mac
byte pre_mac[] = { 0xB8, 0xCA, 0x3A, 0xB5, 0xDF, 0xE3 };  // 192.168.178.49 eht0 mac
int address = 0x01;             // address initialize 
int ServerPort = 9000; 
int pre_ServerPort = 9000 ; 
/* Data parameters */
byte data[] = {0x01,0x03,0x04,0x00,0xfd,0x02,0x4e,0xea,0x97} ;
int buf_len = 9 ;
int state = DISPLAY_MODE0 ;     // mode initialize
bool sensor_state = true ; 
float _temperature , _humidity ;
/* Some temperary parameters */
int R[8] ; 
int i = 0, j=0; // i for counting, j for R[8]
int flash_count = 0 ;
/* Initial setting */
OneWire temp(ONE_WIRE_BUS);  //初始化一個單總線類，以3號引腳作為輸出口
LiquidCrystal lcd(9,8,7,6,5,3);
DallasTemperature sensors(&temp); //初始化一個溫度傳感器類。
EthernetServer server(ServerPort);
DHT dht(dht_pin,DHT11) ; // for DHT11
/* Main */
void setup() {
  pinMode(button_A,INPUT_PULLUP) ; 
  pinMode(button_B,INPUT_PULLUP) ;
  pinMode(button_C,INPUT_PULLUP) ;
  Ethernet.begin(mac, ip);
  server.begin();
  lcd_init();
  sensor_init() ;        //
}

void loop() {
  i = 0 ; 
  check_ethernet_update() ; 
  switch (state)  {
//state display
    case DISPLAY_MODE0: 
      sensor_display();
      if (get_button(button_B) && get_button(button_A)) {
        while (get_button(button_B)|get_button(button_A)) {i++; delay(1) ; }
        sensor_state = !sensor_state ; 
      }
      if (get_button(button_C) && sensor_state == false) {
         while (get_button(button_C)) {i++ ; delay(1) ;}
         state = IP_DISPLAY_MODE ;
        }
      break ; 
/* Activated when sensors off  */
// state ip display
    case IP_DISPLAY_MODE: 
      ip_display(-1,flash_count);
      if (get_button(button_C)) {
        while (get_button(button_C)) {i++ ; delay(1) ;}
        if (i > LONG_PRESS) { state = IP_ADJUST_MODE0 ;  flash_count = 0; break ; }
        else { state = ADDRESS_DISPLAY_MODE ; flash_count = 0 ; break ; }
        }
      break ; 
// state ip adjust 
    case IP_ADJUST_MODE0: 
      flash_count++ ; 
      ip_display(0,flash_count);
      if (flash_count > FLASH_PERIOD ) { flash_count = 0 ; }
      if (get_button(button_C)) {
        while (get_button(button_C)) {i++ ; delay(1) ;}
        if (i > LONG_PRESS) { state = IP_DISPLAY_MODE ;  break ; }
        else { state = IP_ADJUST_MODE1 ;  break ; }
        }      
      break ; 
    case IP_ADJUST_MODE1: 
      flash_count++ ; 
      ip_display(1,flash_count);
      if (flash_count > FLASH_PERIOD ) {  flash_count = 0 ; }
      if (get_button(button_C)) {
        while (get_button(button_C)) {i++ ; delay(1) ;}
        if (i > LONG_PRESS) { state = IP_DISPLAY_MODE ;  break ; }
        else { state = IP_ADJUST_MODE2 ;  break ; }
        }  
      break ; 
    case IP_ADJUST_MODE2:
      flash_count++ ; 
      ip_display(2,flash_count);
      if (flash_count > FLASH_PERIOD ) {  flash_count = 0 ; }
      if (get_button(button_C)) {
        while (get_button(button_C)) {i++ ; delay(1) ;}
        if (i > LONG_PRESS) { state = IP_DISPLAY_MODE ;  break ; }
        else { state = IP_ADJUST_MODE3 ;  break ; }
        }      
      break ; 
    case IP_ADJUST_MODE3: 
      flash_count++ ; 
      ip_display(3,flash_count);
      if (flash_count > FLASH_PERIOD ) {  flash_count = 0 ; }
      if (get_button(button_C)) {
        while (get_button(button_C)) {i++ ; delay(1) ;}
        if (i > LONG_PRESS) { state = IP_DISPLAY_MODE ;  break ; }
        else { state = IP_ADJUST_MODE0 ;  break ; }
        }      
      break ;             
// state address display 
    case ADDRESS_DISPLAY_MODE: 
      address_display(-1,flash_count);
      if (get_button(button_A)&&get_button(button_B)) {
        while (get_button(button_A)&&get_button(button_B)) {i++ ; delay(1) ;}
        if (i > LONG_PRESS) { state = MAC_DISPLAY_MODE0 ;  break ; }
        break ;
        }
      if (get_button(button_C)) {
        while (get_button(button_C)) {i++ ; delay(1) ;}
        if (i > LONG_PRESS) { state = ADDRESS_ADJUST_MODE0 ;  break ; }
        else { state = DISPLAY_MODE0 ; break ; }
        }
      break ; 
// state address adjust
    case ADDRESS_ADJUST_MODE0: 
      flash_count++ ; 
      address_display(0,flash_count);
      if (flash_count > FLASH_PERIOD ) {  flash_count = 0 ; }
      if (get_button(button_C)) {
        while (get_button(button_C)) {i++ ; delay(1) ;}
        if (i > LONG_PRESS) { state = ADDRESS_DISPLAY_MODE ;  break ; }
        }     
      break ;     
/* Hidden Mode */
// state mac[5] adjust
    case MAC_DISPLAY_MODE0: 
      flash_count++ ; 
      mac_display(1,flash_count);
      if (get_button(button_A)&&get_button(button_B)) {
        while (get_button(button_A)&&get_button(button_B)) {i++ ; delay(1) ;}
        if (i > LONG_PRESS) { state = ADDRESS_DISPLAY_MODE ;  break ; }
        break ;
        }
      if (flash_count > FLASH_PERIOD ) {  flash_count = 0 ; }
      if (get_button(button_C)) {
        while (get_button(button_C)) {i++ ; delay(1) ;}
        if (i > LONG_PRESS) { state = MAC_ADJUST_MODE0 ;  break ; }
        else {state = SERVERPORT_DISPLAY_MODE0; break ; }
        }     
      break ;  
    case MAC_ADJUST_MODE0: 
      flash_count++ ; 
      mac_display(0,flash_count);
      if (flash_count > FLASH_PERIOD ) {  flash_count = 0 ; }
      if (get_button(button_C)) {
        while (get_button(button_C)) {i++ ; delay(1) ;}
        if (i > LONG_PRESS) { state = MAC_DISPLAY_MODE0 ;  break ; }
        }     
      break ;  
// state server port adjust
    case SERVERPORT_DISPLAY_MODE0: 
      flash_count++ ; 
      serverport_display(1,flash_count);
      if (flash_count > FLASH_PERIOD ) {  flash_count = 0 ; }
      if (get_button(button_C)) {
        while (get_button(button_C)) {i++ ; delay(1) ;}
        if (i > LONG_PRESS) { state = SERVERPORT_ADJUST_MODE0 ;  break ; }
        else {state = MAC_DISPLAY_MODE0; break ; }
        }     
      break ;  
    case SERVERPORT_ADJUST_MODE0: 
      flash_count++ ; 
      serverport_display(0,flash_count);
      if (flash_count > FLASH_PERIOD ) {  flash_count = 0 ; }
      if (get_button(button_C)) {
        while (get_button(button_C)) {i++ ; delay(1) ;}
        if (i > LONG_PRESS) { state = SERVERPORT_DISPLAY_MODE0 ;  break ; }
        }     
      break ;  
    default:  
      break ; 
    }
  if (sensor_state == true ) sensor_on () ; 
  data_update() ; 
    EthernetClient client = server.available();
    if (client) ethernet_ISR() ;     
}

/**** Functions  ****/ 
bool get_button (int button) {    
  if (digitalRead(button) == 0) return true ; 
  else return false ;
} 
void sensor_on () {
  // Temperature 
  sensors.requestTemperatures();  //對傳感器進行一個溫度請求
  _temperature = sensors.getTempCByIndex(0);  //讀取傳感器輸出的數據，以攝氏度為單位賦值給temperature變量。  
  // Humidity
  _humidity = dht.readHumidity() ; 
  } 
void lcd_init() {
  lcd.begin(16, 2);       //初始化LCD
  lcd.clear() ; 
  lcd.print(" Intializing... ");   //使屏幕顯示文字
  delay(1000) ; 
} 
void sensor_init () {
  sensors.begin() ;
  dht.begin() ; 
}
void sensor_display () {
  lcd.setCursor(0,0) ; 
  lcd.print("    Temp ") ;
  lcd.print((long)(_temperature*10)/10) ;  
  lcd.print(".") ; 
  lcd.print((long)(_temperature*10)%10) ; 
  lcd.print((long)(_temperature*100)%10) ;  
  lcd.print((char)223);      //顯示o符號
  lcd.print("C");          //顯示字母C 
  lcd.setCursor(0,1) ;
  lcd.print("Humidity ") ;
  lcd.print((long)_humidity/1) ;  
  lcd.print(".") ; 
  lcd.print((long)(_humidity*10)%10) ;  
  lcd.print((long)(_humidity*100)%10) ; 
  lcd.print(" %");          //顯示字母C 
}

/* sensor data trnsmission in N8.1 */
void data_update () {
  int CRC = 0xffff ; 
  int Function = 0x03,Data_count = 0x04 ;
  int Temp_Hi = ((long)(_temperature*10)/1) >> 8 ; 
  int Temp_Low = (long)(_temperature*10)/1 ; 
  int Humi_Hi = ((long)(_humidity*10)/1) >> 8 ; 
  int Humi_Low = (long)(_humidity*10)/1 ;   
  CRC = CRC16(address,CRC) ;
  CRC = CRC16(Function,CRC) ; 
  CRC = CRC16(Data_count,CRC) ;; 
  CRC = CRC16(Temp_Hi,CRC) ;
  CRC = CRC16(Temp_Low,CRC) ;  
  CRC = CRC16(Humi_Hi,CRC); 
  CRC = CRC16(Humi_Low,CRC) ; 
  data[0] = address ; 
  data[1] = Function ; 
  data[2] = Data_count ; 
  data[3] = Temp_Hi ; 
  data[4] = Temp_Low ; 
  data[5] = Humi_Hi ; 
  data[6] = Humi_Low ; 
  data[7] = CRC ; 
  data[8] = CRC >> 8 ; 
}

/* CRC-16 Algorithm (Arduino version, slicely different from C++) */
int CRC16 (int data,int CRC) {
  int _CRC ; 
  _CRC = CRC  ^ (data & 0x00ff); 
  for (int i =0;i<8;i++){
    if((_CRC & 0x0001) == 0x0001 ) {
      _CRC = (_CRC >> 1) & 0x7fff ; 
      _CRC = _CRC ^ 0xa001 ; 
    }
    else if ((_CRC & 0x0001) == 0x0000) {
      _CRC = (_CRC >> 1) & 0x7fff ;
    }
  }
  return _CRC ; 
}

/* display functions */
void ip_display(int adjust_mode, int flash_count) {     // 0~3 adjust mode , elseothers display mode 
  lcd.setCursor(0,0) ;
  lcd.print("Sensor IP:      ") ;
  lcd.setCursor(0,1) ; 
  for (int i=0;i<4;i++) {
    if (i == adjust_mode && flash_count > FLASH_PERIOD/2) {         
      if (get_button(button_A)) {while (get_button(button_A)) ;  if (++ip[i] > 255) ip[i] = 0 ;}
      if (get_button(button_B)) {while (get_button(button_B)) ; if (--ip[i] < 0 ) ip[i] = 255;}  
      lcd.print("   ") ;
    }
    else {
      if (ip[i] < 100) lcd.print("0") ;
      if (ip[i] < 10) lcd.print("0") ; 
      lcd.print(ip[i]) ;
    }
    if (i!=3) lcd.print(".") ;    
  }
  lcd.print(" ") ; 
  return ; 
}
void address_display(int adjust_mode , int flash_count) {// 0 adjust , elseothers display mode
  lcd.setCursor(0,0) ;
  lcd.print("Sensor address: ") ;
  lcd.setCursor(0,1) ; 
  if (adjust_mode == 0 && flash_count > FLASH_PERIOD/2 ){
    if (get_button(button_A)) {while (get_button(button_A)) ;if (++address > 99) address = 1  ;}
    if (get_button(button_B)) {while (get_button(button_B)) ;if (--address < 1) address = 99  ;}
    lcd.print("  ") ;
  }
  else {
    if (address < 10) lcd.print("0") ; 
    lcd.print(address) ; 
  }
  lcd.print("              ") ; 
  return ;  
}
void mac_display(int adjust_mode , int flash_count) {// 0 adjust , elseothers display mode
  lcd.setCursor(0,0) ;
  lcd.print("Sensor MAC:     ") ;
  lcd.setCursor(0,1) ; 
  for (int k=0 ; k<5;k++) {
    if (mac[k] < 16) lcd.print("0") ; 
    lcd.print(mac[k],HEX); 
  }
  if (adjust_mode == 0 && flash_count > FLASH_PERIOD/2 ){
    if (get_button(button_A)) {while (get_button(button_A)) ;if (++mac[5] > 255) mac[5] = 0  ;}
    if (get_button(button_B)) {while (get_button(button_B)) ;if (--mac[5] < 0) mac[5] = 255  ;}
    lcd.print("  ") ;
  }
  else {
    if (mac[5] < 16) lcd.print("0") ; 
    lcd.print(mac[5],HEX) ; 
  }
  lcd.print("    ") ; 
  return ;  
}
void serverport_display(int adjust_mode , int flash_count) {// 0 adjust , elseothers display mode
  lcd.setCursor(0,0) ;
  lcd.print("Server Port:    ") ;
  lcd.setCursor(0,1) ; 
  if (adjust_mode == 0 && flash_count > FLASH_PERIOD/2 ){
    if (get_button(button_A)) {while (get_button(button_A)) ;if (++ServerPort > 10000) ServerPort -= 10000  ;}
    if (get_button(button_B)) {while (get_button(button_B)) ;if (--ServerPort< 0) ServerPort += 10000  ;}
    lcd.print("  ") ;
  }
  else {
    if (ServerPort < 10000) lcd.print("0") ; 
    if (ServerPort < 1000) lcd.print("0") ; 
    if (ServerPort < 100) lcd.print("0") ; 
    if (ServerPort < 10) lcd.print("0") ; 
    lcd.print(ServerPort) ; 
  }
  lcd.print("             ") ; 
  return ;  
}
void second_line_clear() {
  lcd.setCursor(0,1) ;
  lcd.print("                ") ;
  return ;  
}
void ethernet_ISR () {
    EthernetClient client = server.available();
  if (client) {
    j = 0 ;      
    int CRC_chk = 0xffff  ;
    while (client.connected()) {
      if (j>7) {j = 0 ; CRC_chk = 0xffff ;}
      if (client.available()) {
        byte c = client.read();        
        R[j] = int(c) ;       
        CRC_chk = CRC16(R[j++],CRC_chk) ;   ; 
      }
      if (j == 8 && CRC_chk == 0 && R[0] == address) client.write(data,buf_len) ;
    }

    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}
void check_ethernet_update() {
  int ip_flag = 0, mac_flag = 0, port_flag = 0 ; 
  for (int i=0 ; i<4; i++) {
    if (pre_ip[i] != ip[i]) {ip_flag = 1 ; pre_ip[i] = ip[i] ; }
  }
  for (int i=0 ; i<6; i++) {
    if (pre_mac[i] != mac[i]) {mac_flag = 1 ; pre_mac[i] = mac[i]; }
  } 
  if (pre_ServerPort != ServerPort) {port_flag = 1 ; pre_ServerPort = ServerPort ; }
  if (ip_flag || mac_flag)   {
    Ethernet.begin(mac, ip);
  }
  if (port_flag) {
    EthernetServer server(ServerPort); 
    server.begin();
    }
  return ;
}
