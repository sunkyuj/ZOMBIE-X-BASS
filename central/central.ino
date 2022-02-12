#include <ArduinoBLE.h>
#include "PluggableUSBHID.h"
#include "USBMouse.h"
//#define MIC A0
//#define SILENT 752
//#define LOUD 4


const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
//const char* deviceXCharacteristicUuid = "741c12b9-e13c-4992-8a5e-fce46dec0bff";
const char* deviceYCharacteristicUuid = "baad41b2-f12e-4322-9ba6-22cd9ce09832";
const char* deviceZCharacteristicUuid = "5748a25d-1834-4c68-a49b-81bf3aeb2e50";
//const char* deviceProxCharacteristicUuid = "278fdd97-e3e9-4860-b9b5-d4f51183db80";
const char* deviceGrenadeCharacteristicUuid = "47a4159c-4a86-4e76-be9e-3330113dd368";
const char* deviceReloadCharacteristicUuid = "2b1403ce-f0a4-4948-9958-08060edd9a388";
const char* deviceBassCharacteristicUuid = "ae89c431-708b-4199-b11c-0aac2b853e43";

//bool grn_btn_clicked;
float x,y,z;
int bass_state=0;

USBMouse mouse;

union uni{
  unsigned char byt[4]; // byte
  bool bl;
  int i;
  float f;
};

uni getData(const unsigned char data[]){
  uni dat;
  for (int i = 0; i < 4; i++) {
    dat.byt[i] = data[i]; 
  }
  return dat;
}


uni getData1(const unsigned char data[],int length){
  uni dat;
  for (int i = 0; i < length; i++) {
    dat.byt[i] = data[i];
    Serial.print(dat.byt[i]); 
  }
  Serial.println();
  Serial.println(dat.bl);
  return dat;
}

void setup() {
  Serial.begin(9600);
  //while (!Serial);

  if (!BLE.begin()) { // BLE begin
    Serial.println("starting BLE failed!");
    while (1);
  }
  delay(500);
  
  BLE.setLocalName("central");

  Serial.println("Arduino Nano 33 BLE Sense (Central Device)");
  Serial.println();
}

void loop() {
  connectToPeripheral();
}

void connectToPeripheral(){
  BLEDevice peripheral; // peripheral Nano
  
  Serial.println("- Discovering peripheral device...");

  do
  {
    BLE.scanForUuid(deviceServiceUuid); // peripheral Nano 찾아서 연결
    peripheral = BLE.available();
  } while (!peripheral);
  
  if (peripheral) {
    Serial.println("* Peripheral device found!");
    Serial.print("* Device MAC address: ");
    Serial.println(peripheral.address());
    Serial.print("* Device name: ");
    Serial.println(peripheral.localName());
    Serial.print("* Advertised service UUID: ");
    Serial.println(peripheral.advertisedServiceUuid());
    Serial.println(" ");
    BLE.stopScan();
    Serial.println("- Connecting to peripheral device...");

    if (peripheral.connect()) { // Server Nano에 연결
      Serial.println("* Connected to peripheral device!");
      Serial.println(" ");
    } else {
      Serial.println("* Connection to peripheral device failed!");
      Serial.println(" ");
      return;
    }

    Serial.println("- Discovering peripheral device attributes...");
    if (peripheral.discoverAttributes()) {
      Serial.println("* Peripheral device attributes discovered!");
      Serial.println(" ");
    } else {
      Serial.println("* Peripheral device attributes discovery failed!");
      Serial.println(" ");
      peripheral.disconnect();
      return;
    }
    controlPeripheral(peripheral); // Server Nano 컨트롤 해주는 함수
  }
}
    

void controlPeripheral(BLEDevice peripheral) { // Server Nano를 인자로 받음

  BLEService peripheralService = peripheral.service(deviceServiceUuid);
  //BLECharacteristic gyroX = peripheralService.characteristic(deviceXCharacteristicUuid);
  BLECharacteristic gyroY = peripheralService.characteristic(deviceYCharacteristicUuid);
  BLECharacteristic gyroZ = peripheralService.characteristic(deviceZCharacteristicUuid);

  //BLECharacteristic prox = peripheralService.characteristic(deviceProxCharacteristicUuid);
  BLECharacteristic grn = peripheralService.characteristic(deviceGrenadeCharacteristicUuid);
  BLECharacteristic reload = peripheralService.characteristic(deviceReloadCharacteristicUuid);
  BLECharacteristic bass = peripheralService.characteristic(deviceBassCharacteristicUuid);
  /*int val=0;
  int abs_val=0;
  int shot_loop=0*/;

  while (peripheral.connected()) { // 서버와 연결이 되어있는 동안
      //gyroX.read();
      gyroY.read();
      gyroZ.read();
      //prox.read();
      grn.read();
      reload.read();
      bass.read();

      mouse.release(MOUSE_LEFT);
      
      //베이스 줄 상태 받기 (총, 수류탄)
      bass_state = getData(bass.value()).i;
      Serial.println(bass_state);
      if(bass_state==2) mouse.click(MOUSE_MIDDLE); // 4번 줄 쳤을 때 수류탄
      else if(bass_state==1) mouse.press(MOUSE_LEFT); // 1,2,3번줄 쳤을 때 총 , press 하고 다음 루프때 release?
      //if(bass_state==15) mouse.click(MOUSE_MIDDLE); // 네 줄 모두 쳤을 때 수류탄
      //else if(bass_state!=0) mouse.click(MOUSE_LEFT); // 줄 안 친건 아닐 때 총 

      // 마우스 우클릭 (장전)
      if(getData(grn.value()).bl) mouse.click(MOUSE_RIGHT);
      
      //bool r = getData1(reload.value(),reload.valueLength()).bl;
      //Serial.println(r);
      //if(r) mouse.click(MOUSE_RIGHT);

      // 마우스 이동
      y = getData(gyroY.value()).f;
      z = getData(gyroZ.value()).f;
      //mouse.move(y,z); // [ 방향
      mouse.move(-y,z); // ] 방향


      /*
      // 마우스 좌클릭(총)      
      if(shot_loop) {
        mouse.press(MOUSE_LEFT);
        shot_loop--;
      }
      else{
        val=analogRead(A0);
        abs_val = abs(val - SILENT);
        if (LOUD < abs_val) {
          shot_loop=10;
          for(int i=0;i<10;i++)
            mouse.press(MOUSE_LEFT); //<--------------여기!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        }
      }
      if(shot_loop<=0) mouse.release(MOUSE_LEFT);
    
      // 마우스 우클릭(장전)
      //if(getData(prox.value()).bl) mouse.click(MOUSE_RIGHT);
  
      // 마우스 중간클릭(수류탄)
      //if(getData(grn.value()).bl) mouse.click(MOUSE_MIDDLE);*/
  
    }  
  Serial.println("- Peripheral device disconnected!");
}
