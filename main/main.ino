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
#define POT_PIN A1
#define sensorPin A0

const byte startLampHour PROGMEM = 6;
const byte startLampMinute PROGMEM = 30;
const byte accuracy PROGMEM = 3;

const byte stopLampHour PROGMEM = 25;
const int stopLampMinute PROGMEM = 31;

int level;



dimmerLamp dimmer(outputPin); //initialase port for dimmer for MEGA, Leonardo, UNO, Arduino M0, Arduino Zero

class Lamp{
  public:
  byte outVal = 0;
  byte lampState = 0;
  


  void startLamp(){

      if (lampState==0){
        lampState=2;
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
   
  }

    void on(){
      
      dimmer.setState(ON);      
      Serial.print("STOP OUTPUT VALUE: ");
      Serial.print(outVal);
      outVal = 90;
      dimmer.setPower(outVal);
      lampState=2;
      
    }


    
    void off(){
      outVal = 0;
      dimmer.setPower(outVal);
      lampState=0;
      dimmer.setState(OFF);     
    }

    void stopLamp(){
      if (lampState==2){
            lampState=3;
            if (dimmer.getState() == OFF){
              dimmer.setState(ON);
            }
      
            for (; outVal > 0; --outVal){
          
              USE_SERIAL.println(outVal); 
      
              
              dimmer.setPower(outVal); // name.setPower(0%-100%)
//              outVal = outVal-1;
            
              delay(7000);
            }
      
            outVal = 0;
            dimmer.setPower(outVal);
            lampState = 0;
            dimmer.setState(OFF);
      
        
          }
    }
  
};

















// lampState - состояние лампы. 0 - выключена 1 - нагревается 2 - горит - 3 постепенно выключается



tmElements_t tm; 
Lamp lamp;             






void setup() {

  pinMode(POT_PIN, INPUT);
  Serial.begin(9600);
  delay(200);


  
  dimmer.begin(NORMAL_MODE, OFF); //dimmer initialisation: name.begin(MODE, STATE) 
}





void loop() 
{

  
  level = analogRead(POT_PIN);
  Serial.print("POT data:");
  Serial.print(level);
  Serial.print("\n");

  
  
  int sensorValue = analogRead(sensorPin);
  Serial.print("Sensor data:");
  Serial.print(sensorValue);
  Serial.print("\n");

  if (lamp.lampState == 0 && (lamp.outVal != 0)){
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
        lamp.startLamp();
    } else if (hour == stopLampHour && (minute>=stopLampMinute && (minute <= (stopLampMinute + accuracy)))){
        Serial.print("lampState = ");
        Serial.println(lamp.lampState);
        lamp.stopLamp();
    }

   
    if ((hour == startLampHour+2 && minute == (startLampMinute+10))&& lamp.lampState == 2){
      lamp.off();
    }
    //Резкое влючение перед плавным вечерним выключением
    //23:20     23:16  -- h1 59     h1 -59  
//    boolean check = (((stopLampHour - hour <= 1) && (stopLampMinute-minute<=59&&stopLampMinute>=-59))&& lamp.lampState == 0);
//    Serial.print("\n");
//    Serial.print("Check:");
//    Serial.print(check);
//    Serial.print("\n");  
//    if (((stopLampHour - hour <= 1) && (stopLampMinute-minute<=59&&stopLampMinute>=-59))&& lamp.lampState == 0){
//      lamp.on();
//    }

//      Serial.print(hour > startLampHour+1 && (hour < stopLampHour));
//      if (hour > startLampHour+1 && (hour < stopLampHour)){
          //освещенность ниже заданного
//          if (sensorValue>=level){
//            if (lamp.lampState==0){
//              lamp.on();   
//            }
//          } else{
//            if (lamp.lampState==2){
//            lamp.off();   
//           }
//          }
//      }


      if (hour>startLampHour+1){
        if(hour<stopLampHour||(hour==stopLampHour&&minute<=stopLampMinute)){
          
           if (sensorValue>=level){
            if (lamp.lampState==0){
              lamp.on();   
            }
          } else{
            if (lamp.lampState==2){
              lamp.off();   
            }
          } 
          
        }
      }

    
    
  } 
   delay(1000);
}





















void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
