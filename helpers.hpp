#include "dr_api.h"
#include "options.hpp"

// MUST LOCK
static void resume_application_threads (void) {
  stopped_context = std::nullopt;
  stopped_block = std::nullopt;
  assert (dr_resume_all_other_threads (suspend_params->first, suspend_params->second));
  suspend_params = std::nullopt;
}

// MUST LOCK
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

static void assert_helper (bool b, std::string msg, bool unlock_mutex_on_error) {
  if (!b) {
    Exception e;
    e.msg = msg;
    if (unlock_mutex_on_error)
      dr_mutex_unlock (*mutex);
    throw e;
  }
}

static std::optional<app_pc> AbsOrRelAddr_to_AbsAddr (const AbsOrRelAddr &a, bool unlock_mutex_on_error) {
  if (a.__isset.absaddr)
    return (app_pc) a.absaddr;
  else if (a.__isset.reladdr) {
    module_data_t *moddata = dr_lookup_module_by_name (a.reladdr.modulename.c_str ());
    //assert_helper (moddata, (std::string ("Unable to locate module: ") + a.reladdr.modulename).c_str (), unlock_mutex_on_error);
    if (!moddata) return std::nullopt;

    app_pc r;
    if (a.reladdr.offset.__isset.offset)
      r = moddata->start + a.reladdr.offset.offset;
    else if (a.reladdr.offset.__isset.symbol) {
      // get module handle
      r = (app_pc) dr_get_proc_address (moddata->handle, a.reladdr.offset.symbol.c_str ());
      assert_helper (r, (std::string ("Unable to locate symbol: ") + a.reladdr.offset.symbol + std::string (" in module: ") + a.reladdr.modulename).c_str (), unlock_mutex_on_error);
    }
    else
      assert_helper (false, "Neither offset nor symbol set in OffsetOrSymbol", unlock_mutex_on_error);

    dr_free_module_data (moddata);
    return r;
  } else {
    assert_helper (false, "AbsOrRelAddr contained neither an absolute or relative address.", unlock_mutex_on_error);
  }
}
