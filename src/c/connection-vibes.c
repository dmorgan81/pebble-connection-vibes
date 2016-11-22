#include <pebble.h>
#include <pebble-events/pebble-events.h>
#include "connection-vibes.h"

#ifdef PBL_HEALTH
static bool s_sleeping;
static EventHandle s_health_event_handle;
#endif

static ConnectionVibesState s_state = ConnectionVibesStateDisconnect;
static uint32_t *s_segments;
static uint32_t s_num_segments;

static bool s_connected;
static EventHandle s_connection_service_event_handle;

#ifdef PBL_HEALTH
static void prv_health_event_handler(HealthEventType event, void *context) {
    if (event == HealthEventSignificantUpdate) {
        prv_health_event_handler(HealthEventSleepUpdate, context);
    } else if (event == HealthEventSleepUpdate || (event == HealthEventMovementUpdate && s_sleeping)) {
        HealthActivityMask mask = health_service_peek_current_activities();
        s_sleeping = (mask & HealthActivitySleep) || (mask & HealthActivityRestfulSleep);
    }
}
#endif

static void prv_vibe(void) {
    if (quiet_time_is_active()) return;

    if (s_segments != NULL) {
        VibePattern pattern = {
            .durations = s_segments,
            .num_segments = s_num_segments
        };
        vibes_enqueue_custom_pattern(pattern);
    } else {
        vibes_double_pulse();
    }
}

static void prv_connection_handler(bool connected) {
#ifdef PBL_HEALTH
    if (s_sleeping) return;
#endif

    switch (s_state) {
        case ConnectionVibesStateNone:
            return;
        case ConnectionVibesStateDisconnect:
            if (!connected) prv_vibe();
            break;
        case ConnectionVibesStateDisconnectAndReconnect:
            if (s_connected != connected) prv_vibe();
            break;
    }
    s_connected = connected;
}

void connection_vibes_init(void) {
    s_connected = connection_service_peek_pebble_app_connection();
    s_connection_service_event_handle = events_connection_service_subscribe((ConnectionHandlers) {
        .pebble_app_connection_handler = prv_connection_handler
    });
}

void connection_vibes_deinit(void) {
    events_connection_service_unsubscribe(s_connection_service_event_handle);
    s_connection_service_event_handle = NULL;

    if (s_segments != NULL) {
        free(s_segments);
        s_segments = NULL;
        s_num_segments = 0;
    }

#ifdef PBL_HEALTH
    if (s_health_event_handle != NULL) {
        events_health_service_events_unsubscribe(s_health_event_handle);
        s_health_event_handle = NULL;
    }
#endif
}

void connection_vibes_set_state(ConnectionVibesState state) {
    s_state = state;
}

void connection_vibes_set_pattern(VibePattern pattern) {
    if (s_segments != NULL) {
        free(s_segments);
        s_segments = NULL;
        s_num_segments = 0;
    }

    if (pattern.num_segments > 0) {
        s_segments = malloc(pattern.num_segments * sizeof(uint32_t));
        memcpy(s_segments, pattern.durations, pattern.num_segments * sizeof(uint32_t));
        s_num_segments = pattern.num_segments;
    }
}

#ifdef PBL_HEALTH
void connection_vibes_enable_health(bool enable) {
    if (enable && s_health_event_handle == NULL) {
        s_health_event_handle = events_health_service_events_subscribe(prv_health_event_handler, NULL);
    } else if (!enable) {
        s_sleeping = false;
        if (s_health_event_handle != NULL) {
            events_health_service_events_unsubscribe(s_health_event_handle);
            s_health_event_handle = NULL;
        }
    }
}
#endif
