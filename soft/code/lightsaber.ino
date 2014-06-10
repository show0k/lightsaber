//SD

//ADXL345
#include <Wire.h>                                                                //include the Wire library (necessary for I2C and SPI protocols)
#include <I2Cdev.h> 
#include <ADXL345.h> 

#include <WaveHC.h>
#include <WaveUtil.h>

#define DEBUG true
#define SWING_THREESHOLD 1.15
#define HIT_THREESHOLD 1.75

//ADXL
#define THRESH_TAP 70 
#define THRESH_ACT 22 
#define DUR 20    


SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the volumes root directory
FatReader file;   // This object represent the WAV file for a pi digit or period
WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time
      
//TODO


static char* start = "start0.wav" ;
static char* hit= "hit1.wav" ;
static char* idle= "idle0.wav"  ;
static char* swing= "swing2.wav"  ;

volatile char g_state = 1;
volatile char g_old_state = 0;
int tap_int, activity_int = 0 ;                                                       //declare and initialize boolean variables to track tap type
ADXL345 myACC = ADXL345();   
/*
 * Define macro to put error messages in flash memory
 */
#define error(msg) error_P(PSTR(msg))


//TODO
//Mettre la led en sortie

void setup() {

  Serial.begin(9600);  

  pinMode(8, OUTPUT) ;
  digitalWrite(8,HIGH); 
                                                       //set LED pins to output         
  PgmPrintln("Lightsaber");
  if (!card.init()) {
    error("Card init. failed!");
  }
  if (!vol.init(card)) {
    error("No partition!");
  }
  if (!root.openRoot(vol)) {
    error("Couldn't open dir");
  }
  PgmPrintln("Files found:");
  root.ls();

  //pinMode(10, OUTPUT);     // 


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



  playcomplete(start) ;
  Serial.println("Lets Begin !");

}



void loop() { 
  myACC.getIntSingleTapSource();
  myACC.getIntActivitySource();

  switch(g_state) {
            case 1 :
              if (!wave.isplaying)
                  playfile(idle) ;
                  if(DEBUG)
                 //   Serial.print("idle");
              break;
            case 2 :
                delay(10);
                if(g_state == 3)
                {
                    if(DEBUG)
                  Serial.print("HIT  ");
                  playcomplete(hit) ;
                }
                else if(g_old_state == 1)
                {
                  playfile(swing) ;
                  //delay(800);
                  if(DEBUG)
                    Serial.print("swing");
                }
                
                myACC.getIntActivitySource();
                change_state(1);
                break ;
            case 3 :
              if(DEBUG)
                Serial.print("HIT  ");
              playfile(hit) ; 
              //delay(200);
              
              change_state(1);
              myACC.getIntSingleTapSource();
                //if (g_state == 2)
                //   g_state = 0 ;*/
                break ;
            default :
                //g_state = 0;
                break ;

  }
}

/////////////////////////////////// HELPERS


void change_state(char new_state)
{
  g_old_state = g_state;
  g_state = new_state ;

}

/*
 * print error message and halt
 */
void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
 // while(1);
}
/*
 * print error message and halt if SD I/O error
 */
void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  //while(1);
}

/*
 * Play a file and wait for it to complete
 */

void playcomplete(char *name) {
  playfile(name);
  while (wave.isplaying);
  
  // see if an error occurred while playing
  sdErrorCheck();
}
/*
 * Open and start playing a WAV file
 */
void playfile(char *name) {
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
  if (!file.open(root, name)) {
    PgmPrint("Couldn't open file ");
    Serial.print(name); 
    return;
  }
  if (!wave.create(file)) {
    PgmPrintln("Not a valid WAV");
    return;
  }
  // ok time to play!
  wave.play();
}

void interrupt0()                                                              //a RISING on interrupt pin 2 (interrupt 0) would mean a single tap (from how we have wired and configured the ADXL345), therefore, set singleTap to true
{
  change_state(3);
}

void interrupt1()                                                              //a RISING on interrupt pin 2 (interrupt 0) would mean a single tap (from how we have wired and configured the ADXL345), therefore, set singleTap to true
{
    change_state(2);
}