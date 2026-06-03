#ifndef NETWORK_H
#define NETWORK_H

#include <stdbool.h>

void network_init(void);
void network_poll(void);
bool network_is_connected(void);
const char* network_get_mode_str(void);

#endif