/*
 ============================================================================
 Name        : asue.c
 Author      : yaoyao
 Version     :
 Copyright   : R & D Center of Internet of Things Security
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "asue_interfaces.h"
#include "logtest.h"

char *AE_ip_addr;

int connect_to_ae()
{

    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;

    socklen_t server_addr_length;
	int client_socket;
	
    // 接收缓冲区
    int nRecvBuf = 32*1024; //设置为32K

    //发送缓冲区
    int nSendBuf = 32*1024; //设置为32K

    //设置一个socket地址结构client_addr,代表客户端internet地址, 端口
    bzero(&client_addr,sizeof(client_addr)); //把一段内存区的内容全部设置为0
    client_addr.sin_family = AF_INET;    //internet协议族
    client_addr.sin_addr.s_addr = INADDR_ANY;//INADDR_ANY表示自动获取本机地址
    client_addr.sin_port = htons(0);    //0表示让系统自动分配一个空闲端口
    //创建用于internet的流协议(TCP)socket,用client_socket代表客户端socket
    client_socket = socket(AF_INET,SOCK_STREAM,0);
    if( client_socket < 0)
    {
        perror("ASUE Create Socket error\n");
        return FAIL;
    }
    //把客户端的socket和客户端的socket地址结构联系起来
    if( bind(client_socket,(struct sockaddr*)&client_addr,sizeof(client_addr)))
    {
        perror("ASUE Bind Port error\n");
        return FALSE;
    }

    //设置一个socket地址结构server_addr,代表服务器的internet地址, 端口
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(inet_aton(AE_ip_addr,&server_addr.sin_addr) == 0) //服务器的IP地址来自程序的参数
    {
        perror("AE Server IP Address Error\n");
        return FALSE;
    }
    server_addr.sin_port = htons(CHAT_SERVER_PORT);
    server_addr_length = sizeof(server_addr);


    setsockopt(client_socket,SOL_SOCKET,SO_RCVBUF,(const BYTE *)&nRecvBuf,sizeof(int));
    setsockopt(client_socket,SOL_SOCKET,SO_SNDBUF,(const BYTE *)&nSendBuf,sizeof(int));

    //客户端向服务器发起连接,连接成功后client_socket代表了客户端和服务器的一个socket连接
    if(connect(client_socket,(struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        perror("Connect To AE error\n");
        return FALSE;
    }
    return client_socket;
}

int send_to_ae(int client_socket, BYTE *send_buffer, int send_len)
{

	int length = send(client_socket,send_buffer,send_len,0);
	printf("--- send %d bytes ---\n",length);

    if(length <0){
        printf("Socket Send Data Failed Or Closed\n");
        close(client_socket);
        return FALSE;
    }
	else
		return TRUE;
}


int recv_from_ae(int client_socket, BYTE *recv_buffer, int recv_len)
{
	//bzero(recv_buffer, recv_len);
	
	int length = recv(client_socket,recv_buffer, recv_len, MSG_WAITALL);//MSG_WAITALL
	if (length < 0){
		printf("Receive Data From Server Failed\n");
		return FALSE;
	}else if(length < recv_len){
		printf("Receive data from server less than required.\n");
		return FALSE;
	}else if(length > recv_len){
		printf("Receive data from server more than required.\n");
		return FALSE;
	}
	else{
		printf("--- receive data succeed, %d bytes. ---\n",length);
		return TRUE;
	}

}

void ProcessWAPIProtocol(int new_ae_socket)
{
	char *user_ID = "1";

	EAP_auth_active eap_auth_active_packet;
	EAP_access_auth_requ eap_access_auth_requ_packet;
	EAP_access_auth_resp eap_access_auth_resp_packet;

	printf("\n***\n Connect to ae.\n");
	
	//1) ProcessWAPIProtocolAuthActive, recv from ae
	printf("\n***\n 1) HandleWAPIProtocolAuthActive: \n");
	
	memset((BYTE *)&eap_auth_active_packet, 0, sizeof(EAP_auth_active));
	printf("recv auth active packet from AE...\n");
	recv_from_ae(new_ae_socket, (BYTE *)&eap_auth_active_packet, sizeof(eap_auth_active_packet));
	HandleWAPIProtocolAuthActive(user_ID, &eap_auth_active_packet.auth_active_packet);
	
	//2) ProcessWAPIProtocolAccessAuthRequest, send to ae
	printf("\n***\n 2) ProcessWAPIProtocolAccessAuthRequest: \n");
	//stop for keyboard
	//getchar();

	memset((BYTE *)&eap_access_auth_requ_packet, 0, sizeof(eap_access_auth_requ_packet));
	eap_access_auth_requ_packet.eap_header.code=1;
	eap_access_auth_requ_packet.eap_header.identifier=1;
	eap_access_auth_requ_packet.eap_header.length=sizeof(eap_access_auth_requ_packet);
	eap_access_auth_requ_packet.eap_header.type=192;
	ProcessWAPIProtocolAccessAuthRequest(user_ID,&eap_auth_active_packet.auth_active_packet, &eap_access_auth_requ_packet.access_auth_requ_packet);

	printf("send to ae...\n");
	send_to_ae(new_ae_socket, (BYTE *)&eap_access_auth_requ_packet, sizeof(eap_access_auth_requ_packet));
	
	//3) ProcessWAPIProtocolCertAuthRequest
	//printf("3) ProcessWAPIProtocolCertAuthRequest: \n");
	//4) ProcessWAPIProtocolCertAuthResp
	//printf("4) ProcessWAPIProtocolCertAuthResp: \n");

	//5) ProcessWAPIProtocolAccessAuthResp, recv from ae
	printf("\n***\n 5) HandleWAPIProtocolAccessAuthResp: \n");

	memset((BYTE *)&eap_access_auth_resp_packet, 0, sizeof(EAP_access_auth_resp));
	printf("recv from ae...\n");
	recv_from_ae(new_ae_socket, (BYTE *)&eap_access_auth_resp_packet, sizeof(eap_access_auth_resp_packet));

	if(HandleWAPIProtocolAccessAuthResp(user_ID, &eap_access_auth_requ_packet.access_auth_requ_packet, &eap_access_auth_resp_packet.access_auth_resp_packet) == FALSE){
		printf("Authentication failed!!\n");
		exit(1);
	}else{
		return 0;
	}

}

int main(int argc, char **argv)
{

	int new_ae_socket;
		
	OpenSSL_add_all_algorithms();

        if (argc != 2)
        {
		printf("Usage: ./%s AE_ip_address\n", argv[0]);
		exit(1);
	}

	AE_ip_addr = argv[1];

	new_ae_socket = connect_to_ae();
	
	Testmode = 1;
	tick();
	ProcessWAPIProtocol(new_ae_socket);
	tock();

	return 0;
}


