#pragma once
#include <pebble.h>

typedef enum {
    ConnectionVibesStateNone = 0,
    ConnectionVibesStateDisconnect,
    ConnectionVibesStateDisconnectAndReconnect
} ConnectionVibesState;

void connection_vibes_init(void);
void connection_vibes_deinit(void);
void connection_vibes_set_state(ConnectionVibesState state);
void connection_vibes_set_pattern(VibePattern pattern);
#ifdef PBL_HEALTH
void connection_vibes_enable_health(bool enable);
#endif
