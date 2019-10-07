/**************
 *  ---------------------- OUTPUT & INPUT Pin table ---------------------
 *  +---------------+-------------------------+-------------------------+
 *  |   Board       | INPUT Pin               | OUTPUT Pin              |
 *  |               | Zero-Cross              |                         |
 *  +---------------+-------------------------+-------------------------+
 *  | Uno           | D2 (NOT CHANGABLE)      | D0-D1, D3-D20           |
 *  +---------------+-------------------------+-------------------------+
 */

#include <RBDdimmer.h>//
#include <Wire.h>                           // Подключаем библиотеку Wire                           
#include <TimeLib.h>                        // Подключаем библиотеку TimeLib
#include <DS1307RTC.h>
#include <avr/pgmspace.h>


#define USE_SERIAL  Serial
#define outputPin  12 
#define zerocross  5 // for boards with CHANGEBLE input pins

const byte startLampHour PROGMEM = 6;
const byte startLampMinute PROGMEM = 30;
const byte accuracy PROGMEM = 3;

const byte stopLampHour PROGMEM = 23;
const byte stopLampMinute PROGMEM = 10;

int sensorPin = A0;


dimmerLamp dimmer(outputPin); //initialase port for dimmer for MEGA, Leonardo, UNO, Arduino M0, Arduino Zero

//const char *monthName[12] = {
//  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
//  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
//};
//
//tmElements_t tm;


// lampState - состояние лампы. 0 - выключена 1 - нагревается 2 - горит - 3 постепенно выключается


byte outVal = 0;
byte lampState = 0;
tmElements_t tm; 
             



void setup() {
//  bool parse=false;
//  bool config=false;
//
//  // get the date and time the compiler was run
//  if (getDate(__DATE__) && getTime(__TIME__)) {
//    parse = true;
//    // and configure the RTC with this info
//    if (RTC.write(tm)) {
//      config = true;
//    }
//  }

  Serial.begin(9600);
  delay(200);
//  while (!Serial) ; // wait for Arduino Serial Monitor
//  delay(200);
//  if (parse && config) {
//    Serial.print("DS1307 configured Time=");
//    Serial.print(__TIME__);
//    Serial.print(", Date=");
//    Serial.println(__DATE__);
//  } else if (parse) {
//    Serial.println("DS1307 Communication Error :-{");
//    Serial.println("Please check your circuitry");
//  } else {
//    Serial.print("Could not parse info from the compiler, Time=\"");
//    Serial.print(__TIME__);
//    Serial.print("\", Date=\"");
//    Serial.print(__DATE__);
//    Serial.println("\"");
//  }

  
  dimmer.begin(NORMAL_MODE, OFF); //dimmer initialisation: name.begin(MODE, STATE) 
}

void loop() 
{

  int sensorValue = analogRead(sensorPin);
  Serial.print("Sensor data:");
  Serial.print(sensorValue);
  Serial.print("\n");

  if (lampState == 0 && (outVal != 0)){
    dimmer.setPower(0);
  }
             
  if (RTC.read(tm)) {
    Serial.print("Ok, Time = ");

    int hour = tm.Hour;
    int minute = tm.Minute; 

    
    print2digits(hour);
    Serial.write(':');
    print2digits(minute);
    Serial.write('\n');

    if (hour == startLampHour && (minute>=startLampMinute && (minute <= (startLampMinute + accuracy)))){
        Serial.print("lampState = ");
        Serial.println(lampState);
        if (lampState == 0){
          lampState = 1;
          startLamp();
        }
    } else if (hour == stopLampHour && (minute>=stopLampMinute && (minute <= (stopLampMinute + accuracy)))){
        Serial.print("lampState = ");
        Serial.println(lampState);
        if (lampState == 2){
          lampState = 3;
          stopLamp();
        }
    }

    //TODO:  Убрать хардкод
    //резкое выключение после плавного нагрева
    if ((hour == startLampHour+1 && minute == (startLampMinute+10))&& lampState == 2){
      outVal = 0;
      dimmer.setPower(outVal);
      lampState=0;
      dimmer.setState(OFF);     
    }
    //Резкое влючение перед плавным вечерним выключением
    if ((hour == stopLampHour-0 && minute == (stopLampMinute-10))&& lampState == 0){
      dimmer.setState(ON);      
      Serial.print("STOP OUTPUT VALUE: ");
      Serial.print(outVal);
      outVal = 90;
      dimmer.setPower(outVal);
      lampState=2;
    }

    
    
  } else {
  
  }
  delay(1000);
}









void startLamp(){
  Serial.println("StartLAMP!");
  if (dimmer.getState() == OFF){
    dimmer.setState(ON);
  }
  
  for (int i = 0; i < 99; i++){
    USE_SERIAL.println(outVal); 
    dimmer.setPower(outVal); // name.setPower(0%-100%)
    outVal = outVal+1;  
    delay(1000);
  }
  lampState = 2;
  
   
}


void stopLamp(){
  if (dimmer.getState() == OFF){
    dimmer.setState(ON);
  }

  for (int i = 99; i >= 0; i--){
    
    USE_SERIAL.println(outVal); 

    if (outVal>=0){
    dimmer.setPower(outVal); // name.setPower(0%-100%)
    outVal = outVal-1;
    } else {
      dimmer.setPower(0);
      break;
    }
      
    delay(1000);

    
  }

  outVal = 0;
  dimmer.setPower(outVal);
  lampState = 0;
  dimmer.setState(OFF);

  
}












void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

//bool getTime(const char *str)
//{
//  int Hour, Min, Sec;
//
//  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
//  tm.Hour = Hour;
//  tm.Minute = Min;
//  tm.Second = Sec;
//  return true;
//}
//
//bool getDate(const char *str)
//{
//  char Month[12];
//  int Day, Year;
//  uint8_t monthIndex;
//
//  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
//  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
//    if (strcmp(Month, monthName[monthIndex]) == 0) break;
//  }
//  if (monthIndex >= 12) return false;
//  tm.Day = Day;
//  tm.Month = monthIndex + 1;
//  tm.Year = CalendarYrToTm(Year);
//  return true;
//}
