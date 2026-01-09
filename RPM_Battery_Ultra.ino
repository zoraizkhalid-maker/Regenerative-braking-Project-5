#include <ACS712.h>
#include <LiquidCrystal.h>
//initialize the interface pins
LiquidCrystal lcd(3, 4, 5, 6, 7, 8);

//Icons
const static uint8_t Battery[8]  = {0x0E, 0x1B, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F};
const static uint8_t Panel[8]  = {0x1F, 0x15, 0x1F, 0x15, 0x1F, 0x15, 0x1F, 0x00};
const static uint8_t Pwm[8]  = {0x1D, 0x15, 0x15, 0x15, 0x15,0x15, 0x15, 0x17};
const static uint8_t Flash[8]  = {0x01, 0x02, 0x04, 0x1F, 0x1F, 0x02, 0x04, 0x08};

//Inputs
ACS712 sensor(ACS712_20A, A2);
//ACS712_05B for 20 Amp type
//ACS712_20A for 20 Amp type
//ACS712_30A for 30 Amp type

float amp1 = 0, old_amp=0;

#define voltagePin1 A0
#define voltagePin2 A1
float V1=0, V2=0;
float power=0, old_power=0;
int Voltage1, Voltage2, level=0;

#define relay 9

const int sensorPin = 2;
const int sensorInterrupt = 0;
volatile unsigned long lastPulseTime;
volatile unsigned long interval = 0;
//volatile int timeoutCounter;
int rpm;
int rpmlast=3000;
int Speed=0;

#define e_s1 10 //echo pin
#define t_s1 11 //Trigger pin
int cm=0, timer=0;

void setup(){
sensor.calibrate();

pinMode(voltagePin1,INPUT);
pinMode(voltagePin2,INPUT);
pinMode(relay, OUTPUT);
pinMode(e_s1,INPUT);  pinMode(t_s1,OUTPUT);

pinMode(sensorPin, INPUT);
attachInterrupt(0, &sensorIsr, FALLING);
lastPulseTime = 0;  

Serial.begin(9600);
  
lcd.begin(20, 4);
lcd.setCursor(0, 0);
lcd.print("     WELCOME");
lcd.setCursor(0,1); 
lcd.print("  Electric Vehicle  ");
lcd.setCursor(0,2); 
lcd.print("Power Train TestBed ");
lcd.setCursor(0,3); 
lcd.print("       (EVPTT)");

lcd.createChar(1, Battery);
lcd.createChar(2, Panel);
lcd.createChar(3, Pwm);
lcd.createChar(4, Flash);
delay(5000);
lcd.clear();
}

void loop(){
amp1 = sensor.getCurrentDC();

Voltage1 = analogRead(voltagePin1);
Voltage2 = analogRead(voltagePin2);

V1 = Voltage1/9.999;
power = V1*amp1;

if(rpm>=0){//Remove the error readings of minus values   
  rpm = constrain (rpm, 0, 9999); //Let's keep this RPM value under control, between 0 and 9999
if((micros() - lastPulseTime)<5e6){//If the engine is not running, print 0
  rpm = rpm;
 }else{rpm=0;}
}



Speed = map(rpm,0,4000,0,40);

level = map(Voltage1,320, 400,0,100);
if(level<0)level=0;
if(level>100)level=100;

cm = ultra_read(t_s1,e_s1);

lcd.setCursor(0, 0);    
lcd.write(1);          
lcd.print("  ");
lcd.print(level);
lcd.print("%   ");
lcd.print((Voltage1/100)%10);
lcd.print((Voltage1/10)%10);
lcd.print(".");
lcd.print(Voltage1%10);
lcd.print("v  ");

lcd.setCursor(1, 1);
lcd.write(2);      
lcd.print(" ");        
lcd.print((Voltage2/100)%10);
lcd.print((Voltage2/10)%10);
lcd.print(".");
lcd.print(Voltage2%10);
lcd.print("v ");

if(power>old_power){
lcd.setCursor(10, 1);    //Column 0 row 1
lcd.print(amp1,2);     //Solar current
lcd.print("A  ");      //Ampers
  
lcd.setCursor(0, 2);          
lcd.print("Power:  ");  
lcd.print(power,1);    
lcd.print("W  ");     
}

old_power=power;

lcd.setCursor(0, 3);          
lcd.print("   Speed:  ");  
lcd.print(Speed);    
lcd.print("Kmh  ");   
  
if(cm>50 && timer==0){digitalWrite(relay, 0); 
lcd.setCursor(10, 1);    //Column 0 row 1
lcd.print(amp1,2);     //Solar current
lcd.print("A  ");      //Ampers
  
lcd.setCursor(0, 2);          
lcd.print("Power:  ");  
lcd.print(power,1);    
lcd.print("W  ");  
}

if(cm<40){digitalWrite(relay, 1); timer=100;}
if(timer>0)timer=timer-1;

delay(1);    
}//End void loop

void sensorIsr() {
  unsigned long now = micros();
  interval = now - lastPulseTime;
  if (interval > 1000){
     rpm = 61000000UL/(interval * 8);
     lastPulseTime = now;
  }
}

//**********************ultra_read****************************
int ultra_read(int pin_t,int pin_e){
digitalWrite(pin_t,LOW);
delayMicroseconds(2);
digitalWrite(pin_t,HIGH);
delayMicroseconds(10);
long time = pulseIn (pin_e,HIGH);
int ultra_time =  time / 29 / 2; 
delay(10);
return ultra_time;
}
