#include <OneWire.h>          //ATEŞÖLÇER İÇİN KÜTÜPHANE
#include <DallasTemperature.h>
#include <TinyGPS++.h>        //GPS KÜTÜPHANE
#include <SoftwareSerial.h>
#include <HTTPClient.h>       //IOT İÇİN KÜTÜPHANE
#include <WiFi.h>

static const int RXPin = 34, TXPin = 35;  //GPS PIN TANIMLAMALARI
static const uint32_t GPSBaud = 9600;


const char* ssid = "td";        //BAĞLANILACAK WİFİ VE PASS
const char* password = "21081999";
String serverName = "https://askertakip.muhendisblog.com/insert.php";

float gpsLat;   //GPS DEGISKEN TANIMLAMALARI
float gpsLng;

TinyGPSPlus gps;      //GPS PIN ATAMALARI
SoftwareSerial ss(RXPin, TXPin);
OneWire oneWire (32);
DallasTemperature sicakliksensor (&oneWire);

void setup()
{
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wi-Fi bağlantısı için bekle
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  ss.begin(GPSBaud);
  sicakliksensor.begin();
}

void loop()
{
  HTTPClient http;
  
  gpsLat = gps.location.lat();
  gpsLng = gps.location.lng();
  
  sicakliksensor.requestTemperatures();
  float sicaklik = sicakliksensor.getTempCByIndex(0);

  String strgpsLat = String(gpsLat, 6);
  String strgpsLng = String(gpsLng, 6);
  
  String strVal = serverName + "?gpsLat=" + strgpsLat + "&gpsLng=" + strgpsLng+ "&temp=" + sicaklik;
  Serial.println(strVal);
  http.begin(strVal);
  int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();

      Serial.println(sicaklik);
  smartDelay(3000);
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
