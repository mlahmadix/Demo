#include <iostream>
#include <fstream>
#include "logger/lib_logger.h"

using namespace std;


int ALOGX(char* tag, int priority, char* message)
{
    #ifdef LOGDEBUG
    cout << "Logger Version: " << LOGGER_VERSION << endl;
    char buffer[1024]={0};
    sprintf(buffer,"%s\t%d\t%s",tag, priority, message);
    cout << buffer << endl;
    #else
    cout << "Logging Feature is not supported" << endl;
    #endif
    return 0;
}
