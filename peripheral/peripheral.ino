#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>
#include "Kalman.h"
#define BTN 2 // digital 2
#define RELEASE 770
#define HIT 200
#define R 3 // 빵판 9
#define G 4 // 빵판 10
#define B 5 // 빵판 11

int led_cycle=100;
float x,y,z;

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceYCharacteristicUuid = "baad41b2-f12e-4322-9ba6-22cd9ce09832";
const char* deviceZCharacteristicUuid = "5748a25d-1834-4c68-a49b-81bf3aeb2e50";

//const char* deviceGrenadeCharacteristicUuid = "47a4159c-4a86-4e76-be9e-3330113dd368";
const char* deviceReloadCharacteristicUuid = "2b1403ce-f0a4-4948-9958-08060edd9a388";
const char* deviceBassCharacteristicUuid = "ae89c431-708b-4199-b11c-0aac2b853e43";

BLEService gyroService(deviceServiceUuid);
BLEFloatCharacteristic gyroY(deviceYCharacteristicUuid, BLERead | BLENotify);
BLEFloatCharacteristic gyroZ(deviceZCharacteristicUuid, BLERead | BLENotify);

//BLEBoolCharacteristic grn(deviceGrenadeCharacteristicUuid, BLERead | BLENotify);
BLEBoolCharacteristic re(deviceReloadCharacteristicUuid, BLERead | BLENotify);
BLEIntCharacteristic bass(deviceBassCharacteristicUuid, BLERead | BLENotify);


void setup() {
  Serial.begin(9600);
  //while (!Serial); 
  
  pinMode(BTN, INPUT_PULLUP);
  pinMode(R,OUTPUT);
  pinMode(G,OUTPUT);
  pinMode(B,OUTPUT);

  digitalWrite(R,HIGH);
  digitalWrite(R,HIGH);
  digitalWrite(R,HIGH);
  
  if (!IMU.begin()) { // IMU begin
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  if (!BLE.begin()) { // BLE begin
    Serial.println("- Starting BLE module failed!");
    while (1);
  }
  
  BLE.setLocalName("Arduino Nano 33 BLE (Peripheral)");
  
  BLE.setAdvertisedService(gyroService);

  gyroService.addCharacteristic(gyroY);
  gyroService.addCharacteristic(gyroZ);
  
  //gyroService.addCharacteristic(grn);
  gyroService.addCharacteristic(re);
  gyroService.addCharacteristic(bass);
    
  BLE.addService(gyroService);
  BLE.advertise();

  Serial.println("Nano 33 BLE (Peripheral Device)");
  Serial.println();
}

void loop() {
  BLEDevice central = BLE.central(); // central 에 연결되어 있는가
  Serial.println("- Discovering central device...");
  delay(500);

  if (central) {
    Serial.println("* Connected to central device!");
    Serial.print("* Device MAC address: ");
    Serial.println(central.address());
    Serial.println(" ");
    delay(1000);

    
    while (central.connected()) {
      Serial.println(led_cycle);
      
      send_bass(); // gunshot + grenade
      send_mouse_loc(); // mouse cursor
      send_btn(); // reload
      //send_reload(); // reload
      
      if(led_cycle<=0){ // LED가 눈에 보일 정도로는 오래 지속되게끔
        digitalWrite(R,HIGH); //끄기
        digitalWrite(G,HIGH); //끄기
        digitalWrite(B,HIGH); //끄기
        led_cycle = 100;
      }
      led_cycle--;
      
    }
    Serial.println("* Disconnected to central device!");
  }
}

void send_bass(){ // 총 또는 수류탄
  //https://blog.naver.com/PostView.naver?blogId=eduino&logNo=222064670712&categoryNo=14&parentCategoryNo=0
  // 시계방향 == 민감도 낮춤    // 반시계방향 == 민감도 높임
  int val = 0;
  int s0 = analogRead(A0)-RELEASE;
  int s1 = analogRead(A1)-RELEASE;
  int s2 = analogRead(A2)-RELEASE;
  int s3 = analogRead(A3)-RELEASE;
  
  if(abs(s0)>HIT) val+=8; //27 베이스 0번줄
  if(abs(s1)>HIT) val+=4; //26 베이스 1번줄
  if(abs(s2)>HIT) val+=2; //25 베이스 2번줄  // 빵판 초록선
  if(abs(s3)>HIT) val+=1; //24 베이스 3번줄

  if(val%2){
    Serial.println("수류탄!");
    digitalWrite(R,HIGH); //끄기
    digitalWrite(G,HIGH); //끄기
    digitalWrite(B,LOW);  //켜기 B
    //Serial.println(2);
    bass.writeValue(2);   // 수류탄
  }
  else if(val!=0){
    Serial.println("총 발사!");
    digitalWrite(R,LOW);  //켜기 R
    digitalWrite(G,HIGH); //끄기
    digitalWrite(B,HIGH); //끄기
    //Serial.println(1);
    bass.writeValue(1);   // 총
  }
  else {
    bass.writeValue(0); // 아무것도 안함
  }
  
}
void send_mouse_loc(){
  if (IMU.gyroscopeAvailable()) { // 마우스 위치
    IMU.readGyroscope(x, y, z);
    gyroY.writeValue(5*(y-0.4)); //감도
    gyroZ.writeValue(5*(z-1)); // 감도
  }
}


void send_btn(){ // 장전
  if(!digitalRead(BTN)){
    Serial.println("장전버튼 누름!");
    re.writeValue(true);
    digitalWrite(R,HIGH); //끄기
    digitalWrite(G,LOW);  //켜기 
    digitalWrite(B,HIGH); //끄기
  }
  else re.writeValue(false);
}


/* kalman filter
float kal(float u){
  static const float r = 10;
  static const float h = 1.00;
  static float q = 10;
  static float p = 0;
  static float u_hat = 0;
  static float k = 0;

  k = p*h/(h*p*h+r);
  u_hat = u_hat + k*(u-h*u_hat);

  p = (1-k*h)*p+q;

  return u_hat;
  
}
float U[3];
*/
