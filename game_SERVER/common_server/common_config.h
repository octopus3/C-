#ifndef __CENTER_CONFIG_H__
#define __CENTER_CONFIG_H__

struct common_config {
	char* ip;
	int port;

	// ���ݿ⣬redis
	// end 
};

extern struct common_config COMMON_CONF;
#endif

