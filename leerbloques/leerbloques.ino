#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

// Learn more about using SPI/I2C or check the pin assigment for your board: https://github.com/OSSLibraries/Arduino_MFRC522v2#pin-layout
MFRC522DriverPinSimple ss_pin(5);

MFRC522DriverSPI driver{ss_pin}; // Create SPI driver
//MFRC522DriverI2C driver{};     // Create I2C driver
MFRC522 mfrc522{driver};         // Create MFRC522 instance

MFRC522::MIFARE_Key key;

byte blockAddress1 = 10;
byte blockAddress2 = 9;
byte bufferblocksize = 18;
byte blockDataRead1[18];
byte blockDataRead2[18];



void setup() {
  Serial.begin(115200);  // Initialize serial communication
  while (!Serial);       // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
  
  mfrc522.PCD_Init();    // Init MFRC522 board.
 
  // Prepare key - all keys are set to FFFFFFFFFFFF at chip delivery from the factory.
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

void loop() {
  String id_alumno1 = "";  // Variable para concatenar datos del bloque 1
  String id_alumno2 = "";  // Variable para concatenar datos del bloque 2
  // Check if a new card is present
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(500);
    return;
  }

  // Display card UID
  Serial.print("\nCard UID: ");
  MFRC522Debug::PrintUID(Serial, (mfrc522.uid));
  Serial.println();

  // Authenticate the specified block using KEY_A = 0x60
  if (mfrc522.PCD_Authenticate(0x60, blockAddress1, &key, &(mfrc522.uid)) != 0) {
    Serial.println("Authentication failed.");
    return;
  }

  //LEER 1 BLOQUE
  if (mfrc522.MIFARE_Read(blockAddress1, blockDataRead1 , &bufferblocksize) != 0) {
    Serial.println("Read failed.");
  } else {
    for (byte i = 0; i < 16; i++) {
      id_alumno1 += (char)blockDataRead1[i];  // Concatenar como String
    }
    
  }

  // LEER 2 BLOQUE
  if (mfrc522.MIFARE_Read(blockAddress2, blockDataRead2 , &bufferblocksize) != 0) {
    Serial.println("Read failed.");
  } else {
    for (byte i = 0; i < 9; i++) {
      id_alumno2 += (char)blockDataRead2[i];  // Concatenar como String  // Print as character
    }

  }
  Serial.println(id_alumno1 + id_alumno2);  
  // Halt communication with the card
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(2000);  // Delay for readability
}