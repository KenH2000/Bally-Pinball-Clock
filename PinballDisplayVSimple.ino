//////////////////
//rev Oct 2019
////////////////// 
#include <Wire.h>  
#include <Time.h>
#include <Timerus.h>    //edited timermaster library to add micros() class(Timerus)
#define DS1307_ADDRESS 0x68
//init variables ********************************************************************
boolean debug=false;  //when true, clock displays on PC instead of hardware
//***********************************************************************************
Timerus tus;  //uses micros()
Timer t;      //uses millis()
byte displaydig[]={13,12,11,10,9,8};
byte abcd[]={2,3,4,5};
byte blanking=A3;byte strobe=A2;  //tie strobe(LE) HIGH, but blanking is needed to prevent digit bleed 
byte pinDisplay=A0;
byte zero = 0x00;             //workaround for issue #527
boolean flashon;
int flashinterval=200;
byte onz;                      //counter for display interrupt
byte dig[6];                   //digits that have the value for display
boolean hr24=false;
byte modeflag=0;
unsigned long syncinterval = 18000000; //18000000 ms is 5hrs interval to sync time 
int dinterval=3000;
//unsigned long syncinterval = 5000;       //for debug
int savSec;                   //for display update every second
const int buttonMin = 7;      //the number of the pushbutton pins
const int buttonHr = 6;      
const int buttondelay=250;
int synccount=0;
boolean errorflag;
unsigned long flashMillis=0;
//unsigned long previousMicro=0;
unsigned long buttonPushedMillis;

void setup() {      
  pinMode(pinDisplay, OUTPUT); digitalWrite(pinDisplay, LOW); //init the display
  int i;
  for (i=0;i<4;i++){
  pinMode(abcd[i], OUTPUT);              //init the abcd pins
  }
  for (i=0;i<6;i++){
    pinMode(displaydig[i], OUTPUT);      //init the digit pins
  }
  pinMode(blanking, OUTPUT);pinMode(strobe, OUTPUT); 
  //timers
  tus.every(dinterval,displaycall);     //need micros() to prevent flicker
  t.every(syncinterval,syncTime);       //resyncs after interval
  pinMode(buttonMin, INPUT_PULLUP);pinMode(buttonHr, INPUT_PULLUP);   //initialize pushbutton pin as an input with pullup 
  Wire.begin();
  if (debug){Serial.begin(9600);}
  syncTime();
  digitalWrite(strobe, HIGH);           //LE can always be enabled, but blanking needs to be pulsed
}

// the loop routine runs over and over again forever:
void loop() {
int i;int j;  
t.update();
tus.update();

checkButtons();
int mysec=second();        //if mysec != savSec, update display 
  if (modeflag==1)         //set hours
  {
  dig[0]=0xF;dig[1]=0xF;   //0xF (hex F) blank dig[n]
  }
  else if (modeflag==2)    //set minutes
   {
    dig[0]=0xF;dig[1]=0xF; //0xF (hex F) blank dig[n]
   }
  else if (modeflag==3) //set 24 or 12 hr
  {
   dig[5]=0xF;dig[4]=0xF;dig[3]=0xF;dig[2]=0xF;
   if (hr24) {dig[1]=2;dig[0]=4;}else{dig[1]=1;dig[0]=2;}
  } else {              //show time
    if (mysec!=savSec) {
      savSec=mysec;
        if (debug){
          debugClockDisplay();displayTime();
        }else{
          displayTime(); 
        }
    }
  }
} //*********************end main loop********************************************

void displaycall() {
  digitalWrite(blanking,HIGH);
  digitalWrite(displaydig[onz], LOW);    //turn off the last z value
  onz++;if (onz==6){onz=0;}
  writeDig(dig[onz]);                    //turn on the new z value
  digitalWrite(displaydig[onz], flashit());       
  digitalWrite(blanking,LOW);   
}

void checkButtons(){
  unsigned long currentMillis = millis();
  if(currentMillis - buttonPushedMillis > buttondelay) {
    int buttonStateHr;int buttonStateMin;
    buttonStateHr = digitalRead(buttonHr);
    buttonStateMin= digitalRead(buttonMin);
    if (buttonStateHr==LOW){
      buttonPushedMillis = currentMillis;
      chgmodeflag(); 
      //delay(buttondelay);              //using delay distorts clock display 
      if (modeflag==1) displayTime();
    }
    if (buttonStateMin==LOW){
      buttonPushedMillis = currentMillis;
      if (modeflag==1) {dig[0]=0xF;dig[1]=0xF;
        increaseHour();
      }else if (modeflag==2){dig[0]=0xF;dig[1]=0xF;
        increaseMin(); 
      }else if (modeflag==3){
        if (hr24) {hr24=false;}else{hr24=true;}
      }
    //delay(buttondelay);                //using delay distorts clock display
    }
  }
}  //end function   

void increaseHour() {   
byte myHour=hour();  
myHour++;
if (myHour>23) myHour=0;
byte myMin=minute();
byte mySec=second();
byte myDay=day();
byte myMonth=month();
int myYear=year();   
setTime(myHour,myMin,mySec,myDay,myMonth,myYear);
displayTime();
}

void increaseMin() {
  byte myHour=hour();
  byte myMin=minute()+1;
  byte mySec=second();
  byte myDay=day();
  byte myMonth=month();
  int myYear=year(); 
  if (myMin>59) {myMin=0;}
  setTime(myHour,myMin,mySec,myDay,myMonth,myYear);
  displayTime();
}

void displayTime() {
  //dig[5]=1;dig[4]=8;dig[3]=1;dig[2]=8;dig[1]=1;dig[0]=8;return;  //for debug
  byte myHour=hour();
  byte myMin=minute();
  byte mySec=second();
  dig[3]=myMin/10;
  dig[2]=myMin%10;
  dig[1]=mySec/10;
  dig[0]=mySec%10;  

  if (modeflag==0){
    if (hr24){
      dig[5]=hour()/10;
      dig[4]=hour()%10;
    }else{
      dig[4]=hourFormat12()%10;
      if (hourFormat12()/10==0){
        dig[5]=0xF;
      }else{
        dig[5]=hourFormat12()/10;
      }
    } 
  }else{
    dig[5]=hour()/10; 
    dig[4]=hour()%10;
  }
}  //end function

void syncTime(){
 // Reset the register pointer
  Wire.beginTransmission(DS1307_ADDRESS);

  byte zero = 0x00;
  Wire.write(zero);
  Wire.endTransmission();

  int bytesread = Wire.requestFrom(DS1307_ADDRESS, 9);
  if (bytesread == 9){
  errorflag=false;
  int mySec = bcdToDec(Wire.read());
  int myMin = bcdToDec(Wire.read());
  int myHour = bcdToDec(Wire.read() & 0b111111); //24 hour time
  int myweekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
  int mymonthDay = bcdToDec(Wire.read());
  int myMonth = bcdToDec(Wire.read());
  int myYear = bcdToDec(Wire.read());
  Wire.read();                         //control register
  hr24 = Wire.read();
    synccount=0;
    if (mymonthDay==1 && myMonth==1 && myYear==0){  //battery has failed
      errorflag=true; 
    }
    setTime(myHour,myMin,mySec,mymonthDay,myMonth,1);
    digitalWrite(pinDisplay, HIGH);
  }else{
    synccount++;
    if (synccount>10){
      synccount=0;
      setTime(0,0,0,1,1,1);
      errorflag=true;
      digitalWrite(pinDisplay, HIGH);
    }else{
     t.after(100,syncTime);
    }
  }
  if (debug){
   Serial.print("bytesread=");Serial.print(bytesread);Serial.println();
   Serial.print("errorflag=");Serial.print(errorflag);Serial.println();
   }
}

void setDateTime(){
  byte myHour=hour();
  byte myMin=minute();
  byte mySec=second();
  
  byte weekDay =   weekday(); //1-7
  byte monthDay =  day(); //1-31
  byte monthX =     month(); //1-12
  byte yearX  =     year()-2000; //0-99

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);           // address of first register
  Wire.write(decToBcd(mySec));
  Wire.write(decToBcd(myMin));
  Wire.write(decToBcd(myHour));
  Wire.write(decToBcd(weekDay));
  Wire.write(decToBcd(monthDay));
  Wire.write(decToBcd(monthX));
  Wire.write(decToBcd(yearX));
  Wire.write(0);             //control register 
  Wire.write(hr24);          //hr24
  Wire.endTransmission();
}

byte decToBcd(byte val){
// Convert normal decimal numbers to binary coded decimal
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)  {
// Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}

int writeDig(byte bcd) {
  for (int i=0;i<4;i++){
    digitalWrite(abcd[i],bitRead(bcd,i));    
  }
}

byte flashit(){
  if (errorflag || (modeflag==1 && (onz==4 || onz==5)) || (modeflag==2 && (onz==2 || onz==3)) ){
  unsigned long currentMillis = millis();       
    if(currentMillis - flashMillis > flashinterval){
      flashMillis = currentMillis;   // save the last time 
      //code goes here
      flashon=!flashon;
      //code goes here
    } //end if    
    return flashon;
  }else{
    return HIGH;
  }
} //end flashit

byte chgmodeflag(){
  modeflag++;
  if (errorflag){errorflag=false;}
if (modeflag>3){ 
  modeflag=0;
  setDateTime();
  }
}
void debugClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" "); 
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
  Serial.println();
  
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
//****************end program***********************************
