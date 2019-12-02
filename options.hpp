#include "dr_api.h"
#include "gen-cpp/ConcreteEvaluator.h"
#include <cstdlib>
#include <cstdint>
#include <utility>
#include <optional>

extern std::optional<std::pair <uintptr_t, size_t>> fastforward_params;

// Set whenever threads are suspended
extern std::optional<std::pair <void**, uint>> suspend_params;

extern std::optional<void*> suspend_event;

// What events should we stop for?
extern std::optional<EventTypes> stop_events;

// What was the event and context when we stopped?
extern std::optional<std::pair <EventType::type, dr_mcontext_t>> stopped_context;
