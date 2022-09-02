// All definition and imports 

//#include <LiquidCrystal_I2C.h>
#include <Q2HX711.h>
#include "Average.h"
#include<Servo.h>
//#include "spo2_algorithm.h"
#include <Wire.h>
#include <DHT.h>
#include "MAX30105.h"
#include "heartRate.h"
#define DHTPIN 6
#define DHTTYPE DHT11
#define MAX_BRIGHTNESS 255
#include "HX710B.h"
#include<math.h>




// Humidity sensor variable declaration
//LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN,DHTTYPE);

// spo2 sensor variable declaration

MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;



//servo and servoCycles
Servo Myservo;
int pos;
int ambuMode; //0 == do nothing, 1 == automated, 2 == assisted
int ambuDelay[3];//0: delay 15 == > 20 bpm; 1: D = 20, bpm = 15; 2: D = 25, bpm = 12-13 ; bpm x delay = 300
int breathsPerMin;


// Pressure sensor Variables declaration


const byte MPS_OUT_pin = 2; // OUT data pin
const byte MPS_SCK_pin = 4; // clock data pin
int avg_size = 10; // #pts to average over
int sensorpin=A0;
Q2HX711 MPS20N0040D(MPS_OUT_pin, MPS_SCK_pin); // start comm with the HX710B
Average<long> ave(avg_size);



void read_sensors()
{

  float temperature_body = particleSensor.readTemperature();


  
  
}


void setup()
{

  
  Serial.begin(9600); // start the serial port
  


  //
  dht.begin();
  

  // Servo setup 
  Myservo.attach(3);
  ambuDelay[0] = 15;
  ambuDelay[1] = 20;
  ambuDelay[2] = 25;
  ambuMode = 2;
  breathsPerMin = 2;

  // spo2 setup

    // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1);
  }


  particleSensor.enableDIETEMPRDY(); //Enable the temp ready interrupt. This is required.

  
}

void loop()
{
  

  read_sensors();
  ave.push(MPS20N0040D.read());
  //Serial.println(ave.mean());
  
  float ecg=analogRead(A0);
  float hum=dht.readHumidity();
  if(isnan(hum))
  hum=60;
  
  float temp=dht.readTemperature();
  if(isnan(temp))
  temp=31;
  float pressure=ave.mean();
  
  Serial.print(temp);
  Serial.print("|");
  Serial.print(hum);
  Serial.print("|");
  Serial.print(100.5);
  Serial.print("|");
  Serial.print(99.5);
  Serial.print("|");
  Serial.print(pressure/93000);
  Serial.print("|");
  Serial.print(ecg/2);
  Serial.println("|");
  
  if(ambuMode == 2)
  {
      
       if(ave.mean()>9950000.00)
      {
        for(pos=0;pos<=180;pos=pos+10)
        {
          
          Myservo.write(pos);
          //read_sensors();
         // delay(1);
        }
      }
      else if(ave.mean()<9000000.00)
      {
        for(pos=180;pos>=0;pos=pos-10)
        {
          Myservo.write(pos);
          //read_sensors();
         // delay(1);
        }
      }
      else 
      {
          Myservo.write(pos);
//        lcd.print("Resting");
      }
      delay(100);

      
      
  }
  else if(ambuMode == 1){
    
      for (pos = 0; pos <= 180; pos = pos + 3) {
        
      Myservo.write(pos);
      //read_sensors();
      delay(ambuDelay[breathsPerMin]); //delay 15 == > 20 bpm; D = 20, bpm = 15; D = 25, bpm = 12-13 
    }
    delay(500);
  
    for (pos = 180; pos >= 0; pos = pos - 2) {
      
      Myservo.write(pos);
      //read_sensors();
      delay(ambuDelay[breathsPerMin]);
    }
    delay(500);
  }
 
  


  
}
