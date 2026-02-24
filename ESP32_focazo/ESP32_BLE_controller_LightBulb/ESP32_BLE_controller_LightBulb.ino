#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define DEVICE_NAME "No se conecten"
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHAR_UUID    "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define BRIGHTNESS   255   // Intensidad máxima (0-255)

// --- LED de estado (conexión BLE) ---
#define PIN_RED   21
#define PIN_GREEN 19
#define PIN_BLUE  18

// --- Relevador controlado por la app ---
int relayPin = 17;

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Función para escribir valores RGB en LED de estado
void rgbWrite(int r, int g, int b) {
  analogWrite(PIN_RED, r);
  analogWrite(PIN_GREEN, g);
  analogWrite(PIN_BLUE, b);
}

// Parpadeo rápido en LED de estado
void flashLED(int r, int g, int b, int count) {
  for (int i = 0; i < count; i++) {
    rgbWrite(r, g, b); delay(100);
    rgbWrite(0, 0, 0); delay(100);
  }
}

// Función para controlar el relevador
void setRelay(String command) {
  if (command == "On") {
    digitalWrite(relayPin, HIGH);   // Activa el relevador
    Serial.println("Relevador ENCENDIDO");
  } else if (command == "Off") {
    digitalWrite(relayPin, LOW);    // Apaga el relevador
    Serial.println("Relevador APAGADO");
  }
}

// Callbacks BLE
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) { deviceConnected = true; };
    void onDisconnect(BLEServer* pServer) { deviceConnected = false; }
};

// Callback para recibir datos desde la app
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String value = pCharacteristic->getValue();
      if (value.length() > 0) {
        Serial.print("Dato recibido: ");
        Serial.println(value);
        setRelay(value);   // Procesar comando "On" o "Off"
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando BLE...");

  // Configurar pin del relevador
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Inicialmente apagado

  // BLE
  BLEDevice::init(DEVICE_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  pCharacteristic = pService->createCharacteristic(
                      CHAR_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("Esperando cliente...");
}

void loop() {
  // LED de estado (conexión BLE)
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
    Serial.println(">>> CONECTADO <<<");
    flashLED(0, BRIGHTNESS, 0, 5); // Verde parpadeo rápido
  }

  if (!deviceConnected && oldDeviceConnected) {
    Serial.println(">>> DESCONECTADO <<<");
    flashLED(BRIGHTNESS, 0, 0, 5); // Rojo parpadeo rápido
    delay(500); 
    pServer->startAdvertising();
    oldDeviceConnected = deviceConnected;
  }

  if (deviceConnected) {
    rgbWrite(0, BRIGHTNESS, 0); // Verde fijo
  } else {
    rgbWrite(0, 0, BRIGHTNESS); // Azul fijo (esperando)
  }

  delay(10);
}