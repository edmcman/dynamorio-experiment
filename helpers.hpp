#include "dr_api.h"
#include "options.hpp"

static void resume_application_threads (void) {
  stopped_context = std::nullopt;
  stopped_block = std::nullopt;
  assert (dr_resume_all_other_threads (suspend_params->first, suspend_params->second));
  suspend_params = std::nullopt;
}

static void suspend_application_threads (void) {
  uint num_unsuspended;
  suspend_params = std::make_pair ((void**) 0, 0U);
  DR_ASSERT (dr_suspend_all_other_threads (&(suspend_params->first), &(suspend_params->second), &num_unsuspended));
}

static void suspend_helper (const std::optional<EventType::type> &e, app_pc start, app_pc end) {
  // Save the event type
  void *drcontext = dr_get_current_drcontext ();
  dr_mcontext_t mcontext = {
    sizeof (mcontext),
    DR_MC_ALL,
  };
  dr_get_mcontext (drcontext, &mcontext);
  stopped_context = std::make_optional (std::make_pair (*e, mcontext));
  stopped_block = std::make_optional (std::make_pair (start, end));

  // Signal the suspender thread to suspend all application threads
  // (including this one)
  dr_event_signal (*suspend_event);

  // Give ourselves a little bit of time to be suspended before
  // returning to the application code.
  dr_sleep (1000);
}

static void total_flush (void) {
    assert (dr_delay_flush_region (0UL,
                                   SIZE_MAX,
                                   0,
                                   NULL));
}

static void assert_helper (bool b, std::string msg) {
  if (!b) {
    Exception e;
    e.msg = msg;
    throw e;
  }
}
