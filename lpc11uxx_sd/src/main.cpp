/*
===============================================================================
 Name        : main.c
 Author      : 
 Version     :
 Copyright   : Copyright (C) 
 Description : main definition
===============================================================================
*/

#include "LPC11Uxx.h"
#include "type.h"

#include "integer.h"
#include "ff.h"

#include "armcmx.h"
#include "USARTSerial.h"
#include "I2Cbus.h"
#include "ST7032i.h"
#include "RTC.h"
#include "spi.h"
#include "SPIBus.h"
#include "SPISRAM.h"
#include "SDFatFs.h"

#include "PWM0Tone.h"

#include "cappuccino.h"

#ifdef _LPCXPRESSO_CRP_
#include <cr_section_macros.h>
#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;
#endif

SPIBus SPI0(&SPI0Def, PIO1_29, PIO0_8, PIO0_9, PIO0_2); // sck, miso, mosi, cs
SPIBus SPI1(&SPI1Def, PIO1_20, PIO1_21, PIO1_22, PIO1_23); // sck, miso, mosi, cs

SDFatFs sd(SPI0, PIO0_2);
SDFatFile file(sd);
void sd_test(void);

ST7032i lcd(Wire, LED_LCDBKLT);
RTC rtc(RTC::ST_M41T62);
//SPIBus SPI1(&SPI1Def, PIO1_20, PIO1_21, PIO1_22, PIO1_23); // sck, miso, mosi, cs
//SPISRAM sram(SPI1, PIO1_23, SPISRAM::BUS_MBITS);


int main(void) {
	int i;

	SystemInit();
	GPIOInit();
  start_delay();

	// GPIO, delay, PWM0Tone test.
	pinMode(LED_SDBUSY, OUTPUT);
  for (i = 0; i < 3; i++) {
    digitalWrite(LED_SDBUSY, HIGH);
    delay(100);
    digitalWrite(LED_SDBUSY, LOW);
    delay(100);
  }
  digitalWrite(LED_SDBUSY, HIGH);
  PWM0_tone(PIO1_13, 1320, 100);
	PWM0_tone(PIO1_13, 1540, 100);
  delay(500);
  digitalWrite(LED_SDBUSY, LOW);
  //
  
	USART_init(&usart, RXD_A, TXD_A);
//  USART_init(&usart, RXD_C, TXD_C);
	USART_begin(&usart, 115200);
  USART_puts(&usart, "\nHello, nice to see you!\n");

  Wire.begin();
  lcd.begin();
  lcd.backlightHigh();
  lcd.print("Let's start LCD!");

  rtc.begin();
  rtc.update();
  lcd.setCursor(0,1);
  lcd.print(rtc.time, HEX);
  lcd.print(" ");
  lcd.print(rtc.cal, HEX);
  Serial.print("Current time printed on LCD: ");
  Serial.println(rtc.time, HEX);
//	 下記は不要な部分はコメントアウトしてお試しください。

//  SPI1.begin();
//  sram.begin();
 /*
  * SDカードのデモ（エンドレス）
  */
  Serial.print("result of get_fattime: ");
  Serial.println(get_fattime(), HEX);
  
  //SPI_init(&SPI0Def, PIO1_29, PIO0_8, PIO0_9, SSP_CS0);
  SPI0.begin();
  sd.begin();
  if ( digitalRead(SW_SDDETECT) == HIGH ) {
    Serial.println("SD slot is empty.");
  } else {
    Serial.println("Card is in SD slot.");
  }
	sd_test();
/*
 * i2C液晶のテスト（エンドレス）
 */
	  while (1){                                /* Loop forever */
		  digitalWrite(LED_SDBUSY, LOW);
		  delay(500);
		  digitalWrite(LED_SDBUSY, HIGH);
		  delay(500);

		  //i2clcd_backlight(cn &1);
		  //i2c_cmd(0x80+7);
	  }

//	return 0 ;
}


#define BCD8TODEC(n)  ( ((n)&>>4&0x0f)*10 + ((n)&0x0f) )

DWORD get_fattime(void) {
  return SDFatFs::fattime(rtc.cal, rtc.time);
}


//FATFS Fatfs;		/* File system object */
//FIL Fil;			/* File object */
static uint8_t buff[128];


/*
 * SDカードからMESSAGE.TXTのファイルを読み込んでI2C液晶に表示します。
 * (先頭32文字だけ)
 * その後、SD0001.TXTというファイルを作成して、LPCcappuccino!+CR+LFという文字を永遠に書き込みます
 */

/*
 * SDカードのサンプル
 */
void sd_test()
{
//	FRESULT rc;
  long swatch;
  
//	DIR dir;				/* Directory object */
//	FILINFO fno;			/* File information object */
	UINT br, i, bw ;

//	f_mount(0, &Fatfs);		/* Register volume work area (never fails) */
	/*
	 * SDカードのMESSAGE.TXTを開いてI2C液晶に表示します。英数カナのみ
	 * ２行分のみ
	 */
	//rc = f_open(&Fil, "MESSAGE.TXT", FA_READ);
	file.open("MESSAGE.TXT", SDFatFile::FILE_READ); 
  if ( !file.result() ) { //!rc){
    USART_puts(&usart, "\nType the file content:\n\n");
    for (;;) {
      /* Read a chunk of file */
      //if (rc || !f_gets((TCHAR*)buff, sizeof(buff), &Fil) ) break;			/* Error or end of file */
      if ( file.gets((TCHAR*) buff, sizeof(buff)) == NULL || file.result() )
        break;

      USART_puts(&usart, (char*)buff);
    }
    if ( file.result() ) {
      USART_puts(&usart, "\nFailed while reading.\n");
      return;
    }
    //rc = f_close(&Fil);
    file.close();
    /*
     *	ファイル書き込みテスト
     *	SD0001.TXTファイルを作成し、Strawberry Linuxの文字を永遠に書き込む
     */

//    rc = f_open(&Fil, "SD0001.TXT", FA_WRITE | FA_CREATE_ALWAYS);
    file.open("SD0001.TXT", SDFatFile::FILE_WRITE);
    if ( file.result() ) { //rc) {
      USART_puts(&usart, "\nCouldn't open SD0001.TXT.\n");
      return;
    }

    swatch = millis();
    // 無限ループでこの関数からは抜けない
    while(1){
      i = sprintf((char*)buff, "%08u ", millis());
      //f_write(&Fil, buff, i, &bw);
      file.write(buff, i);
      //rc = f_write(&Fil, "Strawberry Linux\r\n", 18, &bw);
      file.write((uint8_t *)"Strawberry Linux\r\n", 18);
      //if (rc) 
      if ( file.result() ) 
        break;
      // SDカードに書き出します。
     // f_sync(&Fil);
      file.flush();
      if ( swatch + 2000 < millis() ) 
        break;
    }
    //f_close(&Fil);
    file.close();
    USART_puts(&usart, "\nSD File IO test finished.\n");
  	return;

  }
  if ( file.result() == FR_NOT_READY ) //rc == FR_NOT_READY )
    USART_puts(&usart, "\nCouldn't open MESSAGE.TXT.\n");

}
