#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>

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

// Replace with your network credentials

const char* ssid     = "Livebox6-5E4B";
const char* password = "467LkN59XhRk";

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://192.168.1.200/post-esp-data.php";

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String apiKeyValue = "tPmAT5Ab3j7F9";
String hola = "daroca.zaale23";
String espacio = "1";



void setup() {
  Serial.begin(115200);
    mfrc522.PCD_Init();    // Init MFRC522 board.
 
  // Prepare key - all keys are set to FFFFFFFFFFFF at chip delivery from the factory.
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
    String id_alumno1 = "";  // Variable para concatenar datos del bloque 1
    String id_alumno2 = "";  // Variable para concatenar datos del bloque 2



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
      id_alumno1 += ((char)blockDataRead1[i]);  // Print as character
    }
    
  }

  // LEER 2 BLOQUE
  if (mfrc522.MIFARE_Read(blockAddress2, blockDataRead2 , &bufferblocksize) != 0) {
    Serial.println("Read failed.");
  } else {
    for (byte j = 0; j < 9; j++) {
      id_alumno2 += ((char)blockDataRead2[j]);  // Print as character
    }

  }
id_alumno1.trim(); 
id_alumno2.trim();

  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&id_alumno=" + id_alumno1 + id_alumno2 +"&id_espacio=" + espacio;
    Serial.print("httpRequestData: ");

    Serial.println(httpRequestData);
    
    // You can comment the httpRequestData variable above
    // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
    //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
     
    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");
    
    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json");
    //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");
        
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  //Send an HTTP POST request every 30 seconds
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(1000);  
}