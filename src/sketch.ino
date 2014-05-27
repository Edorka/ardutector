/*
 * Sketch for testing sleep mode with wake up on WDT.
 * Donal Morrissey - 2011.
 *
 */
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <Wire.h>
#define LED_PIN (13)
#define HALL_SENSOR_PIN (2)
#define BT_POWER_PIN (3)
#define SLEEP_TIMEOUT (30)


volatile int f_wdt=1;
int doorState = 0;
int timeSleeping = 0;
void pinInterrupt(void)
{
  sleep_disable(); 
  Serial.print("waked up on pin.");  //getMovement();   
  doorState = digitalRead(HALL_SENSOR_PIN);
  if (doorState == HIGH){
	
     Serial.print("closed"); 

     digitalWrite(BT_POWER_PIN, LOW);
  } else {
     Serial.print("open"); 
     digitalWrite(BT_POWER_PIN, HIGH);
  }

  detachInterrupt(0);
  f_wdt = 1;
}

/***************************************************
 *  Name:        ISR(WDT_vect)
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Watchdog Interrupt Service. This
 *               is executed when watchdog timed out.
 *
 ***************************************************/
ISR(WDT_vect)
{
  timeSleeping = timeSleeping + 8;
  if(f_wdt == 0)
  {
    f_wdt=1;
  }
  else
  {
    Serial.println("WDT Overrun!!!");
  }
}


/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Enters the arduino into sleep mode.
 *
 ***************************************************/
void enterSleep(void)
{

     digitalWrite(BT_POWER_PIN, LOW);
  Serial.println("sleep mode by WDT");
  attachInterrupt(0, pinInterrupt, LOW);
  delay(100); //Allow for serial print to complete.
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   
  /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  sleep_enable();
  
  /* Now enter sleep mode. */
  sleep_mode();
  
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
}



void sendKeepAlive(){
	digitalWrite(BT_POWER_PIN, HIGH);
	Serial.print("KA"); delay(1000);
}

/***************************************************
 *  Name:        setup
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Setup for the serial comms and the
 *                Watch dog timeout. 
 *
 ***************************************************/
void setup()
{
  Serial.begin(9600);
  Serial.println("Initialising...");
  delay(100); //Allow for serial print to complete.
  pinMode(LED_PIN,OUTPUT);
  pinMode(HALL_SENSOR_PIN,INPUT);
  pinMode(BT_POWER_PIN,OUTPUT);


  /** Setup the bluetooth module **/

digitalWrite(BT_POWER_PIN, HIGH);
// Ahora se procede a la configuración del modulo:

// Se inicia la configuración:
Serial.print("AT"); delay(1000);

// Se ajusta el nombre del Bluetooth:
Serial.print("AT+NAME"); Serial.print("TECTO"); delay(1000);

// Se ajustan los baudios:
Serial.print("AT+BAUD"); Serial.print("9600"); delay(1000);

// Se ajusta la contraseña:
Serial.print("AT+PIN"); Serial.print("1234"); delay(1000); 

  /*** Setup the WDT ***/
  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
  
  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
  
  Serial.println("Initialisation complete.");
  delay(100); //Allow for serial print to complete.
}
 
/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Main application loop.
 *
 ***************************************************/
void loop()
{
  if(f_wdt == 1)
  {
    /* Toggle the LED */
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    
    /* Don't forget to clear the flag. */
    f_wdt = 0;
    
    /* Re-enter sleep mode. */
    enterSleep();
  }
  else
  {
    /* Do nothing. */
  }
  Serial.print(timeSleeping);
  Serial.println("seconds sleeping");
  if ( timeSleeping > SLEEP_TIMEOUT ){	
     sendKeepAlive();
  }
}
