#include "main.h"

int mdns1(void);
void launchWeb(void);
void setupAP(void);
int testWifi(void);

String urldecode(const char *src);
BlynkTimer timer;
char auth[] = "MUixCwOG4evgwHBERfITVy5aV0tQAmRS";
WiFiServer server(80);
const char *APssid = "nodemcu"; // Name of access point
const char *APpass = "12345678";       // Pass of access point
String rsid;
String rpass;
boolean newSSID = false;
PZEM004Tv30 pzem(&Serial1);
float voltage, current, power, energy;

BLYNK_CONNECTED() {

  Serial.println(__FUNCTION__);
  Blynk.syncAll();
}

void sendSensor() {
  voltage = pzem.voltage();
  current = pzem.current();
  power = pzem.power();
  energy = pzem.energy();

  Blynk.virtualWrite(V5, voltage);
  Blynk.virtualWrite(V6, current);
  Blynk.virtualWrite(V7, power);
  Blynk.virtualWrite(V8, energy);
}

void setup() {

  Serial.begin(57600);
  delay(10);

  Serial.print("Reset energy");
  pzem.resetEnergy();

  Serial.print("Set address to 0x42");
  pzem.setAddress(0x42);

  if (testWifi()) /*--- if the stored SSID and password connected successfully, exit setup ---*/
  {
    Blynk.begin(auth, WiFi.SSID().c_str(), WiFi.psk().c_str());
  }
  else /*--- otherwise, set up an access point to input SSID and password  ---*/
  {

  }
  timer.setInterval(1000L, sendSensor);
}


void loop() {

  Blynk.run();
  timer.run();
}


void launchWeb()
{
  Serial.println("");
  Serial.println(WiFi.softAPIP());
  server.begin(); // Start the server
  Serial.println("Server started");
  int b = 20;
  int c = 0;
  while (b == 20)
  {
    b = mdns1(); //mdns1(function: web interface, read local IP SSID, Pass)
    /*--- If a new SSID and Password were sent, close the AP, and connect to local WIFI ---*/
    if (newSSID == true)
    {
      newSSID = false;
      /*--- convert SSID and Password string to char ---*/
      char ssid[rsid.length()];
      rsid.toCharArray(ssid, rsid.length());
      char pass[rpass.length()];
      rpass.toCharArray(pass, rpass.length());

      Serial.println("Connecting to local Wifi");
      delay(500);
      WiFi.softAPdisconnect(true); //disconnet APmode
      delay(500);
      WiFi.mode(WIFI_STA);    //change to STA mode
      WiFi.begin(ssid,pass);                            //connect to local wifi
      delay(1000);
      ///set -> check -> set
      if (testWifi()) //test connect
      {
        Blynk.begin(auth, ssid, pass);
        return;
      }
      else //if wrong ssid or pass
      {
        Serial.println("");
        Serial.println("New SSID or Password failed. Reconnect to server, and try again.");
        setupAP();
        return;
      }
    }
  }
}

void setupAP(void)
{
  WiFi.mode(WIFI_STA); //mode STA
  WiFi.disconnect();   //disconnect to scan wifi
  delay(100);

  Serial.println("");
  delay(100);
  WiFi.softAP(APssid, APpass); //change to AP mode with AP ssid and APpass
  Serial.println("softAP");
  Serial.println("");
  launchWeb(); //?
}


int testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while (c < 20)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("WiFi connected.");
      Serial.println(WiFi.localIP());
      return (1);
    }
    delay(500);
    Serial.print(WiFi.status());
    c++;
  }
  return (0);
} //end testwifi
