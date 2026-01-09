#define PWM_MAX_DUTY      255
#define PWM_MIN_DUTY      0
#define PWM_START_DUTY    0
 
byte bldc_step = 0, motor_speed, pin_state;
 
void setup()
{
  DDRD  |= 0xE0;  // configure pins 5, 6 and 7 as outputs
  PORTD  = 0x00;
  DDRB  |= 0x0E;  // configure pins 9, 10 and 11 as outputs
  PORTB  = 0x31;
  // Timer1 module setting: set clock source to clkI/O / 1 (no prescaling)
  TCCR1A = 0;
  TCCR1B = 0x01;
  // Timer2 module setting: set clock source to clkI/O / 1 (no prescaling)
  TCCR2A = 0;
  TCCR2B = 0x01;
  // ADC module configuration
  ADMUX  = 0x60;   // configure ADC module and select channel 0
  ADCSRA = 0x84;   // enable ADC module with 16 division factor (ADC clock = 1MHz)
 
  PCICR  = EIMSK = 0;  // disable all external interrupts
 
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
}
 
// pin change interrupt 2 (PCINT2) ISR
ISR (PCINT2_vect)
{
  if( (PIND & PCMSK2) != pin_state )
    return;
  // BEMF debounce
  for(byte i = 0; i < 20; i++)
  {
    if(bldc_step & 1){
      if(PIND & PCMSK2)     i -= 1;
    }
    else {
      if(!(PIND & PCMSK2))  i -= 1;
    }
  }
 
  bldc_move();
  bldc_step++;
  bldc_step %= 6;
}
 
// BLDC motor commutation function
void bldc_move()
{
  switch(bldc_step)
  {
    case 0:
      AH_BL();
      BEMF_C_FALLING();
      break;
    case 1:
      AH_CL();
      BEMF_B_RISING();
      break;
    case 2:
      BH_CL();
      BEMF_A_FALLING();
      break;
    case 3:
      BH_AL();
      BEMF_C_RISING();
      break;
    case 4:
      CH_AL();
      BEMF_B_FALLING();
      break;
    case 5:
      CH_BL();
      BEMF_A_RISING();
  }
}
 
void loop()
{
  SET_PWM_DUTY(PWM_START_DUTY);  // setup starting PWM with duty cycle = PWM_START_DUTY
  int i = 5000;
 
  // motor start
  while(i > 100)
  {
    delayMicroseconds(i);
    bldc_move();
    bldc_step++;
    bldc_step %= 6;
    i = i - 20;
  }
 
  motor_speed = PWM_START_DUTY;
  
  PCICR  = 4;  // enable pin change interrupt for pins PCINT23..16 (Arduino 0 to 7)
  
  while(1)
  {
    ADCSRA |= 1 << ADSC;    // start conversion
    while(ADCSRA & 0x40);   // wait for conversion complete
    motor_speed = ADCH;     // read ADC data (8 bits only)
    if(motor_speed < PWM_MIN_DUTY)
      motor_speed = PWM_MIN_DUTY;
    SET_PWM_DUTY(motor_speed);
  }
}
 
void BEMF_A_RISING()
{
  PCMSK2 = 0x04;    // enable Arduino pin 2 (PCINT18) interrupt, others are disabled
  pin_state = 0x04;
}
void BEMF_A_FALLING()
{
  PCMSK2 = 0x04;    // enable Arduino pin 2 (PCINT18) interrupt, others are disabled
  pin_state = 0;
}
void BEMF_B_RISING()
{
  PCMSK2 = 0x08;    // enable Arduino pin 3 (PCINT19) interrupt, others are disabled
  pin_state = 0x08;
}
void BEMF_B_FALLING()
{
  PCMSK2 = 0x08;    // enable Arduino pin 3 (PCINT19) interrupt, others are disabled
  pin_state = 0;
}
void BEMF_C_RISING()
{
  PCMSK2 = 0x10;    // enable Arduino pin 4 (PCINT20) interrupt, others are disabled
  pin_state = 0x10;
}
void BEMF_C_FALLING()
{
  PCMSK2 = 0x10;    // enable Arduino pin 4 (PCINT20) interrupt, others are disabled
  pin_state = 0;
}
 
void AH_BL()
{
  PORTD &= ~0xA0;
  PORTD |=  0x40;
  TCCR1A =  0;      // turn pin 11 (OC2A) PWM ON (pin 9 & pin 10 OFF)
  TCCR2A =  0x81;   //
}
void AH_CL()
{
  PORTD &= ~0xC0;
  PORTD |=  0x20;
  TCCR1A =  0;      // turn pin 11 (OC2A) PWM ON (pin 9 & pin 10 OFF)
  TCCR2A =  0x81;   //
}
void BH_CL()
{
  PORTD &= ~0xC0;
  PORTD |=  0x20;
  TCCR2A =  0;       // turn pin 10 (OC1B) PWM ON (pin 9 & pin 11 OFF)
  TCCR1A =  0x21;    //
}
void BH_AL()
{
  PORTD &= ~0x60;
  PORTD |=  0x80;
  TCCR2A =  0;      // turn pin 10 (OC1B) PWM ON (pin 9 & pin 11 OFF)
  TCCR1A =  0x21;   //
}
void CH_AL()
{
  PORTD &= ~0x60;
  PORTD |=  0x80;
  TCCR2A =  0;       // turn pin 9 (OC1A) PWM ON (pin 10 & pin 11 OFF)
  TCCR1A =  0x81;    //
}
void CH_BL()
{
  PORTD &= ~0xA0;
  PORTD |=  0x40;
  TCCR2A =  0;       // turn pin 9 (OC1A) PWM ON (pin 10 & pin 11 OFF)
  TCCR1A =  0x81;    //
}
 
void SET_PWM_DUTY(byte duty)
{
  OCR1A  = duty;  // set pin 9  PWM duty cycle
  OCR1B  = duty;  // set pin 10 PWM duty cycle
  OCR2A  = duty;  // set pin 11 PWM duty cycle
}
