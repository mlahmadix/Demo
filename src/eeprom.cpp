#include "main_app/eeprom.h"
#include "logger/lib_logger.h"

#define TAG "Eeprom"

using namespace std;

eeprom::eeprom(const char * cstEepromPath, const unsigned int cstEepromSize):
FileIo(cstEepromPath, CeFile_RdWr, CeFile_BIN, true, cstEepromSize),
mEepromSize(cstEepromSize),
mEeprom_CSum(0)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	if(FileIo_GetStatus() == CeFile_Opened) {
		ALOGD(TAG, __FUNCTION__, "E2P device opened successfully");
	}else {
		ALOGE(TAG, __FUNCTION__, "Fail to open E2P device");
	}
}

eeprom::~eeprom()
{
	ALOGD(TAG, __FUNCTION__, "DTOR");
}

int eeprom::eeprom_read(char * ucReadBuffer, unsigned int E2poffset, int ReadLen)
{
	ALOGD(TAG, __FUNCTION__, "Reading ...");
	return FileIo_ReadStr(ucReadBuffer, E2poffset, ReadLen);
}

int eeprom::eeprom_write(char * ucWriteBuffer, unsigned int E2poffset, int WriteLen)
{
	ALOGD(TAG, __FUNCTION__, "Writing ...");
	int Status = FileIo_WriteStr(ucWriteBuffer, E2poffset, WriteLen);
	//Next Step is To Calculate and Update CS
	if(Status == 0) {
		int CS = FileIo_CalculateCS(2, mEepromSize); //two first bytes reserved for E2prom CS
		ALOGD(TAG, __FUNCTION__, "Calculated CS = %08X", CS);
		Status = FileIo_WriteStr((char*)&CS, 0, 2);
	} else {
		ALOGE(TAG, __FUNCTION__, "Fail to Write To E2P device");
	}
	return Status;
}

int eeprom::eeprom_CloneToFile(string EepromCloneFile)
{
	return FileIo_Mirroring(EepromCloneFile);
}

