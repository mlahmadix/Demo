#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "sharedMem/lib_sharedMem.h"
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>

using namespace std;
using namespace boost::interprocess;

Shared_Memory::Shared_Memory(boost::interprocess::open_or_create_t m_mode, char * name, size_t size, char* segment_name):
mshmName(name)
{
    cout << __FUNCTION__ << endl;
    shm_obj = managed_shared_memory
      (m_mode                       //m_mode
      ,name                         //name
      ,size                         //size
      );

   //construct segment
   shm_obj.find_or_construct<int>(segment_name)();
}

Shared_Memory::~Shared_Memory()
{
    cout << __FUNCTION__ << endl;
	shared_memory_object::remove(mshmName);
}

int Shared_Memory::read_from_sharedMemory()
{
    std::pair<int*, std::size_t> p = shm_obj.find<int>(mshmName);
     if (p.first)
       return *p.first;
}

void Shared_Memory::write_in_shared_memory(int value)
{
    shm_obj.find_or_construct<int>(mshmName)(value);
}
