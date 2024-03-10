#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SS_PIN  2  // D5
#define RST_PIN 4 // D17

MFRC522 mfrc522(SS_PIN, RST_PIN);

const char *ssid = "IH5G";
const char *password = "Samsunga20s@";
const char* device_token  = "cb539439fce32b22";

String URL = "http://192.168.131.30/rfidattendance/getdata.php"; //computer IP or the server domain
String getData, Link;
String OldCardID = "";
unsigned long previousMillis = 0;

void setup() {
  delay(1000);
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  connectToWiFi();
}

void loop() {
  if(!WiFi.isConnected()){
    connectToWiFi();
  }
  
  if (millis() - previousMillis >= 15000) {
    previousMillis = millis();
    OldCardID="";
  }
  delay(50);
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  String CardID ="";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    CardID += mfrc522.uid.uidByte[i];
  }
  
  if( CardID == OldCardID ){
    return;
  }
  else{
    OldCardID = CardID;
  }
  
  SendCardID(CardID);
  delay(1000);
}

void SendCardID( String Card_uid ){
  Serial.println("Sending the Card ID");
  if(WiFi.isConnected()){
    HTTPClient http;
    getData = "?card_uid=" + String(Card_uid) + "&device_token=" + String(device_token);
    Link = URL + getData;
    http.begin(Link);
    
    int httpCode = http.GET();
    String payload = http.getString();
    
    Serial.println(httpCode);
    Serial.println(Card_uid);
    Serial.println(payload);

    if (httpCode == 200) {
      if (payload.substring(0, 5) == "login") {
        String user_name = payload.substring(5);
      }
      else if (payload.substring(0, 6) == "logout") {
        String user_name = payload.substring(6);
      }
      else if (payload == "succesful") {
      }
      else if (payload == "available") {
      }
      delay(100);
      http.end();
    }
  }
}

void connectToWiFi(){
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected");
  
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    delay(1000);
}
