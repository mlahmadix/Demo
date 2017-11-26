#ifndef __UART_UTILS_H
#define __UART_UTILS_H

#include <boost/asio/serial_port.hpp> 

class SerialPort
{
	protected:
		enum SerialPortStatus {
			CeStatus_UartIdle,
			CeStatus_UartInitOK,
			CeStatus_UartInitKO,
			CeStatus_UartSettingOK,
			CeStatus_UartSettingKO
		};
		SerialPort(const char * ucUartPortPath, unsigned long usBaudrate, bool Parity, int CharSize,
				   int StopBit, bool FlowControl);
		~SerialPort();
		SerialPortStatus SetSerialPortBaudrate(unsigned long usBaudrate);
		inline  SerialPortStatus getSerialPortStatus() { return mSerialPortStatus; }
		unsigned long SerialPortReadResp(char * pucReadBuffer, unsigned long ulReadLen);
		unsigned long SerialPortWrite(const char * pucWriteBuffer);

	private:
		boost::asio::io_service SerialPortIO;
		boost::asio::serial_port * SerialportHnd;
		SerialPortStatus mSerialPortStatus;
		unsigned long mSerialPortBaudrate;
		const char * mComPort;
		inline unsigned long getSerialPortBaudrate() { return mSerialPortBaudrate; }
		void SerialPortFlushData();
		char SerialPortReadChar();
		
};

#endif
