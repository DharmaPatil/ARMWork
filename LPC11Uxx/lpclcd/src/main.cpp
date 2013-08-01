/*
 * LPCLDC ported to Keil uV
 *
 */

#include <stdio.h>
//#include <stdlib.h>
#include <string.h>

#include "LPC11Uxx.h"
#include "type.h"

#include "armcmx.h"

#include "systick.h"

#include "USARTSerial.h"
#include "I2CBus.h"
#include "ST7032i.h"

#ifdef _LPCXPRESSO_CRP_
#include <cr_section_macros.h>
#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;
#endif


/*******************************************************************************
**   Main Function  main()
*******************************************************************************/

// Strawberry Linux original lpclcd port maps
#define LPCLCDBKLT PIO1_3
//#define CAPPUCINOBKLT PIO0_3
#define LCDRST  PIO1_25
#define USERLED PIO1_6
#define USERBTN PIO0_1
#define RXD2    PIO0_18
#define TXD2    PIO0_19

#define CAPPUCINO_LED_SDBUSY   PIO1_19

#define SYSTICK_DELAY		(SystemCoreClock/100)

ST7032i i2clcd(Wire, LPCLCDBKLT, LCDRST);

int main (void) {
	long sw;
  char str[32];
  int i;
  
  SystemCoreClockUpdate();

  SystemInit();
  GPIOInit();
  start_delay();
  
  sw = SYSTICK_DELAY;
  // systick initialize
  SysTick_Config(SYSTICK_DELAY);
  // Clear SysTick Counter 
  SysTick->VAL = 0;
  // Enable the SysTick Counter 
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
  
  delay(200);
  // I2C LCD Backlight controll pin
  pinMode(LPCLCDBKLT, OUTPUT);
  digitalWrite(LPCLCDBKLT, LOW);
  
  pinMode(USERBTN, INPUT);
  
  Wire.begin();
  if ( Wire.status == FALSE ) 
  	while ( 1 );				/* Fatal error */

  // I2C液晶を初期化します
  if ( ! i2clcd.begin() ) 
    while (1); 

  pinMode(USERLED, OUTPUT);
  digitalWrite(USERLED, HIGH);
    
  i2clcd.print("I was an lpclcd.");
  i2clcd.setCursor(0, 1);	// move to 2nd line
  i2clcd.print("Hi, everybody!");
  for(i = 0; i < 3; i++) {
    delay(500);
    i2clcd.noDisplay();
    delay(500);
    i2clcd.display();
  }
  delay(500);
  i2clcd.clear();
  
  sw = millis();
  
  while (1){    /* Loop forever */
    
    if ( millis() != sw ) {
      sw = millis();

      i2clcd.setCursor(0, 1);
      sprintf(str, " %06d", TimeTick);
      i2clcd.print(str);
    }
  }
  
}

/******************************************************************************
**                            End Of File
******************************************************************************/
