#include <EEPROM.h>
#include <VescUart.h>
#include <MCUFRIEND_kbv.h>

#define BLACK   0x0000
#define GREEN   0x06CA
#define CYAN    0x071D
#define WHITE   0xFFFF

MCUFRIEND_kbv tft;
VescUart UART;

bool i=false;
float rpm;
float voltage;
float current;
float power;
float amphour;
float tach;
float distance;
float velocity;
float watthour;
float batpercentage;

//postavljanje nemoguće vrijednosti kao početnu vrijednost
float rpmP = -1;
float voltageP = -1;
float currentP = -1;
float powerP = -1;
float amphourP = -1;
float tachP = -1;
float distanceP = -1;
float velocityP = -1;
float watthourP = -1;
float batpercentageP = -1;
float powerfilteredP = -1;
float rpmfilteredP = -1;
float currentfilteredP = -1;
float totalKmP = -1;

//total km
float totalKm;
int tripDP = 0;
int tripD;

//calc - conversions
float km = 0.001;
float ratio = 0.0625;
float kmAsecond = 0.06;


void setup() {

  Serial.begin(19200); //115200 defoult

  // Učitava s EEROM-a te sprema u varijablu
  totalKm = EEPROM.read(0);
  Serial.println(" ");
  Serial.print("Read from memory: ");
  Serial.println(totalKm);

  while (!Serial) {;}

  UART.setSerialPort(&Serial);
  uint16_t ID = tft.readID();
  tft.begin(ID);

  // Početni zaslon
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(5);   
  tft.setCursor(35,135);
  tft.print("NLKEC'S E-BIKE");
  delay(5000);

  // Konstantno vidljivo
  tft.fillScreen(BLACK);
  Serial.println("Start");
  tft.setTextSize(1);   
  tft.setCursor(30,30);
  tft.print("AMPS");
  tft.setCursor(30,110);
  tft.print("VELOCITY");
  tft.setCursor(30,240);
  tft.print("TRIP");
  tft.setCursor(180,30);
  tft.print("WATTS");
  tft.setCursor(180,240);
  tft.print("TOTAL");
  tft.setCursor(330,30);
  tft.print("VOLTAGE");
  tft.setCursor(330,240);
  tft.print("BATTERY");
  tft.setCursor(280,110);
  tft.print("REVOLUTION");
  tft.setCursor(140,70);
  tft.print("A");
  tft.setCursor(290,70);
  tft.print("W");
  tft.setCursor(440,70);
  tft.print("V");
  tft.setCursor(260,185);
  tft.print("KM/H");
  tft.setCursor(400,150);
  tft.print("RPM");
  tft.setCursor(140,290);
  tft.print("KM");
  tft.setCursor(290,290);
  tft.print("KM");
  tft.setCursor(430,290);
  tft.print("%");
  delay(0);

  /*//test
  //Velocity
  tft.setTextSize(9);
  tft.setCursor(40,130);
  tft.setTextColor(GREEN);
  tft.print("45.4");
  //Voltage
  tft.setTextSize(4);
  tft.setCursor(340,50);
  tft.setTextColor(CYAN);
  tft.print("46.5");
  //Amprage
  tft.setCursor(40,50);
  tft.setTextColor(CYAN);
  tft.print("85.5"); 
  //Trip
  tft.setCursor(40,270);
  tft.setTextColor(CYAN);
  tft.print("21.3");
  //Power
  tft.setCursor(190,50);
  tft.setTextColor(CYAN);
  tft.print(4500);
  //Battery
  tft.setCursor(350,270);
  tft.setTextColor(CYAN);
  tft.print("100");
  //Revolution
  tft.setCursor(300,130);
  tft.setTextColor(CYAN);
  tft.print("8064");
  //Total
  tft.setCursor(190,270);
  tft.setTextColor(CYAN);
  tft.print("356");*/
}
void loop() {
////////// Read values //////////  
if ( UART.getVescValues() ) {
  rpm = (UART.data.rpm)/13;          // The '7' is the number of pole pairs in the motor. Most motors have 14 poles, therefore 7 pole pairs
  voltage = (UART.data.inpVoltage);
  current = (UART.data.avgInputCurrent);
  power = voltage*current;
  amphour = (UART.data.ampHours);
  watthour = amphour*voltage;
  tach = (UART.data.tachometerAbs)/78;          // The '42' is the number of motor poles multiplied by 3
  distance = tach*3.142*km*0.6985*ratio;          // Motor RPM x Pi x (1 / meters in a mile or km) x Wheel diameter x (motor pulley / wheelpulley)
  velocity = rpm*3.142*kmAsecond*0.6985*ratio;          // Motor RPM x Pi x (seconds in a minute / meters in a mile) x Wheel diameter x (motor pulley / wheelpulley)
  batpercentage = ((voltage-38.4)/12)*100;          // ((Battery voltage - minimum voltage) / number of cells) x 100

  // Preračunava svaki prođeni kilometar te ga zapisuje u EEROM nesmije se ponavljati svaki loop
  tripD = distance;
  if(tripDP!=tripD){
  totalKm += 1;
  EEPROM.write(0, totalKm);
  tripDP=tripD;

  //test
  Serial.print("Total KM: ");
  Serial.println(totalKm);
  }
// test

 /* Serial.println(" ");
    Serial.print("rpm  ");Serial.println(UART.data.rpm);
    Serial.print("inV  ");Serial.println(UART.data.inpVoltage);
    Serial.print("Amph  ");Serial.println(UART.data.ampHours);
    Serial.print("Abs  ");Serial.println(UART.data.tachometerAbs);  
    Serial.print("km/h  "); Serial.println(velocity);
    Serial.print("km/h PPP "); Serial.println(velocityP);
    Serial.print("Distance  "); Serial.println(distance);
    Serial.print("Voltage  "); Serial.println(voltage);
    Serial.print("Current  "); Serial.println(current);
    Serial.print("Power  "); Serial.println(power);
    Serial.print("amphour  "); Serial.println(amphour);
    Serial.print("batpercentage  "); Serial.println(batpercentage);
    Serial.print("RPM 2  ");Serial.println(rpm);
*/

// TFT
  if(i==false){
  tft.setTextSize(1);
  tft.setCursor(50,150);
  tft.setTextColor(BLACK);
  tft.println("Failed to get data!");
  i=true;
  }

  tft.setTextSize(9);

// Ispisuje vrijednosti na TFT displej
if(velocityP!=velocity){
  tft.setCursor(40,130);
  tft.setTextColor(BLACK);
  tft.print(velocityP, 1);
  tft.setCursor(40,130);
  tft.setTextColor(GREEN);
  tft.print(velocity, 1);
}
  tft.setTextSize(4);
if(voltageP!=voltage){
  tft.setCursor(340,50);
  tft.setTextColor(BLACK);
  tft.print(voltageP, 1);
  tft.setCursor(340,50);
  tft.setTextColor(CYAN);
  tft.print(voltage, 1);
}
if(currentP!=current){
  tft.setCursor(40,50);
  tft.setTextColor(BLACK);
  tft.print(currentP, 1);
  tft.setCursor(40,50);
  tft.setTextColor(CYAN);
  tft.print(current, 1);  
}
if(distanceP!=distance){
  tft.setCursor(40,270);
  tft.setTextColor(BLACK);
  tft.print(distanceP, 1);
  tft.setCursor(40,270);
  tft.setTextColor(CYAN);
  tft.print(distance, 1);
}
if(powerP!=power){
  tft.setCursor(190,50);
  tft.setTextColor(BLACK);
  tft.print(powerP, 0);
  tft.setCursor(190,50);
  tft.setTextColor(CYAN);
  tft.print(power, 0);
}
if(batpercentageP!=batpercentage){ 
  tft.setCursor(350,270);
  tft.setTextColor(BLACK);
  tft.print(batpercentageP, 0);
  tft.setCursor(350,270);
  tft.setTextColor(CYAN);
  tft.print(batpercentage, 0);
}
if(rpmP!=rpm){
  tft.setCursor(300,130);
  tft.setTextColor(BLACK);
  tft.print(rpmP, 0);
  tft.setCursor(300,130);
  tft.setTextColor(CYAN);
  tft.print(rpm, 0);
}
if(totalKmP!=totalKm){
  tft.setCursor(190,270);
  tft.setTextColor(BLACK);
  tft.print(totalKmP, 0);
  tft.setCursor(190,270);
  tft.setTextColor(CYAN);
  tft.print(totalKm, 0);
}
  // Svaki loop stavlja trenutnu vrijednost u prijašnju radi brisanja brojka s TFT-a
  rpmP = rpm;
  voltageP = voltage;
  currentP = current;
  powerP = power;
  amphourP = amphour;
  tachP = tach;
  distanceP = distance;
  velocityP = velocity;
  watthourP = watthour;
  batpercentageP = batpercentage;
  powerP = power;
  rpmP = rpm;
  currentP = current;
  totalKmP = totalKm;
}
else
{    
  tft.setTextSize(1);
  tft.setCursor(50,150);
  tft.setTextColor(WHITE);
  tft.print("Failed to get data!");
  Serial.println("Failed to get data!");
  i=false;
}
}
