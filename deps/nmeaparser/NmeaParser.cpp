#include <iostream>
#include "nmeaparser/NmeaParser.h"
#include "logger/lib_logger.h"

#define TAG "NmeaParser"

using namespace std;

NmeaParser::NmeaParser(const char * ucUartPortPath, unsigned long usBaudrate):
SerialPort(ucUartPortPath, usBaudrate, true, 8,
				   1, false)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	if(getSerialPortStatus() == CeStatus_UartInitOK) {
		ALOGD(TAG, __FUNCTION__, "NMEA Serial Port Opened Successfully");
	} else {
		ALOGD(TAG, __FUNCTION__, "Fail to open NMEA Serial Port");
	}
}

#ifdef NMEAP_DEBUG
NmeaParser::NmeaParser(const char * EmulatedPath):
SerialPort(NULL, 9600, true, 8, 1, false),
NmeaFileStatus(CeNmea_FileNoInit),
NmeapFileHandle(NULL)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	if(EmulatedPath != NULL) {
		NmeapFileHandle.open(EmulatedPath, ios::in | ios::app);
		if(NmeapFileHandle.is_open()) {
			NmeaFileStatus = CeNmea_FileInitOK;
		}
		else {
			NmeaFileStatus = CeNmea_FileInitKO;
		}
	}
	
}
#endif

NmeaParser::~NmeaParser()
{
	ALOGD(TAG, __FUNCTION__, "DTOR");
#ifdef NMEAP_DEBUG
	if(NmeapFileHandle.is_open()) {
		NmeapFileHandle.close();
	}
#endif
}

void NmeaParser::NmeapPortSetBaudrate(unsigned long uwNewBaudrate)
{
	ALOGD(TAG, __FUNCTION__, "New Baudrate =%d", uwNewBaudrate);
	if(SetSerialPortBaudrate(uwNewBaudrate)) {
		ALOGE(TAG, __FUNCTION__, "Unable to set new baudrate");
	}
}


