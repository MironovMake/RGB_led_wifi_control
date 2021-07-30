
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <Hash.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <LITTLEFS.h>
#include <DNSServer.h>
// #include <WiFiManager.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#else
#include <WiFi.h>
#endif

#include <ESPAsyncWiFiManager.h> //https://github.com/tzapu/WiFiManager

AsyncWebServer server(80);
DNSServer dns;

// Create an Event Source on /events
AsyncEventSource events("/events");

// REPLACE WITH YOUR NETWORK CREDENTIALS

String GeneralString;
const char *MyFile = {"/MyFile.txt"};
String TemporaryFile1;
String TemporaryFile2;
bool InitFlag = 0;
const char *parametr[leng] = {
    "value", "GREEN", "BLUE", "POWER", "BRIGHTNESS", //5
};                                                   // corner to turn
String sliderValue = "0";

//=====================//
// Set your Static IP address
IPAddress local_IP(192, 168, 1, 184);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <meta charset="UTF-8" />
    <title>ESP Web Server</title>
    <style>
      * {
        margin: 0;
        padding: 0;
      }
      html {
        font-family: Arial;
        display: inline-block;
        text-align: center;
      }
      h2 {
        font-size: 2.3rem;
        color: #fff;
      }
      p {
        font-size: 1.5rem;
        color: #fff;
        padding-bottom: 1rem;
      }
      body {
        padding-top: 30px;
        max-width: 100%;
        background: rgb(0, 14, 51);
      }
      .slider {
        -webkit-appearance: none;
        margin: 14px;
        width: 360px;
        height: 25px;
        background: #5c90ff;
        outline: none;
        -webkit-transition: 0.2s;
        transition: opacity 0.2s;
      }
      .slider::-webkit-slider-thumb {
        -webkit-appearance: none;
        appearance: none;
        width: 35px;
        height: 35px;
        background: #003249;
        cursor: pointer;
      }
      .slider::-moz-range-thumb {
        width: 35px;
        height: 35px;
        background: #003249;
        cursor: pointer;
      }

      input[type="checkbox"] {
        height: 40px;
        width: 40px;
        margin-bottom: 30px;
      }
      @media only screen and (max-width: 600px) {
        /*   For desktop:*/
        p {
          font-size: 1rem;
          color: #fff;
        }
        h2 {
          font-size: 2rem;
          color: #fff;
        }
        input[type="checkbox"] {
          height: 20px;
          width: 20px;
          margin-bottom: 20px;
        }
      }
    </style>
  </head>
  <body>
    <div>
      <h2>Подсветка Тимура</h2>
      <p>синий</p>
      <p>
        <input
          type="range"
          onchange="updateRed(this)"
          id="red"
          min="0"
          max="100"
          value="%RED%"
          step="-1"
          class="slider"
        />
      </p>
      <p>зеленый</p>
      <p>
        <input
          type="range"
          onchange="updateGreen(this)"
          id="green"
          min="101"
          max="200"
          value="%GREEN%"
          step="-1"
          class="slider"
        />
      </p>
      <p>красный</p>
      <p>
        <input
          type="range"
          onchange="updateBlue(this)"
          id="blue"
          min="201"
          max="300"
          value="%BLUE%"
          step="-1"
          class="slider"
        />
      </p>
      <p>тусклость</p>
      <p>
        <input
          type="range"
          onchange="updateBright(this)"
          id="bright"
          min="301"
          max="400"
          value="%BRIGHTNESS%"
          step="-1"
          class="slider"
        />
      </p>
      <br />
    </div>
    <script>
      let saf = ["red", "blue", "green"];

      function updateRed(element) {
        var sliderValue = document.getElementById("red").value;
        document.getElementById("red").innerHTML = sliderValue;
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/slider?value=" + sliderValue, true);
        xhr.send();
      }

      function updateGreen(element) {
        var sliderValue = document.getElementById("green").value;
        document.getElementById("green").innerHTML = sliderValue;
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/slider?value=" + sliderValue, true);
        xhr.send();
      }

      function updateBlue(element) {
        var sliderValue = document.getElementById("blue").value;
        document.getElementById("blue").innerHTML = sliderValue;
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/slider?value=" + sliderValue, true);
        xhr.send();
      }

      function updateBright(element) {
        var sliderValue = document.getElementById("bright").value;
        document.getElementById("bright").innerHTML = sliderValue;
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/slider?value=" + sliderValue, true);
        xhr.send();
      }

      function updatePower(element) {
        var sliderValue = document.getElementById("power").value;
        document.getElementById("power").innerHTML = sliderValue;
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/slider?value=" + sliderValue, true);
        xhr.send();
      }
    </script>
  </body>
</html>
)rawliteral";

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS &fs, const char *path)
{
  File file = fs.open(path, "r");
  if (!file || file.isDirectory())
  {
    return String();
  }
  String fileContent;
  while (file.available())
  {
    fileContent += String((char)file.read());
  }
  return fileContent;
}

void writeFile(fs::FS &fs, const char *path, String message)
{
  File file = fs.open(path, "w");
  if (!file)
  {
    return;
  }
  if (file.print(message))
  {
  }
  else
  {
  }
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
  File file = fs.open(path, "w");
  if (!file)
  {
    return;
  }
  if (file.print(message))
  {
  }
  else
  {
  }
}

String processor2(const String &var)
{
  for (int j = 0; j < leng; j++)
  {
    if (var == parametr[j])
    {
      TemporaryFile1 = parametr[j];
      TemporaryFile2 = "";
      if (j != leng - 1)
      {
        for (int i = GeneralString.indexOf(parametr[j]) + TemporaryFile1.length(); i < GeneralString.indexOf(parametr[j + 1]); i++)
        {
          TemporaryFile2 += GeneralString[i];
        }
      }
      else if (j == leng - 1)
      {
        for (int i = GeneralString.indexOf(parametr[j]) + TemporaryFile1.length(); i < GeneralString.length(); i++)
        {
          TemporaryFile2 += GeneralString[i];
        }
      }
      return TemporaryFile2;
    }
  }
  return String();
}

String GettingValueFromString(int j)
{
  TemporaryFile1 = parametr[j];
  TemporaryFile2 = "";
  if (j != leng - 1)
  {
    for (int i = GeneralString.indexOf(parametr[j]) + TemporaryFile1.length(); i < GeneralString.indexOf(parametr[j + 1]); i++)
    {
      TemporaryFile2 += GeneralString[i];
    }
  }
  else if (j == leng - 1)
  {
    for (int i = GeneralString.indexOf(parametr[j]) + TemporaryFile1.length(); i < GeneralString.length(); i++)
    {
      TemporaryFile2 += GeneralString[i];
    }
  }
  return TemporaryFile2;
}

void SendingValueToString(int j, int k)
{
  TemporaryFile1 = parametr[j];
  TemporaryFile2 = "";
  if (j != leng - 1)
  {
    for (int i = GeneralString.indexOf(parametr[j]) + TemporaryFile1.length(); i < GeneralString.indexOf(parametr[j + 1]); i++)
    {
      TemporaryFile2 += GeneralString[i];
    }
  }
  else if (j == leng - 1)
  {
    for (int i = GeneralString.indexOf(parametr[j]) + TemporaryFile1.length(); i < GeneralString.length(); i++)
    {
      TemporaryFile2 += GeneralString[i];
    }
  }
  GeneralString.replace(parametr[j] + TemporaryFile2, parametr[j] + String(k));
}

void WiFiSetup()
{
  Serial.println("ESP8266 INIT");
  Serial.println(F("Inizializing FS..."));
  (LittleFS.begin()) ? Serial.println(F("done.")) : Serial.println(F("fail."));

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("STA Failed to configure");
  }

  GeneralString = readFile(LittleFS, MyFile);
  bool IKnow = 1;
  if (IKnow)
  {
    GeneralString = "";
    for (int i = 0; i < leng - 1; i++)
    {
      GeneralString += parametr[i] + String(0);
    }
    GeneralString += parametr[leng - 1] + String(254);
    writeFile(LittleFS, MyFile, GeneralString);
  }
  else
  {
    for (int i = 0; i < leng; i++)
    {
      InternetSensorState[i] = 0;
      CurrentSensorState[i] = GettingValueFromString(i).toInt();
    }
  }
  Serial.println("GeneralString ");
  Serial.println(GeneralString);
  AsyncWiFiManager wifiManager(&server, &dns);
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("connected...yeey :)");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html, processor2); });
  // Send a GET request to <ESP_IP>/get?ENC=<inputMessage>

  server.on("/slider", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String inputMessage;

              if (request->hasParam("value")) // changed
              {
                inputMessage = request->getParam("value")->value();
                int TemporaryVar = inputMessage.toInt();
                if (TemporaryVar < 101)
                {
                  CurrentSensorState[0] = map(TemporaryVar, 0, 100, 0, 1023);
                  SendingValueToString(0, CurrentSensorState[0]);
                  if (CurrentSensorState[0] > 1000)
                    CurrentSensorState[0] = 1023;
                  Serial.print("CurrentSensorState[0]  ");
                  Serial.println(CurrentSensorState[0]);
                }
                else if (TemporaryVar > 100 && TemporaryVar < 201)

                {
                  CurrentSensorState[1] = map(TemporaryVar - 100, 0, 100, 0, 1023);
                  if (CurrentSensorState[1] < 50)
                    CurrentSensorState[1] = 0;
                  SendingValueToString(1, CurrentSensorState[1]);
                  Serial.print("CurrentSensorState[1]  ");
                  Serial.println(CurrentSensorState[1]);
                }
                else if (TemporaryVar > 200 && TemporaryVar < 301)
                {
                  CurrentSensorState[2] = map(TemporaryVar - 200, 0, 100, 0, 1023);
                  if (CurrentSensorState[2] < 50)
                    CurrentSensorState[2] = 0;
                  SendingValueToString(2, CurrentSensorState[2]);
                  Serial.print("CurrentSensorState[2]  ");
                  Serial.println(CurrentSensorState[2]);
                }
                else if (TemporaryVar == 401)
                {
                  CurrentSensorState[3] = !CurrentSensorState[3];
                  SendingValueToString(3, CurrentSensorState[3]);
                  Serial.print("CurrentSensorState[3]  ");
                  Serial.println(CurrentSensorState[3]);
                }
                else if (TemporaryVar < 401 && TemporaryVar > 300)
                {
                  CurrentSensorState[4] = -map(TemporaryVar - 300, 0, 100, 0, 1023);
                  if (CurrentSensorState[4] < -1000)
                    CurrentSensorState[4] = -1023;
                    if (CurrentSensorState[4] > -50)
                    CurrentSensorState[4] = 0;
                  SendingValueToString(4, CurrentSensorState[4]);
                  Serial.print("CurrentSensorState[4]  ");
                  Serial.println(CurrentSensorState[4]);
                }
                Internet_flag = 1;
              }

              request->send(200, "text/text", inputMessage);
            });
  server.onNotFound(notFound);

  events.onConnect([](AsyncEventSourceClient *client)
                   {
                     if (client->lastId())
                     {
                       Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
                     }
                     // send event with message "hello!", id current millis
                     // and set reconnect delay to 1 second
                     client->send("hello!", NULL, millis(), 10000);
                   });
  server.addHandler(&events);
  server.begin();

// other adjust
#ifndef ESP8266
  while (!Serial)
    ; // wait for serial port to connect. Needed for native USB
#endif
}