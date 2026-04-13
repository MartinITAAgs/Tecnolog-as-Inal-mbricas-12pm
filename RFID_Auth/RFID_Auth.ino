#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

String uidString = "";    
int control = 0;
const int LED1=16;
const int LED2=17;


MFRC522DriverPinSimple ss_pin(5);
MFRC522DriverSPI driver{ss_pin};
MFRC522 mfrc522{driver};

void setup() {
  Serial.begin(115200);  
  while (!Serial);       
  


  mfrc522.PCD_Init();
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);	
	Serial.println(F("Scan PICC to see UID"));
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
}

void loop() {

    String uidentrada = "";
	if (!mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	if (!mfrc522.PICC_ReadCardSerial()) {
		return;
	}
 if (control == 0){
    Serial.print("Card UID: ");
  MFRC522Debug::PrintUID(Serial, (mfrc522.uid));
  Serial.println();

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uidString += "0"; 
    }
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println("tarjeta registrada");
  Serial.println(uidString);
  control = 1;
 }
else
 {
  Serial.print("Card UID: ");
  MFRC522Debug::PrintUID(Serial, (mfrc522.uid));
  Serial.println();
  

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uidentrada += "0"; 
    }
    uidentrada += String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println(uidentrada);
    if (uidString == uidentrada){
    analogWrite(LED1, 255);
    Serial.println("La tarjeta escaneada esta registrada");
    Serial.println("");
    delay(2000);
    analogWrite(LED1, 0);
  }
  else{
    analogWrite(LED2, 255);
    Serial.println("La tarjeta escaneada no esta registrada");
    Serial.println("");
    delay(2000);
    analogWrite(LED2, 0);
  }
 }
  delay(2000);
}
