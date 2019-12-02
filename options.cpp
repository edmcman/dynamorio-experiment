#include "options.hpp"

std::optional<std::pair <uintptr_t, size_t>> fastforward_params;

std::optional<std::pair <void**, uint>> suspend_params;

std::optional<void*> suspend_event;

std::optional<EventTypes> stop_events;

std::optional<EventType::type> stopped_event;
