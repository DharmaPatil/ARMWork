/*
 Copyright (c) 2010 by arms22 (arms22 at gmail.com)
 Microchip 23x256 SPI SRAM library for Arduino

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#if defined (ARDUINO)
#include <SPI.h>
#elif defined (ARMCMX)
#include "SPI.h"
#endif

#include "SPISRAM.h"

SPISRAM::SPISRAM(SPIBus & spi, const byte csPin, const byte addr_width) :
		SPIx(spi), _csPin(csPin), _addrbus(addr_width) {
	//addr = 0;
}

void SPISRAM::init() {
	pinMode(_csPin, OUTPUT);
	csHigh();
	//addr = 0;
	select();
	writeStatus(SEQ_MODE);
	deselect();
}

byte SPISRAM::read(const long & address) {
	byte data;
	//addr = ;
	select();
	set_access(READ, address);
	data = SPIx.transfer(0);
	deselect();
	return data;
}

void SPISRAM::read(const long & address, byte *buffer, const long & size) {
	select();
	writeStatus(SEQ_MODE);
  csHigh();
  csLow();
	//
	byte * p = buffer;
	set_access(READ, address);
	for (unsigned int i = 0; i < size; i++)
		*p++ = SPIx.transfer(0);
	deselect();
}

void SPISRAM::write(const long & address, byte data) {
	select();
	set_access(WRITE, address);
	SPIx.transfer(data);
	deselect();
}

void SPISRAM::write(const long & address, byte *buffer, const long & size) {
	select();
	writeStatus(SEQ_MODE);
  csHigh();
	//
  csLow();
	set_access(WRITE, address);
	for (unsigned int i = 0; i < size; i++)
		SPIx.transfer(*buffer++);
	deselect();
}

void SPISRAM::setSPIMode(void) {
//	SPIx.setBitOrder(MSBFIRST);
	SPIx.setDataMode(SPI_MODE0);
  SPIx.setClockDivider(SPI_CLOCK_DIV4);
}

inline void SPISRAM::csLow() {
	digitalWrite(_csPin, LOW);
}

inline void SPISRAM::csHigh() {
	digitalWrite(_csPin, HIGH);
}

inline void SPISRAM::select(void) {
	setSPIMode();
	csLow();
}

inline void SPISRAM::deselect(void) {
	csHigh();
}

uint8 SPISRAM::started(void) {
  uint8 res;
  select();
  res = readStatus();
  deselect();
  return (res & 0xc0) == 0x40;
}
