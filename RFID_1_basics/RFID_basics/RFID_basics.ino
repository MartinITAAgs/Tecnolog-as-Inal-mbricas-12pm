/* disclaimer
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.  

Librerías incluidas para el funcionamiento del modulo RFID*/
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

//Configuracion para permitir la comunicación del modulo RFID con la placa ESP-32
//MFRC522DriverPinSimple ss_pin(5); Se define el pin 5 como el pin SSA del RFID
//MFRC522DriverSPI driver{ss_pin};  Crea un driver SPI usando el pin 5
//MFRC522 mfrc522{driver}; Instancia del lector RFID, el objeto mfrc522 se necesitara para iniciar el lector

void setup() {
//Inicia una comunicación Serial
  Serial.begin(115200);  
  while (!Serial);       
  mfrc522.PCD_Init();   // Inicia el lector RIFD con el objeto mfrc522
//Muestra en el monitor serial  la información del lector.
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial); 
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
//Si no detecta una tarjeta que el lector pueda leer, vuelve a empezar el loop()
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  //Si al leer la tarjeta no la lee correctamente también sale del loop() y vuelve a empezar
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

//Muestra en el monitor Serial toda la información de la tarjeta, espera 2 segundos para evitar que se lea rápida y repetidamente la información de la tarjeta.
  MFRC522Debug::PICC_DumpToSerial(mfrc522, Serial, &(mfrc522.uid));
  delay(2000);
}
