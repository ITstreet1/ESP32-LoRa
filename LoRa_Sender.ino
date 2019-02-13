#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h" 
#include "DHT.h"
#define SDA     4    // GPIO4  -- SX127x's SDA
#define SCK     5    // GPIO5  -- SX127x's SCK
#define SCL     15   // GPIO15 -- SX127X's SCL
#define MISO    19   // GPIO19 -- SX127x's MISO
#define MOSI    27   // GPIO27 -- SX127x's MOSI
#define SS      18   // GPIO18 -- SX127x's CS
#define RST     14   // GPIO14 -- SX127x's RESET
#define RST_LED 16   // GPIO16 -- OLED reset pin
#define LED     25   // GPIO25 -- LED Light pin
#define DI00    26   // GPIO26 -- SX127x's IRQ(Interrupt Request)
#define DHTPIN  17   // GPIO17 -- dht pin
#define DHTTYPE DHT22 //DHT22 modul
DHT dht(DHTPIN, DHTTYPE);

#define BAND    868E6 //biramo band
#define PABOOST true
#define V2   1
#ifdef V2 //WIFI Kit series V1 not support Vext control
  #define Vext  21
#endif

unsigned int counter = 0;
SSD1306  display(0x3c, SDA, SCL, RST_LED);
String rssi = "RSSI --";
String packSize = "--";
String packet ;

void setup(){
  pinMode(Vext,OUTPUT);
  pinMode(LED,OUTPUT);
  digitalWrite(Vext, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  display.init();
  display.flipScreenVertically();  
  display.setFont(ArialMT_Plain_10);
  delay(1500);
  display.clear();
  dht.begin();
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI00);
  
  if (!LoRa.begin(BAND,PABOOST))
  {
    display.drawString(0, 0, "Starting LoRa failed!");
    display.display();
    while (1);
  }
  display.drawString(0, 0, "LoRa Initial success!");
  display.display();
  delay(1000);
}

void loop()
{
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  analogReadResolution(10);  //izcitavamo 10-bitnu rezoluciju
  int adc_hig = analogRead(13);
  adc_hig = 1024-adc_hig; //invertovanje vrednosti
  float g = map(adc_hig, 0, 1024, 0, 100); //izvlacimo postotak
  
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Sending packet: ");
  display.drawString(90, 0, String(counter));  //broj poslatih paketa
  display.drawString(0, 16, "Temperatura: ");
  display.drawString(90, 16, String(t)+" "+(char)247+"C");  //temperatura u celzijusima
  display.drawString(0, 32, "Vlaznost vazduha");
  display.drawString(90,32, String(h)+" %");  //relativna vlaznost vazduha
  display.drawString(0, 47, "Vlaznost zemlje");
  display.drawString(90,47, String(g)+" %");  //vlaznost zemlje
  display.display();

  // send packet
  LoRa.beginPacket(); //otpocinjemo slanje paketa
  //LoRa.print("hello ");
  //LoRa.print(counter);
  LoRa.print(t);  //temperatura
  LoRa.print(","); 
  LoRa.print(h); //relativna vlaznost vazduha
  LoRa.print(",");
  LoRa.print(g);  //vlaznost zemlje
  LoRa.endPacket();  //zakljucujemo paket

  counter++;
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(10000);                       // wait for a second
}
