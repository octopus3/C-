#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../utils/log.h"
#include "../game_stype.h"
#include "common_config.h"
#include "../gateway/gateway.h"

int main(int argc, char** argv) {
	init_log();
	init_server_gateway(WEB_SOCKET_IO, JSON_PROTOCAL);
	// init_server_gateway(WEB_SOCKET_IO, BIN_PROTOCAL);
	// init_server_gateway(TCP_SOCKET_IO, BIN_PROTOCAL);
	// init_server_gateway(TCP_SOCKET_IO, JSON_PROTOCAL);
	// 注册服务的模块;
	// end
	start_server(COMMON_CONF.ip, COMMON_CONF.port);
	exit_server_gateway(); 

	return 0;
}
