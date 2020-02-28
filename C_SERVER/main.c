#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//C:\Program Files (x86)\Windows Kits\10\Include\10.0.16299.0\ucrt

#include"socket_io/tcp_listener.h"
int main(int argc, char** argv) {
	start_tcp_listener(6000);
	return 0;
}	