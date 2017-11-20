#ifndef __FILEIO_H__
#define __FILEIO_H__

#include <fstream>

class FileIo {
	
	protected:
		enum FileAccessType {
			CeFile_Read = 1,
			CeFile_Write = 2,
			CeFile_RdWr = 3
		};
		
		enum FileType {
			CeFile_ASCII,
			CeFile_BIN
		};
		
		enum FileStatus {
			CeFile_Idle,
			CeFile_Opened,
			CeFile_Busy,
			CeFile_Closed,
		};
		
		FileIo(std::string FilePath, FileAccessType FileAccess, FileType binary, bool FixedFileSize, unsigned int FileDataSize);
		FileIo(std::string FilePath, FileAccessType FileAccess, FileType binary);
		~FileIo();
		int FileIo_ReadStr(char * ReadBuff, unsigned int Offset, unsigned int ReadLen);
		int FileIo_WriteStr(char * WriteBuff, unsigned int Offset, unsigned int WriteLen);
		int FileIo_Mirroring(std::string & DestFile);
		unsigned int FileIo_GetSize();
		int FileIo_CalculateCS(unsigned int uiBegPos, unsigned int uiEndPos);
		inline FileStatus FileIo_GetStatus() { return mFileRunStatus; }
		inline void FileIo_SetStatus (FileStatus Status) { mFileRunStatus = Status; }
		
	private:
		int mFileHndle;
		unsigned int mFileOffset;
		bool mFixedFileSize;
		unsigned int mFileSize;
		FileAccessType mFileAccess;
		FileType mBinAscii;
		FileStatus mFileRunStatus;
		int FileIo_GetOffset();
		void InitFileAccess(std::string FilePath, FileAccessType FileAccess, FileType binary, bool FixedFileSize, unsigned int FileDataSize);
		int FileIo_SetOffset(unsigned int Offset);
};
#endif
