#include <cstdlib>
#include <cstdint>
#include <utility>
#include <optional>

extern std::optional<std::pair <uintptr_t, size_t>> fastforward_params;

// Set whenever threads are suspended
extern std::optional<std::pair <void**, uint>> suspend_params;
