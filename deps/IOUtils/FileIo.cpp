#include <iostream>
#include "IOUtils/FileIo.h"
#include "logger/lib_logger.h"

using namespace std;

#define TAG "FileIO"

FileIo::FileIo(std::string FilePath, FileAccessType FileAccess, FileType binary, bool FixedFileSize, unsigned int FileDataSize):
mFileHndle(NULL),
mFileOffset(0),
mFixedFileSize(FixedFileSize),
mFileSize(FileDataSize),
mFileAccess(FileAccess),
mBinAscii(binary),
mFileRunStatus(CeFile_Idle)
{
	InitFileAccess(FilePath, FileAccess, binary, FixedFileSize, FileDataSize);
}

FileIo::FileIo(std::string FilePath, FileAccessType FileAccess, FileType binary):
mFileHndle(NULL),
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
			mFileHndle = fopen(FilePath.c_str(), "r");
		}else if (mFileAccess == CeFile_Write) {
			mFileHndle = fopen(FilePath.c_str(), "w");
		}else {
			mFileHndle = fopen(FilePath.c_str(), "w");
		}
	}else {
		if(mFileAccess == CeFile_Read) {
			mFileHndle = fopen(FilePath.c_str(), "rb");
		}else if (mFileAccess == CeFile_Write) {
			mFileHndle = fopen(FilePath.c_str(), "wb");
		}else {
			mFileHndle = fopen(FilePath.c_str(), "wb");
		}
	}
	if(mFileHndle != NULL){
		(void)FileIo_GetSize();
		FileIo_SetStatus(CeFile_Opened);
	}
}

FileIo::~FileIo()
{
	ALOGD(TAG, __FUNCTION__,"DTOR");
	while(FileIo_GetStatus() != CeFile_Opened);
	if(mFileHndle != NULL){
		fflush(mFileHndle);
		fclose(mFileHndle);
		FileIo_SetStatus(CeFile_Closed);
		mFileHndle = NULL;
		ALOGD(TAG, __FUNCTION__, "File Destroyed Successfully");
	}
}

int FileIo::FileIo_ReadStr(char* ReadBuff, unsigned int Offset, unsigned int ReadLen)
{
	int rdSize = 0;
	if(FileIo_GetStatus() != CeFile_Opened) {
		return rdSize;
	}
	FileIo_SetStatus(CeFile_Busy);
	if (!FileIo_SetOffset(Offset)) {
		rdSize = fread(ReadBuff, 1, ReadLen, mFileHndle);
	}else {
		rdSize = 0;
	}
	FileIo_SetStatus(CeFile_Opened);
	return rdSize;
}

int FileIo::FileIo_WriteStr(char * WriteBuff, unsigned int Offset, unsigned int WriteLen)
{
	ALOGD(TAG, __FUNCTION__, "Offset = %d   WriteLen=%d",Offset, WriteLen);
	int wrSize = 0;
	if(FileIo_GetStatus() != CeFile_Opened) {
		ALOGE(TAG, __FUNCTION__, "File is Busy");
		return wrSize;
	}
	FileIo_SetStatus(CeFile_Busy);
	if (!FileIo_SetOffset(Offset)) {
		ALOGD(TAG, __FUNCTION__, "Offset OK 2");
		if(mFileHndle != NULL) {
			wrSize = fwrite(WriteBuff, 1, WriteLen, mFileHndle);
			ALOGD(TAG, __FUNCTION__, "Amount Of %d bytes written", wrSize);
			//(void)FileIo_GetSize(); //update size after each writing
		}
	}else{
		ALOGE(TAG, __FUNCTION__, "Cannot set Offset=%d", Offset);
		wrSize = 0;
	}
	FileIo_SetStatus(CeFile_Opened);
	return wrSize;
}

int FileIo::FileIo_SetOffset(unsigned int Offset)
{
	if((Offset >=0) && (Offset < mFileSize) ){
		if(!fseek(mFileHndle, Offset, SEEK_SET)){
			ALOGD(TAG, __FUNCTION__, "Offset OK 1");
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
	FILE * outFile;
	if(mBinAscii == CeFile_ASCII ) {
		outFile = fopen(DestFile.c_str(), "w");
	} else {
		outFile = fopen(DestFile.c_str(), "wb");
	}
	if(outFile != NULL){
		unsigned int PrevOffset = mFileOffset;
		if (!FileIo_SetOffset(0)) {
			char * MirrorBuffer = new char[mFileSize];
			int rdSize = fread(&MirrorBuffer[0], 1, mFileSize, mFileHndle);
			int wrSize = fwrite(&MirrorBuffer[0], 1, mFileSize, outFile);
			if(wrSize != rdSize) {
				ALOGE(TAG, __FUNCTION__, "Error during Writing to DumpFile \
				wrSize=%d   origSize=%d", wrSize, rdSize);
				iRet = -1;
			}
			delete [] MirrorBuffer;
			//Fallback to Previous Offset
			(void)FileIo_SetOffset(PrevOffset);
		}else {
			iRet = -2;
		}
		fclose(outFile);
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
		if(FileIo_ReadStr(CalcBuffer, uiBegPos, uiEndPos-uiBegPos +1) <= 0) {
			ALOGE(TAG, __FUNCTION__, "Cannot perform CS reading");
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
		fseek(mFileHndle, 0L, SEEK_END);
		uiSize = ftell(mFileHndle);
		fseek(mFileHndle, mFileOffset, SEEK_SET);
		mFileSize = uiSize;
	}
	FileIo_SetStatus(CeFile_Opened);
	return uiSize;
}
