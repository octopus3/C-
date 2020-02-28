#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../gateway/gateway.h"
#include "gw_config.h"
#include "server_session.h"

#define MAX_SERVER 16
struct {
	int need_conect; // �Ƿ���Ҫȥ���
	struct session* server_session[MAX_SERVER];
}SESSION_MAN;

struct session*
get_server_session(int stype) {
	return SESSION_MAN.server_session[stype];
}

void
init_server_session_man() {
	memset(&SESSION_MAN, 0, sizeof(SESSION_MAN));
	SESSION_MAN.need_conect = 1;

	// ÿ��3��ȥ���һ�Σ�����û�жϵ���
	gateway_schedule((void(__cdecl *)(void *))keep_servers_online, NULL, 4);
	// end 
}

void
keep_servers_online() {
	if (SESSION_MAN.need_conect == 0) {
		return;
	}
	SESSION_MAN.need_conect = 0;
	int i = 0;
	// for (i = 0; i < GW_CONFIG.num_server_moudle; i++) {
	for (i = 0; i < 1; i ++) {
		int stype = GW_CONFIG.module_set[i].stype;
		if (SESSION_MAN.server_session[stype] == NULL) {
			SESSION_MAN.server_session[stype] = gateway_connect(GW_CONFIG.module_set[i].ip, GW_CONFIG.module_set[i].port);
			if (SESSION_MAN.server_session[stype] == NULL) {
				SESSION_MAN.need_conect = 1;
				LOGINFO("connected to %s error!!!!!", GW_CONFIG.module_set[i].desic);
			}
			else {
				LOGINFO("connected to %s success!!!!!", GW_CONFIG.module_set[i].desic);
			}
		}
	}
	
}

void
disconnect_server(int stype) {
	SESSION_MAN.server_session[stype] = NULL;
	SESSION_MAN.need_conect = 1;
}