#define __LIB_SHAREDMEM_H__

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <sys/stat.h>

class Shared_Memory
{
    public:
        Shared_Memory(boost::interprocess::open_or_create_t m_mode, char * name, size_t size, char* segment_name);
        ~Shared_Memory();
        int read_from_sharedMemory();
        void write_in_shared_memory(int value);
        boost::interprocess::managed_shared_memory shm_obj;
    private:
        char* mshmName;
};
