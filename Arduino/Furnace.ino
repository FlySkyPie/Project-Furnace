#include <PID_v1.h>     //https://github.com/br3ttb/Arduino-PID-Library
#include "max6675.h"    //https://github.com/adafruit/MAX6675-library

//PID contorllor
double TargetTemperature, Input, Output;
double Kp=2, Ki=5, Kd=2;
PID *PIDController;
int MaxOutPut = 255;

//thermal snesor
int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;
MAX6675 ThermoCouple(thermoCLK, thermoCS, thermoDO);

//output pin
int HeatingPin = 9;

unsigned long LastUpload;

//System status
bool Online = 0;

//reset
void(* resetFunc) (void) = 0; //declare reset function @ address 0

void setup() 
{
  //initial output
  pinMode(HeatingPin, OUTPUT);
  analogWrite(HeatingPin, 0);

  //initial time
  LastUpload = millis();
    
  //initial serial
  Serial.begin(9600);

  delay(500);   // wait for MAX chip to stabilize
}

 
void loop() 
{
  //check upload data
  if( millis() - LastUpload >= 1000)  //1 second do once
  {
    LastUpload = millis();            //update time
    
    double Temperature = ThermoCouple.readCelsius();    //read temperature
    Input = ThermoCouple.readCelsius();    //read temperature
    
    //send temperature
    String data = "T:" + String (Temperature) + "\n";
    Serial.print(data);
    
    if(Online)
    {
      PIDController->Compute();         //PID calculate
      analogWrite(HeatingPin, Output);  //set output

      //send PWM work period
      String data = "O:" + String(Output) + "\n";
      Serial.print(data);
    }
  }
}

void serialEvent()
{
  //read serial command
  if (Serial.available() > 0) 
  {
    String str = Serial.readStringUntil('\n');

    //split string
    String firstVal, secondVal;
    for (int i = 0; i < str.length(); i++) 
    {
      if (str.substring(i, i+1) == ":") {
        firstVal = str.substring(0, i);
        secondVal = str.substring(i+1);
        break;
      }
    }

    if( firstVal.length() > 0 && secondVal.length() > 0)  //It is command?
    {
      double *setting;
      
      if( firstVal == "Kp") //set Kp
        setting = &Kp;
      if( firstVal == "Ki") //set Ki
        setting = &Ki;
      if( firstVal == "Kd") //set Kd
        setting = &Kd;
      if( firstVal == "Target") //set temperature of target
        setting = &TargetTemperature;

      *setting = secondVal.toInt();
      return;   //operate finish, exit
    }

    if(str=="close")
    {
      resetFunc();
    }
    if( str == "start")
    {
      //setup PID
      PIDController = new PID(&Input, &Output, &TargetTemperature,Kp, Ki, Kd, DIRECT);
      PIDController->SetOutputLimits(0, MaxOutPut);  //tell the PID to range between 0 and the full window size
      PIDController->SetMode(AUTOMATIC);             //turn the PID on  

      //return info about furnace
      String info ="";
      info += "W:" +  String(TargetTemperature) + "\n";
      info += "Kp:" + String(Kp) + "\n";
      info += "Ki:" + String(Ki) + "\n";
      info += "Kd:" + String(Kd) + "\n";
      Serial.print(info);

      //set furnace status
      Online = 1;
      return;
    }
  }  
}

