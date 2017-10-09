#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "sharedMem/lib_sharedMem.h"

using namespace std;
using namespace boost::interprocess;

Shared_Memory::Shared_Memory(char * name, size_t size):
mshmName(name),
mShmSize(size),
mShmStatus(CeShm_OK)
{
   shared_memory_object::remove(mshmName);
   cout << "BOOST Shared Memory CTOR" << endl;
   mShmobj = shared_memory_object(open_or_create, (char*)mshmName, read_write);
   
   //set the size of the memory object
   mShmobj.truncate(mShmSize);
}

Shared_Memory::~Shared_Memory()
{
    cout << "BOOST Shared Memory DTOR" << endl;
	bool removed = shared_memory_object::remove(mshmName);
	cout << "BOOST Shared Memory removed = " 
	     << removed << endl;
}

int Shared_Memory::sharedMemoryRead(void * BufferData, int Offset, int size)
{
	if( (size <= 0) || (size > mShmSize) ) {
		cerr << "Wrong passed Size" << endl;
		SharedMemorySetStatus(CeShm_WrongSize);
		return CeShm_WrongSize;
	}
	
	if( (Offset < 0) || ((Offset + size) >= mShmSize) ) {
		cerr << "Wrong passed Offset" << endl;
		SharedMemorySetStatus(CeShm_WrongOffset);
		return CeShm_WrongOffset;
	}

	//map the whole shared memory in this process
	mapped_region region(mShmobj,read_write, Offset, size);

	//get pointer to data
	void *mem_map = static_cast<void*>(region.get_address());
	
	if(mem_map == NULL) {
		SharedMemorySetStatus(CeShm_ReadErr);
		cerr << "Cannot read From SHM" << endl;
		return CeShm_ReadErr;
	}
	
	memcpy(BufferData, mem_map, size);

	return size;
}

int Shared_Memory::sharedMemoryWrite(void * BufferData, int Offset, int size)
{
	if( (size <= 0) || (size > mShmSize) ) {
		cerr << "Wrong passed Size" << endl;
		SharedMemorySetStatus(CeShm_WrongSize);
		return CeShm_WrongSize;
	}
	
	if( (Offset < 0) || ((Offset + size) >= mShmSize) ) {
		cerr << "Wrong passed Offset" << endl;
		SharedMemorySetStatus(CeShm_WrongOffset);
		return CeShm_WrongOffset;
	}
     //Map the whole shared memory in this process
   mapped_region region(mShmobj,read_write, Offset, size);
   
   if(region.get_address() == NULL) {
	   	SharedMemorySetStatus(CeShm_WriteErr);
		return CeShm_WriteErr;
   }

   //Obtain the shared structure
   memcpy(region.get_address(), BufferData, size);
   
   return size;
}
