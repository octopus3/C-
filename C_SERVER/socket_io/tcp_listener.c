#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "tcp_listener.h"
//C:\Program Files (x86)\Windows Kits\10\Include\10.0.16299.0\um
#ifdef WIN32
#include<WinSock2.h>
#include<Windows.h>
#pragma comment(lib,"WSOCK32.LIB") //  将WSOCK32.lib库文件加入到本工程中
#endif // WIN32
#include "tcp_session.h"
static int add_client_listener(struct session* s,void* param) {
	fd_set* set = (fd_set*)param;
	FD_SET(s->c_sock, set);
	return 0;
}

static int process_client_session(struct session*s,void* param) {
	fd_set* set = (fd_set*)param;
	//句柄集合有数据
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
	WSADATA wsadata; //这个结构体用来存储WSAStartup函数调用后返回的Windows Sockets数据。它包含了Winsock.dll执行的数据
	//配置socket版本信息
	WSAStartup(MAKEWORD(2, 2), &wsadata); //MAKEWORD为socket版本
#endif
	//创建socket，监听客户端连接
int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//TCP的socket，AF_INET:IPv4 网络协议的套接字类型，SOCK_STREAM:向系统申请一个通信接口，IPPROTO_TCP:TCP协议
if (server_socket == INVALID_SOCKET) { //INVALID_SOCKET代表无效SOCKET
	failed(&server_socket);
	return;
}
printf("正在绑定socket到端口:%d ...\n", port);
struct sockaddr_in addr;
addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //配置IP地址 inet_addr()用于将点分十进制IP地址转化为长整数型数（u_long类型）
addr.sin_port = htons(port);//配置端口 htons()整型变量从主机字节顺序转变成网络字节顺序， 就是整数在地址空间存储方式变为高位字节存放在内存的低地址处
addr.sin_family = AF_INET; //配置协议
int ret = bind(server_socket, (const struct sockaddr_in*)&addr, sizeof(addr)); //返回0则证明成功 bind()将一本地地址与一套接口捆绑。

if (0 != ret) { //绑定失败
	printf("bind %s:%d failed","127.0.0.1\n", port);
	failed(&server_socket);
	return;
}
//绑定成功
printf("bind %s:%d success","127.0.0.1\n", port);

//监听开始
printf("开始监听端口：%d\n", port);
ret = listen(server_socket, 128); //允许等待最大连接数为128
if (ret != 0) {
	printf("listening on port:%d failed\n", port);
	failed(&server_socket);
	return;
}
printf("listening on port:%d success\n", port);

init_session_manager();
//处理socket数据
fd_set server_fd_set;//句柄集合 fd_set由内核根据IO状态修改fd_set的内容，由此来通知执行了select()的进程哪一socket或文件发生了可读或可写事件。
while (1) {//select模型,处理socket事件
	FD_ZERO(&server_fd_set);//初始化句柄集合
	FD_SET(server_socket, &server_fd_set);// 添加socket到句柄集合
	foreach_online_session(add_client_listener,(void*)&server_fd_set);
	printf("waiting  the client in....\n");
	ret = select(0, &server_fd_set,NULL,NULL,NULL);//传0代表把集合中所有的句柄
	if (ret < 0) { //句柄无效
		printf("select error\n"); 
	//	assert(1 == 0); //抛出断言
	}
	else if (ret == 0) {//超时，返回错误
		continue;
	}
	else
	{
		printf("waiting for data\n");
	}
		//步骤
		//1、监听端口接入信息
	if (FD_ISSET(server_socket, &server_fd_set)) {  //监听端口有可读数据
		//将端口监听信息打印出来
		struct sockaddr_in c_addr;
		int len = sizeof(c_addr);
		int client_socket = accept(server_socket,(struct sockaddr*)&c_addr,&len);
		if (client_socket != INVALID_SOCKET) {//
			printf("new client %s:%d come in ",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port)); //inet_ntoa()把地址转换成字符串类型，ntohs()将一个16位数由网络字节顺序转换为主机字节顺序
			save_session(client_socket,inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));
		}

	}

		//2、所有接入session的socket信息
	foreach_online_session(process_client_session, &server_fd_set);//
	clear_offline_session();
}
exit_session_manager();
#ifdef WIN32
	WSACleanup();
#endif // WIN32

}

