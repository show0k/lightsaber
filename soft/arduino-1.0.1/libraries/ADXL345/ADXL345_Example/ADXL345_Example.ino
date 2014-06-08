
//Barrett Anderies
//Sep. 15, 2012
//This code was written to learn and pracitice using I2C protocol and hardware interrupts on the Arduino. The single and double tap registers on the ADXL345 can be read without using interrupts, 
//simplifying the code, but as mentioned, this was written to demonstrate how interrupts can be used. Get the ADXL345 datasheet here: http://www.analog.com/static/imported-files/data_sheets/ADXL345.pdf
//

#include <Wire.h>                                                                //include the Wire library (necessary for I2C and SPI protocols)
#include "I2Cdev.h"
#include "ADXL345.h"

#define THRESH_TAP 60 
#define THRESH_ACT 30 
//Tap threshold value
#define OFSX 0                                                                   //X-axis offset value  
#define OFSY 0                                                                   //Y-axis offset valuevalue
#define OFSZ 0                                                                   //Z-axis offset value
#define DUR 30                                                                //Tap duration value
#define LATENT 0x40                                                              //Tap Latency value
#define THRESH_INACT 0                                                           //Inactivity threshold value
#define TIME_INACT 0                                                             //Inactivity time value
#define ACT_INACT_CTL 0                                                          //Axis enable control for activity and inactivity detection value
#define THRESH_FF 0                                                              //Free-fall threshold value
#define TIME_FF 0                                                                //Free-fall time value
#define TAP_AXES B00000111                                                       //Axis control for single tap/double tap value 
                                                              //Data rate and power mode control value

#define FIFO_CTL 0                                                               //FIFO control value

int16_t data_x ;
int16_t data_y ;
int16_t data_z ;
int16_t result ;
volatile char acc_state = 0;
int tap_int, activity_int = 0 ;                                                       //declare and initialize boolean variables to track tap type



ADXL345 myACC = ADXL345();                                                        //create an instance of class ADXL345() named myACC

void setup()
{
  Serial.begin(9600);                                                            //join the serial buss at 9600 baud
                                       //attach an interrupt on digital pin 2 (interrupt 0) for rising signal                                       //attach an interrupt on digital pin 3 (interrupt 1) for rising signal
  pinMode(4, OUTPUT);                                                            //set LED pins to output
  pinMode(5, OUTPUT);
   myACC.initialize();
   Serial.println("Testing device connections...");
  Serial.println(myACC.testConnection() ? "ADXL345 connection successful" : "ADXL345 connection failed");
  
  //TAP detection
  attachInterrupt(0, interrupt0, RISING);
   attachInterrupt(1, interrupt1, RISING);      
  myACC.setTapThreshold(THRESH_TAP);                                               //set registers on the ADXL345 to the above defined values (see datasheet for tuning and explanations)
  myACC.setTapDuration(DUR);
  myACC.setIntSingleTapEnabled(true);
  myACC.setIntSingleTapPin(0);
  myACC.setTapAxisXEnabled(true);
  myACC.setTapAxisYEnabled(true);
  myACC.setTapAxisZEnabled(true);
  
  //Activity detection
  myACC.setActivityThreshold(THRESH_ACT);
  myACC.setIntActivityEnabled(true);
  myACC.setIntActivityPin(1);
  myACC.setActivityXEnabled(true);
  myACC.setActivityYEnabled(true);
  myACC.setActivityZEnabled(true);
  myACC.setActivityAC(true);


  myACC.setSleepEnabled(false);
  //myACC.setIntSingleTapPin(TAP_INT_PIN);
 myACC.setOffset(70,70,70);


  delay(2000);

}

void loop()                                                                      //main loop
{
  /*myACC.getAcceleration(&data_x, &data_y,&data_z);       
 Serial.print("result:\t"); 
  Serial.println(result);
    Serial.println("accel:\t");
    Serial.print(data_x); Serial.print("\t");
    Serial.print(data_y); Serial.print("\t");
    Serial.println(data_z);
    //use readDATA functions (see library) and print values
  result = sqrt((data_x*data_x) + (data_y*data_y) + (data_z*data_z)) ;

  delay(1000);/*
  /*if (result > 175)
  {
    //delay(10);
     myACC.getAcceleration(&data_x, &data_y,&data_z);                                                     //use readDATA functions (see library) and print values
    result = sqrt((data_x*data_x) + (data_y*data_y) + (data_z*data_z)) ;
    if (result > 200)
     Serial.println("long");
     else if (result > 160)
      Serial.println("short");
      Serial.println(result);
     
 
    
  }*/
  tap_int = myACC.getIntSingleTapSource();
  activity_int = myACC.getIntActivitySource();
  if (tap_int == 64)
  {
    acc_state = 1 ;
  }

  if (activity_int == 16)
  {
    acc_state = 2 ;
  }
  delay(100);
  
     //! If this register is not read every iteration the interrupts on the ADXL345 will not reset, and therefore not function)
  //Serial.println(int_source, BIN);
  
  

}

void interrupt0()                                                              //a RISING on interrupt pin 2 (interrupt 0) would mean a single tap (from how we have wired and configured the ADXL345), therefore, set singleTap to true
{
  Serial.println("Single Tap!");
 /* if(acc_state == 1)
    Serial.println("Single Tap!");
  else if(acc_state == 2)
    Serial.println("activity!");
  acc_state = 0; */

}

void interrupt1()                                                              //a RISING on interrupt pin 2 (interrupt 0) would mean a single tap (from how we have wired and configured the ADXL345), therefore, set singleTap to true
{
    Serial.println("activity!");
  
}




