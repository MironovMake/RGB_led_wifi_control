
#include <Arduino.h>
int Internet_flag = 0;
const int leng = 5;
int CurrentSensorState[leng];
int InternetSensorState[leng];
bool FirstTimeFlag = 1;
// WIFI ADJUSTs
#include <WiFiAdjust.h>
int red = 15;   //8;
int green = 12; // 6;
int blue = 13;  //7;
int power = 14; //5;

void setup()
{
  pinMode(power, OUTPUT);
  digitalWrite(power, LOW);
  Serial.begin(115200);
  Serial.println("ESP8266 INIT");
  Serial.println(F("Inizializing FS..."));
  // Initialize LittleFS
  WiFiSetup();
  Serial.println(WiFi.localIP());
}

void loop()
{
  if (Internet_flag)
  {
    Serial.println("Something happening in internet");
    writeFile(LittleFS, MyFile, GeneralString);
    Internet_flag = 0;

    analogWrite(red, CurrentSensorState[0]+CurrentSensorState[4]);
    analogWrite(green, CurrentSensorState[1]+CurrentSensorState[4]);
    analogWrite(blue, CurrentSensorState[2]+CurrentSensorState[4]);
    digitalWrite(power, CurrentSensorState[3]);
    Serial.print("red ");
    Serial.print(CurrentSensorState[0]);
    Serial.print("  green ");
    Serial.print(CurrentSensorState[1]);
    Serial.print("  blue ");
    Serial.print(CurrentSensorState[2]);
    Serial.print("  bright ");
    Serial.println(CurrentSensorState[4]);
  }
}