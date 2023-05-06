#include <wiringPi.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 

//The WiringPi pin numbers 
#define bcdD    5  //# binary coded decimal (BCD) least significant bit (LSB) for 74LS47 A input pin
#define bcdA   19
#define bcdB   13
#define bcdC   6  //#binary coded decimal (BCD) most significant bit (MSB) for 74LS47 D input pin
#define blank  4
#define le     26

int d[10];
char count[10];
//d     = [0,27,22,17,10,9,11]
int dig=0;
//t=time()
//tb=time()
int interval=3000;
int second_interval=1000;
long previousMillis = 0;
long previousMicros = 0;

int main()
{
int i;

if (wiringPiSetupGpio () == -1)
 {           exit (1) ;}
d[6]=27;d[5]=22;d[4]=17;d[3]=10;d[2]=9;d[1]=11;
//count[1]=1;count[2]=2;count[3]=3;count[4]=4;count[5]=5;count[6]=6;
strcpy(count,"0123456");
pinMode(bcdA, OUTPUT);pinMode(bcdB, OUTPUT);pinMode(bcdC, OUTPUT);pinMode(bcdD,OUTPUT);
pinMode(blank,OUTPUT);pinMode(le,OUTPUT);
  for (i=0;i<7;i++){ 
  pinMode(d[i],OUTPUT);
  }
digitalWrite(le, 1);  
digitalWrite(blank, 0);  

  while (1){
  unsigned long currentMicros = micros();     //interrupt routine to sync clock from RTC   
  if(currentMicros - previousMicros > interval)   //interval is normally 5 hours
  {previousMicros = currentMicros;   // save the last time 
    //code goes here
    displayWrite();
    //code goes here
  } //end if
  unsigned long currentMillis = millis();     //interrupt routine to sync clock from RTC   
  if(currentMillis - previousMillis > second_interval)   //interval is normally 5 hours
  {previousMillis = currentMillis;   // save the last time 
    //code goes here
    udatetime();
    //code goes here
  } //end if

  }//while       
}

// Functions
/*
void dcall(){
  displayWrite(count[dig])
}
*/
void udatetime(){
  //global count
  //mytime = datetime.now().strftime("%H%M%S")
  //count = [0,int(mytime[5]),int(mytime[4]),int(mytime[3]),int(mytime[2]),int(mytime[1]),int(mytime[0])]
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  int hr = timeinfo->tm_hour;
  int mn = timeinfo->tm_min;
  int sc = timeinfo->tm_sec;
  char shr[2];
  char smn[2];
  char ssc[2];
 sprintf (shr,"%02d",hr);sprintf (smn,"%02d",mn);sprintf (ssc,"%02d",sc);
//printf("%s\n", shr);
//printf("%s\n", smn);
//printf("%s\n", ssc);

  strcpy(count,"-");
  strcat(count,shr);
  strcat(count,smn);
  strcat(count,ssc);  

  // print our string
//printf("%s\n", count);
}


//# Reads a bit of a number
int bitRead(value, bit){
   return value & (1 << bit);  //# shift mask to bit position and AND to value
}
//# Writes value to display using binary coded decimal
void displayWrite(){
   digitalWrite(d[dig], 0); //turn off old digit
   dig=dig+1;
   if (dig > 6){
      dig = 1;
   }  
   digitalWrite(bcdA, bitRead(count[dig], 0));  //write new digit value
   digitalWrite(bcdB, bitRead(count[dig], 1));
   digitalWrite(bcdC, bitRead(count[dig], 2));
   digitalWrite(bcdD, bitRead(count[dig], 3));
   digitalWrite(d[dig], 1);       //turn on new digit
   //digitalWrite(le, 1); 
   //digitalWrite(blank, 0);
    
}

