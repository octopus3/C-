#ifndef __TCP__SESSION_H_
#define __TCP__SESSION_H_

struct session 
{
	char c_ip[32];
	int c_port;
	int c_sock;
	int removed;	//ɾ����ǣ���ֵΪ1��ɾ��
	void* player;
	struct session* next;
};

void init_session_manager();//ȫ�ֹ���
void exit_session_manager();
//�пͻ��˽��룬����session
struct session* save_session(int c_sock, char* ip, int port);
void foreach_online_session(int(*callback)(struct session* s,void* params),void * params); //����session����
void session_on_recv(struct session* s);
void clear_offline_session(); 
#endif // __TCP_SESSION_H_
