#ifndef __LIB_SIGNAL_H__
#define __LIB_SIGNAL_H__
#include <csignal>

typedef void (SignalCallback)(int);

class SignalApi
{
	public:
		SignalApi(int SignNumber, SignalCallback * SigCb);
		~SignalApi();
		int SendSignalApi(pid_t DstPid, int SignNum);
	private:
		int mSignNumber;
		struct sigaction mSigAction;
	
};

#endif
