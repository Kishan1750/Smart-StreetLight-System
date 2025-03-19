#include<WiFi.h>
#include<Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "analogWrite.h"

#define WIFI_SSID "wifi name"
#define WIFI_PASSWORD "wifi pw"

#define API_KEY "api"
#define DATABASE_URL "db link"

#define LDR 36
#define PIR 26
#define R1 2
#define R1_out 4

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
int lightIntensity = 0;
float voltageR1_out = 0.0;
int brightness_100 = 255;
int brightness_25 = 64;
int light = 1;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
  Serial.print("."); delay (300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP () );
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("signUp OK");
    signupOK = true; 
  }else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  pinMode(LDR, INPUT);
  pinMode(PIR, INPUT);
  pinMode(R1, OUTPUT);
  pinMode(R1_out, OUTPUT);
  if(Firebase. RTDB.setInt(&fbdo, "lights/total lights", light)) {
    Serial.println(); Serial.print("Total lights: "); Serial.print(light);
    Serial.print("--successfully saved to: " + fbdo.dataPath());
    Serial.println(" (" + fbdo.dataType() + ")");
    if(Firebase. RTDB.setString(&fbdo, "lights/light status", "OFF")) {
      Serial.println(); Serial.print("Light is: OFF");
      Serial.print("--successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }else{
        Serial.println("FAILED: " + fbdo.errorReason());
      }
  }else{
      Serial.println("FAILED: " + fbdo.errorReason());
  }
}
void loop() {
  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis= millis();
    // ---STORE sensor data to a RTDB
    lightIntensity = analogRead(LDR);
    if (Firebase. RTDB.setInt(&fbdo, "Sensor/ldr", lightIntensity)) {
      Serial.println(); Serial.print("LDR: "); Serial.print(lightIntensity);
      Serial.print("--Successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
      if(lightIntensity <= 400){
        int motion = analogRead(PIR);
        if (Firebase. RTDB.setInt(&fbdo, "Sensor/pir", motion)) {
          Serial.println(); Serial.print("PIR: "); Serial.print(motion);
          Serial.print("--successfully saved to: " + fbdo.dataPath());
          Serial.println(" (" + fbdo.dataType() + ")");
          if(motion != 0){
            Serial.println("R1 high(100 %) ");
            analogWrite(R1, brightness_100);
          }
          else{
            Serial.println("R1 low(25 %) ");
            analogWrite(R1, brightness_25);
          }
        }
        else{
          Serial.println("FAILED: " + fbdo.errorReason() );   //for PIR
        }
      }
      else{
        Serial.println("R1 off(lights off) ");
        analogWrite(R1, 0);
      } 
    }else{
      Serial.println("FAILED: " + fbdo.errorReason() );   //for LDR
     }
    // voltageR1_out = (analogRead(R1_out) * 3.3) / 4095.0;
    voltageR1_out = (float)analogReadMilliVolts(R1_out)/1000;
    if(Firebase. RTDB.setFloat(&fbdo, "lights/voltage of light", voltageR1_out)) {
      Serial.println(); Serial.print("Voltage of light: "); Serial.print (voltageR1_out);
      Serial.print(" successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");  
      if(voltageR1_out != 0 && lightIntensity <=400) {
        if (Firebase.RTDB.setString(&fbdo, "lights/light status", "ON")) {
          Serial.println(); Serial.print("Light is: ON");
          Serial.print(" successfully saved to: " + fbdo.dataPath());
          Serial.println(" (" + fbdo.dataType() + ")");
        }else{
          Serial.println("FAILED: " + fbdo.errorReason());  
        }
      }else{
        if (Firebase. RTDB.setString(&fbdo, "lights/light status", "OFF")) {
          Serial.println(); Serial.print("Lights is: OFF");
          Serial.print("--successfully saved to: " + fbdo.dataPath());
          Serial.println(" (" + fbdo.dataType() + ")");
        }else{
          Serial.println("FAILED: " + fbdo.errorReason());
        }
      }
    }else{
      Serial.println("FAILED: " + fbdo.errorReason());    //for R1_out
    }
    if(voltageR1_out == 0) {
      if(Firebase.RTDB.setString(&fbdo, "lights/faulty", "YES")) {
        Serial.println(); Serial.print("Light is faulty");
        Serial.print("--successfully saved to: " + fbdo.dataPath());
        Serial.println(" (" + fbdo.dataType() + ")");
      }else{
        Serial.println("FAILED: " + fbdo.errorReason());  
      }
    }else{
      if(Firebase.RTDB.setString(&fbdo, "lights/faulty", "NO")) {
        Serial.println(); Serial.print("Light is working");
        Serial.print("--successfully saved to: " + fbdo.dataPath());
        Serial.println(" (" + fbdo.dataType() + ")");
      }else{
        Serial.println("FAILED: " + fbdo.errorReason());  
      }
    }
  }
}
