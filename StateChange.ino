#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>

int count = 0;  
                //
void setup(void)
{
    DDRD &= B00000011;       // set Arduino pins 2 to 7 as inputs, leaves 0 & 1 (RX & TX) as is
    DDRB = B00000000;        // set pins 8 to 13 as inputs
    //PRR = PRR | 0b00100000;  // disable timers
    //PORTD |= B11111100;      // enable pullups on pins 2 to 7
    //PORTB |= B11111111;      // enable pullups on pins 8 to 13
    pinMode(13,OUTPUT);      // set pin 13 as an output so we can use LED to monitor
    digitalWrite(13,HIGH);   // turn pin 13 LED on
}
            //
void loop(void)
{
   digitalWrite(13,LOW); 
      delay(100);     // this delay is needed, the sleep 
      digitalWrite(13,HIGH); 
    // Stay awake for 1 second, then sleep.
    // LED turns off when sleeping, then back on upon wake.
    Serial.print("Awake for ");
  Serial.print(count);
  Serial.println("sec");
  count++;
  delay(1000);  
  
  if (count >= 3) {
      Serial.println("Timer: Entering Sleep mode");
      digitalWrite(13,LOW); 
      delay(100);     // this delay is needed, the sleep 
      digitalWrite(13,HIGH); 
                      //function will provoke a Serial error otherwise!!
      count = 0;
      sleepNow();     // sleep function called here
  } else {

  }
}
                //
void sleepNow(void)
{
  

    // Set pin 2 as interrupt and attach handler:
    sleep_enable();
    attachInterrupt(0, pinInterrupt, CHANGE);
    
    //
    // Choose our preferred sleep mode:
    set_sleep_mode(SLEEP_MODE_IDLE);
    //
    // Set sleep enable (SE) bit:
    sleep_enable();
    power_timer0_disable();
    power_timer1_disable();
    power_timer2_disable();
    
    digitalWrite(13,LOW);   // turn LED off to indicate sleep  
    //sleep_bod_disable();
    // Put the device to sleep:
    sleep_mode(); 
    // Upon waking up, sketch continues from this point.
    sleep_disable();
    digitalWrite(13,HIGH);   // turn LED on to indicate awake
}
                //
void pinInterrupt(void)
{
  detachInterrupt(0);
   power_all_enable();
  //pin2_interrupt_flag = 1;
}
