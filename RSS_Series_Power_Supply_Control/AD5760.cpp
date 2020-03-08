// Driver written by Gokul
// With original Arduino program written by Zhengmin

#include "AD5760.h"
#include "Arduino.h"

AD5760::AD5760() {
	// empty constructor
}

AD5760::AD5760(int param_MISO, int param_MOSI, int param_SCK, int param_LDAC, int param_SS) {
	_MISO = param_MISO;
	_MOSI = param_MOSI;
	_SCK = param_SCK;
	_SS = param_SS;
	_LDAC = param_LDAC;

	pinMode(_SS, OUTPUT);
	pinMode(_LDAC, OUTPUT);

	digitalWrite(_SS, HIGH);
	digitalWrite(_MOSI, LOW);
	digitalWrite(_SCK, LOW);
	initialize();
}

void AD5760::getValues() {
	Serial.println(_MISO);
	Serial.println(_MOSI);
	Serial.println(_SCK);
	Serial.println(_SS);
	Serial.println(_LDAC);
}

void AD5760::sendByte(uint8_t value) {
	int i = 0;
	for (i; i < 8; i++) {
		digitalWrite(_SCK, HIGH);
		if ((value & 0x80) == 0x80) {
			digitalWrite(_MOSI, HIGH);
		}
		else {
			digitalWrite(_MOSI, LOW);
		}
		value = value << 1;
		digitalWrite(_SCK, LOW);
	}
}

void AD5760::sendSequence(uint8_t first, uint8_t second, uint8_t third) {
	digitalWrite(_SS, LOW);
	sendByte(first);
	sendByte(second);
	sendByte(third);
	digitalWrite(_SS, HIGH);
}

void AD5760::setVoltage(float voltage) {
  uint16_t data = (uint16_t) (voltage * 65535 / 10);
  data = data & 0xffff;
  uint8_t first = 0b00010000 | (data >> 12);
  uint16_t second = (data << 4) & (0xffff);

  digitalWrite(SS, LOW);
  sendByte(first);
  sendByte((uint8_t)((second >> 8) & 0xff));
  sendByte((uint8_t)((second) & 0xf0));
  digitalWrite(SS, HIGH);
}

void AD5760::initialize() {
	uint8_t software_high = 0b01000000;
	uint8_t software_mid = 0b00000000;
	uint8_t software_low = 0b00000000;
	sendSequence(software_high, software_mid, software_low);

	uint8_t control_high = 0b00100000;
	uint8_t control_mid = 0b00000000;
	uint8_t control_low = 0b00010010;
	sendSequence(control_high, control_mid, control_low);
}