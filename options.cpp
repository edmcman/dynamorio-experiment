#include "options.hpp"

std::optional<void*> mutex;

std::map<app_pc, size_t> breakpoints;

std::optional<std::pair <void**, uint>> suspend_params;

std::optional<void*> suspend_event;

std::optional<EventTypes> stop_events;

std::optional<std::pair <EventType::type, dr_mcontext_t>> stopped_context;

std::optional<std::pair <app_pc, app_pc>> stopped_block;
