#ifndef __LIB_SHAREDMEM_H__
#define __LIB_SHAREDMEM_H__

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <sys/stat.h>

class Shared_Memory
{
    public:
		enum CeShmStatus {
			CeShm_OK          =  0,
			CeShm_WrongSize   = -1,
			CeShm_WrongOffset = -2,
			CeShm_CreatErr    = -3,
			CeShm_AdjustSize  = -4,
			CeShm_ReadErr     = -5,
			CeShm_WriteErr    = -6,
			CeShm_KO          = -7,
		};
        Shared_Memory(char * name, size_t size);
        ~Shared_Memory();
        int sharedMemoryRead(void * BufferData, int Offset, int size);
        int sharedMemoryWrite(void * BufferData, int Offset, int size);
        CeShmStatus SharedMemoryGetStatus() { return mShmStatus; }
    private:
		void SharedMemorySetStatus(CeShmStatus status) { mShmStatus = status; }
        char* mshmName;
        boost::interprocess::shared_memory_object mShmobj;
        size_t mShmSize;
        CeShmStatus mShmStatus;
};
#endif
