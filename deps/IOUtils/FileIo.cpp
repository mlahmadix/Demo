#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "IOUtils/FileIo.h"
#include "logger/lib_logger.h"

using namespace std;

#define TAG "FileIO"

FileIo::FileIo(std::string FilePath, FileAccessType FileAccess, FileType binary, bool FixedFileSize, unsigned int FileDataSize):
mFileHndle(-1),
mFileOffset(0),
mFixedFileSize(FixedFileSize),
mFileSize(FileDataSize),
mFileAccess(FileAccess),
mBinAscii(binary),
mFileRunStatus(CeFile_Idle)
{
  /* To create a Binary File with Fixed size use following command:
   * sudo dd if=/dev/zero of=./MainE2p.bin bs=16 count=64
   * sector size should be a multiply of 16
   * Total size = bs*count
   * /dev/zero is used to set all bytes to 0x00
   */
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "dd if=/dev/zero of=%s bs=16 count=%d", FilePath.c_str(), FileDataSize / 16);
    FILE *fd = popen(cmd,"w");
    if(fd){
        ALOGD(TAG, __FUNCTION__, "MainE2p.bin Binary File Created");
        pclose(fd);
    }

	InitFileAccess(FilePath, FileAccess, binary, FixedFileSize, FileDataSize);
}

FileIo::FileIo(std::string FilePath, FileAccessType FileAccess, FileType binary):
mFileHndle(-1),
mFileOffset(0),
mFixedFileSize(false),
mFileSize(0),
mFileAccess(FileAccess),
mBinAscii(binary),
mFileRunStatus(CeFile_Idle)
{
	ALOGD(TAG, __FUNCTION__,"CTOR");
}

void FileIo::InitFileAccess(std::string FilePath, FileAccessType FileAccess, FileType binary, bool FixedFileSize, unsigned int FileDataSize)
{
	if(mBinAscii == CeFile_ASCII) {
		if(mFileAccess == CeFile_Read) {
			mFileHndle = open(FilePath.c_str(), O_RDONLY|O_SYNC);
		}else if (mFileAccess == CeFile_Write) {
			mFileHndle = open(FilePath.c_str(), O_WRONLY|O_SYNC);
		}else {
			mFileHndle = open(FilePath.c_str(), O_RDWR|O_SYNC);
		}
	}else {
		ALOGD(TAG, __FUNCTION__, "File Binary Opened");
		if(mFileAccess == CeFile_Read) {
			mFileHndle = open(FilePath.c_str(), O_RDONLY|O_SYNC);
		}else if (mFileAccess == CeFile_Write) {
			mFileHndle = open(FilePath.c_str(), O_WRONLY|O_SYNC);
		}else {
			ALOGD(TAG, __FUNCTION__, "File Opened in Read Write");
			mFileHndle = open(FilePath.c_str(), O_RDWR|O_SYNC);
		}
	}
	if(mFileHndle > 0){
		(void)FileIo_GetSize();
		FileIo_SetStatus(CeFile_Opened);
	}
}

FileIo::~FileIo()
{
	ALOGD(TAG, __FUNCTION__,"DTOR");
	if(mFileHndle > 0){
		close(mFileHndle);
		FileIo_SetStatus(CeFile_Closed);
		mFileHndle = -1;
		ALOGD(TAG, __FUNCTION__, "File Destroyed Successfully");
	}
}

int FileIo::FileIo_ReadStr(char* ReadBuff, unsigned int Offset, unsigned int ReadLen)
{
	int iRet = 0;
	if(FileIo_GetStatus() != CeFile_Opened) {
		ALOGE(TAG, __FUNCTION__, "File is Busy");
		return (iRet = -1);
	}
	FileIo_SetStatus(CeFile_Busy);
	if (!FileIo_SetOffset(Offset)) {
		unsigned int rdSize = 0;
		if((rdSize = static_cast<int>(read(mFileHndle, (void*)ReadBuff, ReadLen))) != ReadLen) {
			ALOGE(TAG, __FUNCTION__, "Not enough Data read = %d", rdSize);
			iRet = -2; //Not All Data have been read
		}
	}else {
		ALOGE(TAG, __FUNCTION__, "Cannot Set Offset = %d", Offset);
		iRet = -3;
	}
	FileIo_SetStatus(CeFile_Opened);
	return iRet;
}

int FileIo::FileIo_WriteStr(char * WriteBuff, unsigned int Offset, unsigned int WriteLen)
{
	int iRet = 0;
	if(FileIo_GetStatus() != CeFile_Opened) {
		ALOGE(TAG, __FUNCTION__, "File is Busy");
		return (iRet = -1);
	}
	FileIo_SetStatus(CeFile_Busy);
	if (!FileIo_SetOffset(Offset)) {
		ALOGD(TAG, __FUNCTION__, "Offset OK 2");
		if(mFileHndle > 0) {
			unsigned int wrSize = 0;
			if((wrSize = static_cast<int>(write(mFileHndle, (void*)WriteBuff, WriteLen))) != WriteLen){
				ALOGE(TAG, __FUNCTION__, "Not all Data have been written = %d", wrSize);
				iRet = -2;
			}else {
				ALOGD(TAG, __FUNCTION__, "Amount Of %d bytes written", wrSize);
				(void)FileIo_GetSize(); //update size after each writing
			}
		}
	}else{
		ALOGE(TAG, __FUNCTION__, "Cannot set Offset=%d", Offset);
		iRet = -3;
	}
	FileIo_SetStatus(CeFile_Opened);
	return iRet;
}

int FileIo::FileIo_SetOffset(unsigned int Offset)
{
	if((Offset >=0) && (Offset < mFileSize) ){
		if(lseek(mFileHndle, Offset, SEEK_SET) == Offset){
			mFileOffset = Offset;
		}else {
			return -1;
		}
	} else {
		return -2;
	}
	return 0;
}


int FileIo::FileIo_Mirroring(string & DestFile)
{
	if(FileIo_GetStatus() != CeFile_Opened) {
		return -1;
	}
	int iRet = 0;
	FileIo_SetStatus(CeFile_Busy);
	int outFile = open(DestFile.c_str(), O_RDWR|O_CREAT|O_SYNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if(outFile > 0){
		unsigned int PrevOffset = mFileOffset;
		if (!FileIo_SetOffset(0)) {
			char * MirrorBuffer = new char[mFileSize+1];
			unsigned int rdSize = static_cast<int>(read(mFileHndle, (void*)&MirrorBuffer[0], mFileSize));
			if(rdSize == mFileSize) {
				unsigned int wrSize = static_cast<int>(write(outFile, (void*)&MirrorBuffer[0], mFileSize));
				if(wrSize == mFileSize) {
					ALOGD(TAG, __FUNCTION__, "Writing to DumpFile \
						wrSize=%d   origSize=%d", wrSize, rdSize);
					if(wrSize != rdSize) {
						ALOGE(TAG, __FUNCTION__, "Error during Writing to DumpFile \
						wrSize=%d   origSize=%d", wrSize, rdSize);
						iRet = -1;
					}
				}
			}
			close(outFile);
			delete [] MirrorBuffer;
			//Fallback to Previous Offset
			(void)FileIo_SetOffset(PrevOffset);
		}else {
			iRet = -2;
		}
	}else {
		iRet = -3;
	}
	FileIo_SetStatus(CeFile_Opened);
	return iRet;
}

int FileIo::FileIo_GetOffset()
{
	if(FileIo_GetStatus() != CeFile_Opened) {
		return -1;
	}
	return mFileOffset;
}

int FileIo::FileIo_CalculateCS(unsigned int uiBegPos, unsigned int uiEndPos)
{
	int iCalcCS = 0;
	if((uiEndPos > 0) && (uiBegPos > 0) && (uiEndPos > uiBegPos)) {
		char * CalcBuffer = new char[uiEndPos-uiBegPos +1];
		int ReadErr = 0;
		if((ReadErr = FileIo_ReadStr(CalcBuffer, uiBegPos, uiEndPos-uiBegPos)) < 0) {
			ALOGE(TAG, __FUNCTION__, "Cannot perform CS reading = %d", ReadErr);
			return -1;
		}
		for (unsigned int i = 0; i < (uiEndPos-uiBegPos +1); i++) {
			iCalcCS += CalcBuffer[i];
		}
		delete [] CalcBuffer;
	}
	return iCalcCS;
}

unsigned int FileIo::FileIo_GetSize()
{
	int uiSize = 0;
	FileIo_SetStatus(CeFile_Busy);
	if(mFixedFileSize)
		uiSize = mFileSize;
	else {
		uiSize = lseek(mFileHndle, 0L, SEEK_END);
		//Fallback to previous File Offset
		lseek(mFileHndle, mFileOffset, SEEK_SET);
		mFileSize = uiSize;
	}
	FileIo_SetStatus(CeFile_Opened);
	return uiSize;
}
