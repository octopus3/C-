#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "tcp_listener.h"
//C:\Program Files (x86)\Windows Kits\10\Include\10.0.16299.0\um
#ifdef WIN32
#include<WinSock2.h>
#include<Windows.h>
#pragma comment(lib,"WSOCK32.LIB") //  ��WSOCK32.lib���ļ����뵽��������
#endif // WIN32
#include "tcp_session.h"
static int add_client_listener(struct session* s,void* param) {
	fd_set* set = (fd_set*)param;
	FD_SET(s->c_sock, set);
	return 0;
}

static int process_client_session(struct session*s,void* param) {
	fd_set* set = (fd_set*)param;
	//�������������
	if (FD_ISSET(s->c_sock,set)) {
		session_on_recv(s);
	}
}

void failed(int *serve_socket) {
	if (*serve_socket != INVALID_SOCKET) {
		closesocket(*serve_socket);
	}
	
}

void start_tcp_listener(unsigned short port) {
#ifdef WIN32
	WSADATA wsadata; //����ṹ�������洢WSAStartup�������ú󷵻ص�Windows Sockets���ݡ���������Winsock.dllִ�е�����
	//����socket�汾��Ϣ
	WSAStartup(MAKEWORD(2, 2), &wsadata); //MAKEWORDΪsocket�汾
#endif
	//����socket�������ͻ�������
int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//TCP��socket��AF_INET:IPv4 ����Э����׽������ͣ�SOCK_STREAM:��ϵͳ����һ��ͨ�Žӿڣ�IPPROTO_TCP:TCPЭ��
if (server_socket == INVALID_SOCKET) { //INVALID_SOCKET������ЧSOCKET
	failed(&server_socket);
	return;
}
printf("���ڰ�socket���˿�:%d ...\n", port);
struct sockaddr_in addr;
addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //����IP��ַ inet_addr()���ڽ����ʮ����IP��ַת��Ϊ������������u_long���ͣ�
addr.sin_port = htons(port);//���ö˿� htons()���ͱ����������ֽ�˳��ת��������ֽ�˳�� ���������ڵ�ַ�ռ�洢��ʽ��Ϊ��λ�ֽڴ�����ڴ�ĵ͵�ַ��
addr.sin_family = AF_INET; //����Э��
int ret = bind(server_socket, (const struct sockaddr_in*)&addr, sizeof(addr)); //����0��֤���ɹ� bind()��һ���ص�ַ��һ�׽ӿ�����

if (0 != ret) { //��ʧ��
	printf("bind %s:%d failed","127.0.0.1\n", port);
	failed(&server_socket);
	return;
}
//�󶨳ɹ�
printf("bind %s:%d success","127.0.0.1\n", port);

//������ʼ
printf("��ʼ�����˿ڣ�%d\n", port);
ret = listen(server_socket, 128); //����ȴ����������Ϊ128
if (ret != 0) {
	printf("listening on port:%d failed\n", port);
	failed(&server_socket);
	return;
}
printf("listening on port:%d success\n", port);

init_session_manager();
//����socket����
fd_set server_fd_set;//������� fd_set���ں˸���IO״̬�޸�fd_set�����ݣ��ɴ���ִ֪ͨ����select()�Ľ�����һsocket���ļ������˿ɶ����д�¼���
while (1) {//selectģ��,����socket�¼�
	FD_ZERO(&server_fd_set);//��ʼ���������
	FD_SET(server_socket, &server_fd_set);// ���socket���������
	foreach_online_session(add_client_listener,(void*)&server_fd_set);
	printf("waiting  the client in....\n");
	ret = select(0, &server_fd_set,NULL,NULL,NULL);//��0����Ѽ��������еľ��
	if (ret < 0) { //�����Ч
		printf("select error\n"); 
	//	assert(1 == 0); //�׳�����
	}
	else if (ret == 0) {//��ʱ�����ش���
		continue;
	}
	else
	{
		printf("waiting for data\n");
	}
		//����
		//1�������˿ڽ�����Ϣ
	if (FD_ISSET(server_socket, &server_fd_set)) {  //�����˿��пɶ�����
		//���˿ڼ�����Ϣ��ӡ����
		struct sockaddr_in c_addr;
		int len = sizeof(c_addr);
		int client_socket = accept(server_socket,(struct sockaddr*)&c_addr,&len);
		if (client_socket != INVALID_SOCKET) {//
			printf("new client %s:%d come in ",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port)); //inet_ntoa()�ѵ�ַת�����ַ������ͣ�ntohs()��һ��16λ���������ֽ�˳��ת��Ϊ�����ֽ�˳��
			save_session(client_socket,inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));
		}

	}

		//2�����н���session��socket��Ϣ
	foreach_online_session(process_client_session, &server_fd_set);//
	clear_offline_session();
}
exit_session_manager();
#ifdef WIN32
	WSACleanup();
#endif // WIN32

}

