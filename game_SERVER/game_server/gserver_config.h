#ifndef __CENTER_CONFIG_H__
#define __CENTER_CONFIG_H__

struct gserver_config {
	char* ip;
	int port;

	// Êý¾Ý¿â£¬redis
	// end 
};

extern struct gserver_config GSERVER_CONF;
#endif

