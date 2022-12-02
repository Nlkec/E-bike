/*
This code is used to gather data from a VESC6 and display on an LCD screen.
Written for Arduino Nano and LCD I2C 1602.

Original VESCUART.h code written by SolidGeek.
 */

#include <VescUart.h>
#include <SimpleKalmanFilter.h>

//Library for the Display
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <ArduinoJson.h>
#include <Adafruit_ST7735.h>

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

MCUFRIEND_kbv tft;

/** Initiate VescUart class */
VescUart UART;

int rpm;
float voltage;
float current;
int power;
float amphour;
float tach;
float distance;
float velocity;
float watthour;
float batpercentage;

SimpleKalmanFilter Filter1(2, 2, 0.01);

extern  unsigned char  SpeedometerBitmap[];
extern  unsigned char  BatteryFullBitmap[];
extern  unsigned char  BatteryHalfBitmap[];
extern  unsigned char  BatteryLowBitmap[];

void DrawSpeedometerBitmap()
{
     tft.drawBitmap(200,25,SpeedometerBitmap,30,30,WHITE);
}
void DrawBatteryFullBitmap()
{
     tft.drawBitmap(200,250,BatteryFullBitmap,30,30,WHITE);
}
void DrawBatteryHalfBitmap()
{
     tft.drawBitmap(250,250,BatteryHalfBitmap,30,30,WHITE);
}
void DrawBatteryLowBitmap()
{
     tft.drawBitmap(300,250,BatteryLowBitmap,30,30,WHITE);
}


void setup() {

  /** Setup Serial port to display data */
  Serial.begin(115200);

  /** Setup UART port (Serial1 on Atmega32u4) */
// Serial1.begin(19200);
  
  while (!Serial) {;}

  /** Define which ports to use as UART */
 UART.setSerialPort(&Serial);
   uint16_t ID = tft.readID();
   tft.begin(ID); // initialize the lcd 
   tft.setRotation(3);
   tft.fillScreen(BLACK);
   tft.setTextColor(WHITE);
   tft.setTextSize(5);
   
  tft.setCursor(35,135);
  tft.print("NLKEC'S E-BIKE");          // Startup screen (can't be more than 10 characters)

  delay(0);
  tft.setTextSize(3);
  tft.fillScreen(BLACK);
}
void loop() {
  
////////// Read values //////////  
//if ( UART.getVescValues() ) {

//  rpm = (UART.data.rpm)/7;          // The '7' is the number of pole pairs in the motor. Most motors have 14 poles, therefore 7 pole pairs
//  voltage = (UART.data.inpVoltage);
//  current = (UART.data.avgInputCurrent);
//  power = voltage*current;
//  amphour = (UART.data.ampHours);
//  watthour = amphour*voltage;
//  tach = (UART.data.tachometerAbs)/42;          // The '42' is the number of motor poles multiplied by 3
//  distance = tach*3.142*(1/1000)*0.72*(12/192);          // Motor RPM x Pi x (1 / meters in a mile or km) x Wheel diameter x (motor pulley / wheelpulley)
//  velocity = rpm*3.142*(60/1000)*0.72*(12/192);          // Motor RPM x Pi x (seconds in a minute / meters in a mile) x Wheel diameter x (motor pulley / wheelpulley)
//  batpercentage = ((voltage-38.4)/12)*100;          // ((Battery voltage - minimum voltage) / number of cells) x 100

////////// Filter //////////
  // calculate the estimated value with Kalman Filter
  float powerfiltered = Filter1.updateEstimate(power);
  
////////// LCD //////////
  
  tft.setCursor(25,25);
  tft.print("Km/h");
  if(velocity < 10){
  tft.setCursor(150,25);  //speed
  tft.print(" ");
  tft.print(velocity, 0);
  }
  else{
  tft.setCursor(150,25);
  tft.print(velocity, 0);
  }
  tft.setCursor(150,50);
  DrawSpeedometerBitmap();

  tft.setCursor(150,100);
  tft.print(voltage,0);     //voltage
  tft.setCursor(25,100);
  tft.print("V");
  
  tft.setCursor(50,125);
  tft.print(distance, 2);  //distance
  tft.setCursor(50,150);
  
// Second line
if(powerfiltered < 10){
  tft.setCursor(50,175);
  tft.print("   ");
  tft.print(powerfiltered, 0);
}
if(powerfiltered > 10 && powerfiltered < 100){
  tft.setCursor(50,175);
  tft.print("  ");
  tft.print(powerfiltered, 0);
}
if(powerfiltered > 100 && powerfiltered < 1000){     //Power W
  tft.setCursor(50,175);
  tft.print(" ");
  tft.print(powerfiltered, 0);
}
if(powerfiltered > 1000){
  tft.setCursor(50,175);
  tft.print(powerfiltered, 0);
}
  tft.setCursor(25,175);
  tft.print("W");
 
if(batpercentage < 100){
  tft.setCursor(50,200);
  tft.print(batpercentage,0);
 }
 else{
  tft.setCursor(50,200);
  tft.print(batpercentage,0);   //Battery
 }
  tft.setCursor(75,250);
  tft.print(amphour, 2);
  tft.setCursor(25,250);
  tft.print("Ah");
  tft.setCursor(150,250);
  DrawBatteryFullBitmap();
  DrawBatteryHalfBitmap();
  DrawBatteryLowBitmap();
  tft.println();
  
//}
// else
//{    
//  tft.println("Failed to get data!");
//}
 
  
//  tft.fillScreen(BLACK);
  delay(50);
  
}
