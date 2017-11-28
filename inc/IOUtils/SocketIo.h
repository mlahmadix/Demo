#ifndef __SOCKETIO_H__
#define __SOCKETIO_H__


#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

using namespace std;

#define MAXPACKETSIZE 4096

class TCPServer
{
	public:
		TCPServer(unsigned short usTcpPort);
		~TCPServer();
		void TcpSendCallback(char * pucTcpBuffer);
		bool GetTcpServerConnStat() { return mConnectedStatus;}
		void StopTcpServer();

	private:
		void TcpServerdetach();
		static void * TcpAcceptCallback(void * context);
		int ServerSockfd, ClientSockfd, n, pid;
		struct sockaddr_in TcpServerAddr;
		struct sockaddr_in TcpClientAddr;
		pthread_t TcpServerThread;
		//static void * TcpServerHandleTask(void * argv);
		bool mConnectedStatus;
		bool mRunning;
};



#endif
