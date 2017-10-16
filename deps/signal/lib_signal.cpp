#include <iostream>
#include "signal/lib_signal.h"
#include "logger/lib_logger.h"

#define TAG "Signal"
using namespace std;
	
SignalApi::SignalApi(int SignNumber, SignalCallback * SigCb):
mSignNumber(SignNumber)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	mSigAction.sa_handler = SigCb;
	mSigAction.sa_flags = 0;
	sigemptyset( &mSigAction.sa_mask );
	sigaction(SignNumber, &mSigAction, NULL );
	
}

SignalApi::~SignalApi()
{
	ALOGD(TAG, __FUNCTION__, "DTOR");
	signal(mSignNumber, SIG_DFL);

}

int SignalApi::SendSignalApi(pid_t DstPid, int SignNum)
{
	return kill(DstPid, SignNum);
}

