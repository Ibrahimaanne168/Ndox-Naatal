#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#define SOIL_PIN 34
#define DHT_PIN 4
#define DHT_TYPE DHT11
const char* WIFI_SSID="VOTRE_WIFI";
const char* WIFI_PASSWORD="VOTRE_MOT_DE_PASSE";
// Adresse IPv4 locale du PC exécutant Flask, pas 127.0.0.1
const char* SERVER_URL="http://192.168.1.10:5000/api/esp32";
const int RAW_DRY=3000; // À calibrer avec votre capteur
const int RAW_WET=1300; // À calibrer avec votre capteur
DHT dht(DHT_PIN,DHT_TYPE);
unsigned long lastSend=0;
float soilPercent(){int raw=analogRead(SOIL_PIN);float p=(float)(RAW_DRY-raw)*100.0/(RAW_DRY-RAW_WET);return constrain(p,0.0,100.0);}
void setup(){Serial.begin(115200);analogReadResolution(12);dht.begin();WiFi.begin(WIFI_SSID,WIFI_PASSWORD);Serial.print("Connexion Wi-Fi");while(WiFi.status()!=WL_CONNECTED){delay(500);Serial.print(".");}Serial.println(WiFi.localIP());}
void loop(){
 if(millis()-lastSend>=10000){lastSend=millis();float soil=soilPercent(),t=dht.readTemperature(),h=dht.readHumidity();
  if(isnan(t)||isnan(h)){Serial.println("Erreur DHT11");return;}
  Serial.printf("Sol %.1f%% | Air %.1f C | Humidite air %.1f%%\n",soil,t,h);
  if(WiFi.status()==WL_CONNECTED){HTTPClient http;http.begin(SERVER_URL);http.addHeader("Content-Type","application/json");
   String body="{\"parcel\":\"Parcelle de Miss\",\"soil_moisture\":"+String(soil,1)+",\"air_temperature\":"+String(t,1)+",\"air_humidity\":"+String(h,1)+"}";
   int code=http.POST(body);Serial.printf("HTTP %d\n",code);http.end();}
 }
 delay(50);
}
