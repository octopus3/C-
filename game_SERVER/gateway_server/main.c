#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/log.h"

#include "../game_stype.h"
#include "gw_config.h"
#include "../gateway/gateway.h"
#include "server_session.h"

int main(int argc, char** argv) {
	init_log();
	init_server_gateway(WEB_SOCKET_IO, JSON_PROTOCAL);
	// init_server_gateway(WEB_SOCKET_IO, BIN_PROTOCAL);
	// init_server_gateway(TCP_SOCKET_IO, BIN_PROTOCAL);
	// init_server_gateway(TCP_SOCKET_IO, JSON_PROTOCAL);
	// 注册服务的模块;
	// end
	init_server_session_man();
	LOGINFO("start server at %s:%d\n", GW_CONFIG.ip, GW_CONFIG.port);

	start_server(GW_CONFIG.ip, GW_CONFIG.port);
	exit_server_gateway(); 

	return 0;
}
