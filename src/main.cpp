#include <SPI.h>
#include <Wire.h>
#define SEALEVELPRESSURE_HPA (1013.25)
#define OLED_RESET -1 // GPIO0
#define buzzer 14 // D15




#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>

Adafruit_SSD1306 display(128,64,&Wire,OLED_RESET);
Adafruit_BME280 bme;

float bme_temperature = 0;
float bme_humidity = 0;


bool show_TorH = 1;  
unsigned status;

bool alarmEnabled = false;
bool alarmActive = false;

float alarmAt = -10;

void beep() {
  tone(buzzer , 500);
  delay (1000); // mit der Dauer von einer Sekunde
  noTone(buzzer ); // Ausschalten des Tons.
  delay(500);
}
 
void setup() {

  beep();
  beep();
  
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  display.clearDisplay();

  status = bme.begin(0x76);  
  if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
      Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
      Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
      beep();
      delay(1000);
      beep();
  }


}

void showTemperatureOLED(String v, boolean m) {
  
  // text display tests
  display.setTextSize(7); // 7 best vor 2 diggits on 128x64
  display.setTextColor(WHITE);
  display.setCursor(5,17);
  display.print(v);

  // write °C
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(110,0);
  display.print("C");
  display.drawCircle(103,4,4,WHITE);

  if (m == true) {
    display.drawFastHLine(4,2,35,1);
    display.drawFastHLine(4,3,35,1);
    display.drawFastHLine(4,4,35,1);
    display.drawFastHLine(4,5,35,1);
    display.drawFastHLine(4,6,35,1);
  }

  //display.display();  
}

void showHumidityOLED(String v) { // value and unit
  
  display.setTextSize(5); // 5 best vor 2 diggits
  display.setTextColor(WHITE);
  display.setCursor(0,25);
  display.print(v);

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(85,0);
  display.print("Hum"); 
}

void loop() {
 
  display.clearDisplay();
    
  bme_temperature = bme.readTemperature();
  bme_humidity = bme.readHumidity();
    
  if ( bme_temperature < alarmAt && alarmEnabled == false )  // aktiviert den Alarm wenn die Temp erstmalig unter -15 fällt
  {
      alarmEnabled = true;
      Serial.println("Temperature going under configured temp, alarm active now.");
  }


  if ( alarmEnabled ) 
  {
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(110,50);
      display.println("A");
      Serial.println("Alarm enabled");

      if( bme_temperature > alarmAt ) alarmActive = true; else alarmActive = false;
      
      
  }
  
  if (show_TorH)
  {
    Serial.print(F("Temperature Sensor [°C]:\t\t"));
    Serial.println(bme_temperature);
    int btr = round(bme_temperature);
    if( btr < 0 ) 
    {
      btr = -btr;
      showTemperatureOLED(String(btr),true);
    } else  showTemperatureOLED(String(btr),false);
    
    show_TorH = !show_TorH;
  }
  else
  {
    if( alarmActive ) {
        display.setTextSize(4);
        display.setTextColor(WHITE);
        display.setCursor(2,8);
        display.println("ALARM"); 
        // BEEP AUSLÖSEN
    } else {
      Serial.print(F("Humidity Sensor [%]:\t\t\t")); 
      Serial.println(bme_humidity);
      int bhr = round(bme_humidity);
      showHumidityOLED(String(bhr)+"%");
    }
    show_TorH = !show_TorH;
  }

} else {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println("BME280");
      display.println("NOT FOUND");
}

display.display(); 
delay(10000);  


