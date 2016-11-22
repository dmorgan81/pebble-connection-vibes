# pebble-connection-vibes

pebble-connection-vibes wraps the [ConnectionService](https://developer.pebble.com/docs/c/Foundation/Event_Service/ConnectionService/) and the [Vibes API](https://developer.pebble.com/docs/c/User_Interface/Vibes/) (and optionally Pebble Health on supported platforms) together. It takes a lot of boilerplate code to implement a service that triggers a vibe on loss of connection to the Pebble app. pebble-connection-vibes reduces that boilerplate to a few lines of code.

Note that pebble-connection-vibes listens soley to connection events for the Pebble App connection.  Listening to a PebbleKit connection is pretty app specific and it's unlikely you'd want to vibe if the PebbleKit connection is lost.

# Installation

```
pebble package pebble-connection-vibes
```

You must be using a new-style project; install the latest pebble tool and SDK and run `pebble convert-project` on your app if you aren't.

# Usage

```c
// This is not a complete example, but should demonstrate the basic usage

#include <pebble-connection-vibes/connection-vibes.h>

...

static void init(void) {
  connection_vibes_init();
  ...
}

static void deinit(void) {
  connection_vibes_deinit();
  ...
}
```

After calling `connection_vibes_init()` you may set additional options such as vibing on disconnect or on disconnect and reconnect (or no vibes at all), a custom vibe pattern, or using Pebble Health.

# pebble-connection-vibes API

| Method | Description |
|--------|---------|
| `void connection_vibes_init(void)` | Initialize everything by subscribing to the ConnectionService |
| `void connection_vibes_deinit(void)` | Deinitialize everything: unsubscribe from services, free memory, etc. |
| `void connection_vibes_set_state(ConnectionVibesState state)` | Set the vibration state to the specified enum value |
| `void connection_vibes_set_pattern(VibePattern pattern)` | Set a custom VibePattern. By default a double pulse is used |
| `void connection_vibes_enable_health(bool enable)` | Register/unregister with Pebble Health |

# pebble-connection-vibes enums

## ConnectionVibesState

| Value | Description |
|--------|---------|
| ConnectionVibesStateNone | No vibes at all. Useful if want to make vibing user configurable |
| ConnectionVibesStateDisconnect | Vibe only on disconnect |
| ConnectionVibesStateDisconnectAndReconnect | Vibe on disconnect and reconnect |

# Note on Pebble Health

On platforms that support Pebble Health, pebble-connection-vibes can subscribe to health events and detect when the user is asleep. Vibes will happen only if the user is awake. This avoids awkward vibes at 0200 that could awaken a user. To enable this, just call `connection_vibes_enable_health(true)` sometime after calling `connection_vibes_init()`.

Pebble Health integration is turned off by default so apps and watchfaces that don't want to integrate with Pebble Health won't get the nag screen thrown up by the watch if the user doesn't have Pebble Health enabled.

# Note on Quiet Time

Starting with Pebble SDK 4.3 the current state of the system quiet time is available. pebble-connection-vibes will respect this on every platform but aplite. On aplite `quiet_time_is_active()` is actually a `#define` that aways evaluates to false.
