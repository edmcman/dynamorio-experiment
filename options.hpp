#include "dr_api.h"
#include "gen-cpp/ConcreteEvaluator.h"
#include <cstdlib>
#include <cstdint>
#include <utility>
#include <optional>

extern std::optional<void*> mutex;

extern std::map<app_pc, size_t> fastforward_params;

// Set whenever threads are suspended
extern std::optional<std::pair <void**, uint>> suspend_params;

extern std::optional<void*> suspend_event;

// What events should we stop for?
extern std::optional<EventTypes> stop_events;

// What was the event and context when we stopped?
extern std::optional<std::pair <EventType::type, dr_mcontext_t>> stopped_context;

// What was the last block when we stopped? [start, end)
extern std::optional<std::pair <app_pc, app_pc>> stopped_block;
