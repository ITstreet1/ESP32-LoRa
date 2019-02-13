#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h" 
#include <WiFi.h>

#define SCK     5    // GPIO5  -- SX127x's SCK
#define SDA     4    // GPIO4  -- SX127x's SDA
#define SCL     15   // GPIO15 -- SX127X's SCL
#define MISO    19   // GPIO19 -- SX127x's MISO
#define MOSI    27   // GPIO27 -- SX127x's MOSI
#define SS      18   // GPIO18 -- SX127x's CS
#define RST     14   // GPIO14 -- SX127x's RESET
#define DI00    26   // GPIO26 -- SX127x's IRQ(Interrupt Request)
#define RST_LED 16   // GPIO16 -- OLED reset pin
#define LED     25   // GPIO25 -- LED Light pin
#define BAND    868E6 
#define PABOOST true

#define V2   1
#ifdef V2 
  #define Vext  21
#endif

SSD1306  display(0x3c, SDA, SCL, RST_LED);
String rssi = "RSSI --";
String packSize = "--";
String packet ;
String t, h, g = ""; // promenljive za temperaturu i relativnu vlaznost vazduha

const char* ssid = "dekip_network";       //SSID
const char* password = "0611975itstreet"; //lozinka
const char* webServer = "www.itstreet.org"; //host
const uint16_t httpPort = 80;
WiFiClient client;

int period = 60000;
unsigned long vreme = 0;

void loraData(){
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0 , 15 , "Received "+ packSize + " bytes");
 // display.drawStringMaxWidth(0 , 48 , 128, packet);
  display.drawString(0, 0, rssi); 
  
  display.drawString(0,26, "Temperatura: ");
  t=packet.substring(0,5);
  display.drawString(90,26, t+" C");
  Serial.println("Temperatura je: "+t+"C");
  
  display.drawString(0,37, "Vlaznost Vazduha: ");
  h=packet.substring(6,11);
  display.drawString(90,37, h+" %");
  Serial.println("Vlaznost V je: "+h+"%");

  display.drawString(0,48, "Vlaznost Zemlje: ");
  g=packet.substring(12,17);
  display.drawString(90,48, g+" %");
  Serial.println("Vlaznost Z je: "+g+"%");
  display.display();
}

void cbk(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC);
  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
  loraData();
}

void setup() {
  Serial.begin(115200);
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  display.init();
  display.flipScreenVertically();  
  display.setFont(ArialMT_Plain_10);
  delay(1500);
  display.clear();
  
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI00);
  
  if (!LoRa.begin(BAND,PABOOST)) {
    display.drawString(0, 0, "Starting LoRa failed!");
    display.display();
    while (1);
  }
  display.drawString(0, 0, "LoRa Initial success!");
  display.drawString(0, 10, "Wait for incoming data...");

  //povezivanje na lokalnu mrezu
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    display.drawString(0,30,".");
    }
  display.drawString(0,40,"Povezan"); 
  display.display();
  delay(1000);
  //LoRa.onReceive(cbk);
  LoRa.receive();
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) { 
    cbk(packetSize);  
    }
 if(millis()>vreme+period){
  sendToWeb();
  vreme=millis();
    //vreme koliko pravimo pauzu nakon svakog novog upisa u bazu na serveru
 }
}
void sendToWeb(){
  if(!client.connect(webServer,httpPort)){
    delay(5000);
    return;
    }
    client.print("GET /test/index.php?temp=");
    client.print(t);
    client.print("&vlaga=");
    client.print(h);
    client.print("&hygro=");
    client.print(g);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(webServer);
    client.println("Connection: close");
    client.println();  
  }
