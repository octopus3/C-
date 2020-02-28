#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/timer.h"
#include "../utils/timer_list.h"

#include "socket/session.h"
#include "gateway.h"


struct timer_list* GATEWAY_TIMER_LIST = NULL;

#define MAX_SERVICES 512

struct {
	struct service_module* services[MAX_SERVICES];
}gateway;


void
init_server_gateway(int socket_type, int protocal_type) {
	memset(&gateway, 0, sizeof(gateway));
	GATEWAY_TIMER_LIST = create_timer_list();
	init_session_manager(socket_type, protocal_type);
}

void
exit_server_gateway() {
	exit_session_manager();
	destroy_timer_list(GATEWAY_TIMER_LIST);
}

void
register_service(int stype, struct service_module* module) {
	if (stype <= 0 || stype >= MAX_SERVICES) {
		// ��ӡerror
		return;
	}

	gateway.services[stype] = module;
	if (module->init_service_module) {
		module->init_service_module(module);
	}
}


void
on_json_protocal_recv_entry(struct session* s, unsigned char* data, int len) {
	data[len] = 0;
	json_t* root = NULL;
	int ret = json_parse_document(&root, data);
	if (ret != JSON_OK || root == NULL) { // ����һ��������json��;
		return;
	}

	// ��ȡ�������������ͣ��ٶ�0(key)Ϊ���������ͣ�
	json_t* server_type = json_find_first_label(root, "0");
	server_type = server_type->child;
	if (server_type == NULL || server_type->type != JSON_NUMBER) {
		goto ended;
	}
	int stype = atoi(server_type->text); // ��ȡ�����ǵķ���š�
	if (gateway.services[stype] && gateway.services[stype]->on_json_protocal_recv) {
		int ret = gateway.services[stype]->on_json_protocal_recv(gateway.services[stype]->module_data, 
			                                          s, root, data, len);
		if (ret < 0) {
			close_session(s);
		}
	} 
	// end 
ended:
	json_free_value(&root);
}

// ����ǰ��4���ֽ�Ϊ��������
void 
on_bin_protocal_recv_entry(struct session* s, unsigned char* data, int len) {
	int stype = ((data[0]) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));
	if (gateway.services[stype] && gateway.services[stype]->on_bin_protocal_recv) {
		int ret = gateway.services[stype]->on_bin_protocal_recv(gateway.services[stype]->module_data,
			s, data, len);
		if (ret < 0) {
			close_session(s);
		}
	}
}

void
on_connect_lost_entry(struct session* s) {
	for (int i = 0; i < MAX_SERVICES; i++) {
		if (gateway.services[i] && gateway.services[i]->on_connect_lost) {
			gateway.services[i]->on_connect_lost(gateway.services[i]->module_data, s);
		}
	}
}

unsigned int
gateway_add_timer(void(*on_timer)(void* udata),
                  void* udata, float after_sec) {
	return add_timer(GATEWAY_TIMER_LIST, on_timer, udata, after_sec);
}

void
gateway_cancel_timer(unsigned int timeid) {
	cancel_timer(GATEWAY_TIMER_LIST, timeid);
}

unsigned int
gateway_schedule(void(*on_timer)(void* udata),
void* udata, float after_sec)
{
	return schedule_timer(GATEWAY_TIMER_LIST, on_timer, udata, after_sec);
}
