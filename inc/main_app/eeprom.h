#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "IOUtils/FileIo.h"

class eeprom: public FileIo
{
	public:
		eeprom(const char * cstEepromPath, const unsigned int cstEepromSize);
		~eeprom();
		int eeprom_read(char * ucReadBuffer, unsigned int E2poffset, int ReadLen);
		int eeprom_write(char * ucWriteBuffer, unsigned int E2poffset, int WriteLen);
		int eeprom_CloneToFile(std::string EepromCloneFile);
	private:
		const unsigned int mEepromSize;
		int mEeprom_CSum;
		
};

#endif
