#include <iostream>
#include <stdio.h>
#include "dynamic/lib_dynamic.h"

using namespace std;

int ALOGX(char* tag, int priority, char* message)
{
    cout << "Library Version: " << DYNAMIC_VERSION << endl;
    #ifdef LOGDEBUG
    char buffer[1024]={0};
    sprintf(buffer,"%s\t%d\t%s",tag, priority, message);
    cout << buffer << endl;
    #else
    cout << "Logging Feature is not supported" << endl;
    #endif
    return 0;
}
