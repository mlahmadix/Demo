#ifndef __NMEAPARSER_H__
#define __NMEAPARSER_H__

/*
 * NMEA0183 GPS Setences Data parser
 * GGA - 3D position and accuracy data
 * GLL - Geographic latitude and longitude
 * RMC - NMEA's own "Recommended Minimum" GPS data
 * GSV - GPS satellites in view
 * GSA - GPS dilution of precision and active satellites
*/

/*
 * NMEA0183 GPS Frames Examples
 * $GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76
 * $GPGSA,A,3,10,07,05,02,29,04,08,13,,,,,1.72,1.03,1.38*0A
 * $GPGSV,3,1,11,10,63,137,17,07,61,098,15,05,59,290,20,08,54,157,30*70
 * $GPRMC,092750.000,A,5321.6802,N,00630.3372,W,0.02,31.66,280511,,,A*43
 */
 
/*
 * Protocol Principle:
 * Each NMEA new received line is called Sentence
 * All Data is ASCII encode based
 * "$" expressed beginning of new sentence
 * "CR LF" expressed end of sentence
 * All characters between "$" and "*" represents the NMEA Sentence Data
 * Two-caracters after "*" is called the checksum which represents results of "All Data XOR calculation".
 * All Sentence fields are separated by comma
 */ 
 
 #include <fstream>
 #include "nmeaparser/NmeaParserDefs.h"
 #include "IOUtils/UartUtils.h"
 
 class NmeaParser: public SerialPort
 {
	 public:
	 
		NmeaParser(const char * ucUartPortPath, unsigned long usBaudrate);
		#ifdef NMEAP_DEBUG
		NmeaParser(const char * EmulatedPath);
		#endif
		~NmeaParser();
		void NmeapPortSetBaudrate(unsigned long uwNewBaudrate);
	private:
		#ifdef NMEAP_DEBUG
		enum NmeaParserFileInit {
			CeNmea_FileNoInit,
			CeNmea_FileInitOK,
			CeNmea_FileInitKO,
		};
		NmeaParserFileInit NmeaFileStatus;
		std::ifstream NmeapFileHandle; 
		#endif
		int NmeapUartHandle;
		unsigned long musBaudrate;
	 
 };

#endif
