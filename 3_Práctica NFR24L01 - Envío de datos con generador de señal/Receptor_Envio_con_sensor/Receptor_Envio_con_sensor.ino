#include <SPI.h>
#include <RF24.h>

#define CE_PIN    4
#define CSN_PIN   5
#define SCK_PIN   12
#define MISO_PIN  13
#define MOSI_PIN  11
#define pinLed 18

int brillo =0;
int distancia = 0;

RF24 radio(CE_PIN, CSN_PIN);
const byte direccion[6] = "00001";

// Variable para controlar si se enciende el LED o no
bool encendido = false;

void setup() {
  Serial.begin(115200);
  pinMode(pinLed, OUTPUT);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);

  if (!radio.begin()) {
    Serial.println("NRF24 NO detectado");
    #ifdef RGB_BUILTIN
      rgbLedWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0); 
    #endif
    while (1);
  }

  Serial.println("NRF24 detectado");
  radio.setChannel(69);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, direccion);
  radio.startListening();
}

void loop() {
  // 1. Revisar si hay datos
  if (radio.available()) {
    radio.read(&distancia, sizeof(distancia));
    
    Serial.print("Recibido: ");
    Serial.println(distancia);
    encendido = !encendido;
    // Actualizamos el status del LED
  }

  // 2. Control del LED basado en el status
  #ifdef RGB_BUILTIN
    if (encendido) {
      rgbLedWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0);
    } else {
      rgbLedWrite(RGB_BUILTIN, 0, 0, 20);
    }
  #endif

  brillo = map(distancia, 0, 50, 255, 0);
  brillo = constrain(brillo, 0, 255);
  if (distancia > 0) {
    analogWrite (pinLed, brillo);
  }
  delay(10);
}