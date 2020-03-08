// Driver written by Gokul
// With original Arduino program written by Zhengmin

#ifndef AD5760_h
#define AD5760_h
#include "Arduino.h"

class AD5760
{
	public:
    AD5760 ();
		AD5760 (int param_MISO, int param_MOSI, int param_SCK, int param_LDAC, int param_SS);
		void initialize();
		void sendSequence(uint8_t first, uint8_t second, uint8_t third);
		void getValues();
    void setVoltage(float voltage);
	private:
		int _MISO, _MOSI, _SCK, _LDAC, _SS;
		void sendByte(uint8_t value);
};

#endif