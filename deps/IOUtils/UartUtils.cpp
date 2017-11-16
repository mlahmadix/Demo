#include <boost/asio/serial_port.hpp> 
#include <boost/asio.hpp>
#include <string> 
#include "IOUtils/UartUtils.h"
#include "logger/lib_logger.h"

#define TAG "SerialPort"

using namespace std;
using namespace boost;

SerialPort::SerialPort(const char * ucUartPortPath, unsigned long usBaudrate, bool Parity, int CharSize,
				   int StopBit, bool FlowControl):
SerialportHnd(NULL),
mSerialPortStatus(CeStatus_UartIdle),
mSerialPortBaudrate(usBaudrate),
mComPort(string(ucUartPortPath))
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	if(ucUartPortPath != NULL) {
		SerialportHnd = new boost::asio::serial_port(SerialPortIO);
		SerialportHnd->open(mComPort.c_str());
		SerialportHnd->set_option(asio::serial_port_base::baud_rate(usBaudrate));
		SerialportHnd->set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::even));
		SerialportHnd->set_option(asio::serial_port_base::character_size(asio::serial_port_base::character_size(CharSize)));
		SerialportHnd->set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
		SerialportHnd->set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::none));
		if(SerialportHnd->is_open()) {
			mSerialPortStatus = CeStatus_UartInitOK;
		}
	}
}

SerialPort::~SerialPort()
{
	ALOGD(TAG, __FUNCTION__, "DTOR");
	if(getSerialPortStatus() == CeStatus_UartInitOK) {
		if(SerialportHnd->is_open()) {
			SerialportHnd->cancel();
			SerialPortFlushData();
			SerialportHnd->close();
			SerialPortIO.stop();
			SerialPortIO.reset();
			mSerialPortStatus = CeStatus_UartIdle;
		}
		delete SerialportHnd;
	}
}

SerialPort::SerialPortStatus SerialPort::SetSerialPortBaudrate(unsigned long usBaudrate)
{
	if(getSerialPortStatus() == CeStatus_UartInitOK) {
		if(usBaudrate == mSerialPortBaudrate) {
			ALOGD(TAG, __FUNCTION__, "Setting Same Baudrate");
			return CeStatus_UartSettingKO;
		} else {
			if(SerialportHnd->is_open()) {
				SerialPortFlushData();
				SerialportHnd->cancel();
				SerialportHnd->close();
				SerialportHnd->open(mComPort.c_str());
				SerialportHnd->set_option(asio::serial_port_base::baud_rate(usBaudrate));
				if(SerialportHnd->is_open()) {
					mSerialPortStatus = CeStatus_UartInitOK;
				} else {
					mSerialPortStatus = CeStatus_UartInitKO;
					return CeStatus_UartSettingKO;
				}
			}
		}
	} else {
		return CeStatus_UartSettingKO;
	}
	return CeStatus_UartSettingOK;
}

char SerialPort::SerialPortReadChar()
{
	char c;
 
	// Read 1 character into c, this will block
	// forever if no character arrives.
	SerialportHnd->read_some(asio::buffer(&c,1));
	return c;
}

unsigned long SerialPort::SerialPortReadResp(char * pucReadBuffer, unsigned long ulReadLen)
{
	unsigned long RdSize = 0;
	if ((getSerialPortStatus() == CeStatus_UartInitOK) && (pucReadBuffer != NULL)) {
		while(RdSize < ulReadLen) {
			pucReadBuffer[RdSize] = SerialPortReadChar();
			RdSize++;
		}
	}
	return RdSize;
	
}

unsigned long SerialPort::SerialPortWrite(const char * pucWriteBuffer)
{
	unsigned long WrSize = 0;
	if((getSerialPortStatus() == CeStatus_UartInitOK) && (pucWriteBuffer != NULL)) {
		SerialportHnd->write_some(asio::buffer(pucWriteBuffer,strlen(pucWriteBuffer)));
		WrSize = strlen(pucWriteBuffer);
	}
	return WrSize;
}

void SerialPort::SerialPortFlushData()
{
	if(getSerialPortStatus() == CeStatus_UartInitOK) {
		//Flushing Send/Receive Data on Serial Port
		::tcflush(SerialportHnd->lowest_layer().native_handle(), TCIOFLUSH);
	}
}
