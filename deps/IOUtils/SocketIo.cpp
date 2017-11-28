#include "IOUtils/SocketIo.h"


TCPServer::TCPServer(unsigned short usTcpPort)
{
	mConnectedStatus = false;
	cout << "TCP Server CTOR" << endl;
	ServerSockfd=socket(AF_INET,SOCK_STREAM,0);
 	memset(&TcpServerAddr,0,sizeof(TcpServerAddr));
	TcpServerAddr.sin_family=AF_INET;
	TcpServerAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	TcpServerAddr.sin_port=htons(usTcpPort);
	bind(ServerSockfd,(struct sockaddr *)&TcpServerAddr, sizeof(TcpServerAddr));
 	listen(ServerSockfd,1);
 	pthread_create(&TcpServerThread,NULL,&TcpAcceptCallback,this);
}

TCPServer::~TCPServer()
{
	cout << "TCP Server DTOR" << endl;

	
}

void * TCPServer::TcpAcceptCallback(void *context)
{
	char MsgRecv[4096]={0};
	int read_size = -1;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	TCPServer * TCPServerInst = static_cast<TCPServer *>(context);
	while(1)
	{
		socklen_t sosize  = sizeof(TcpClientAddr);
		//Accept is a blocking Task
		TCPServerInst->ClientSockfd = accept(TCPServerInst->ServerSockfd,(struct sockaddr*)&TCPServerInst->TcpClientAddr,&sosize);
		if(TCPServerInst->ClientSockfd > 0)
			TCPServerInst->mConnectedStatus = true;
		else
			TCPServerInst->mConnectedStatus = false;
		while( (read_size = recv(TCPServerInst->ClientSockfd , MsgRecv , 4096 , 0)) > 0 )
		{
			cout << MsgRecv << endl;
		}
		 
		if(read_size == 0)
		{
			cout << "Client disconnected" << endl;
			close(TCPServerInst->ClientSockfd);
			TCPServerInst->ClientSockfd = -1;
			continue;
		}
		else if(read_size == -1)
		{
			cout << "recv failed" << endl;
			close(TCPServerInst->ClientSockfd);
			TCPServerInst->ClientSockfd = -1;
			continue;
		}
	}
}

void TCPServer::TcpSendCallback(char * pucTcpBuffer)
{
	if(ClientSockfd > 0){
		send(ClientSockfd, pucTcpBuffer, strlen(pucTcpBuffer), 0);
	}
}

void TCPServer::StopTcpServer()
{
	pthread_cancel(TcpServerThread);
	close(ServerSockfd);
	close(ClientSockfd);
	mConnectedStatus = false;
}

