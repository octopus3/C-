#ifndef __CENTER_CONFIG_H__
#define __CENTER_CONFIG_H__

struct center_config {
	char* ip;
	int port;

	// ���ݿ⣬redis
	// end 
};

extern struct center_config CENTER_CONF;
#endif

