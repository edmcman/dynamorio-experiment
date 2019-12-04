#include "dr_api.h"
#include "droption.h"
#include "options.hpp"
#include "helpers.hpp"
#include <iostream>
#include <boost/lexical_cast.hpp>

void start_server ();

template <typename ElemT>
struct HexTo {
    ElemT value;
    operator ElemT() const {return value;}
    friend std::istream& operator>>(std::istream& in, HexTo& out) {
        in >> std::hex >> out.value;
        return in;
    }
};

static droption_t<twostring_t> fastforward_opt
(DROPTION_SCOPE_CLIENT, "fastforward", std::make_pair ("", ""), "Fastforward",
 "Do not instrument anything until this (address, tuple) is reached.");

// typedef struct _per_thread_t {
//     /* Holds shadow values for general purpose registers. The shadow memory
//      * currently uses UMBRA_MAP_SCALE_DOWN_4X, which implies that each 4-byte
//      * aligned location is represented as one byte. We imitate this here.
//      */
//     byte shadow_gprs[DR_NUM_GPR_REGS];
// } per_thread_t;

extern "C" {
  void __ctype_init (void);
}

// Dummy thread to work around DR i#3973
static void dummy_thread (void* param) {}

static void server_thread (void* param) {
  dr_printf ("Server thread\n");
  dr_client_thread_set_suspendable (false);

  suspend_application_threads ();

  __ctype_init ();

  start_server ();
}

// static void
// event_thread_init (void *drcontext) {
//   per_thread_t *data = (per_thread_t *) dr_thread_alloc (drcontext, sizeof (per_thread_t));
//   memset (data, 0, sizeof (per_thread_t));
//   dr_set_tls_field (drcontext, data);
// }

// static void
// event_thread_exit (void *drcontext) {
//   per_thread_t *data = (per_thread_t *) dr_get_tls_field (drcontext);
//   dr_thread_free (drcontext, data, sizeof (per_thread_t));
// }

static dr_emit_flags_t
event_basic_block (void *drcontext, void *tag, instrlist_t *bb,
                   bool for_trace, bool translating);

static void
event_module_loaded (void *drcontext, const module_data_t *modinfo, bool loaded);

DR_EXPORT void
dr_client_main(client_id_t id, int argc, const char *argv[])
{

  std::string parse_err;
  int last_index;
  if (!droption_parser_t::parse_argv(DROPTION_SCOPE_CLIENT, argc, argv, &parse_err, &last_index)) {
    dr_fprintf(STDERR, "Usage error: %s\n", parse_err.c_str());
    dr_fprintf(STDERR, "%s\n", droption_parser_t::usage_short (DROPTION_SCOPE_CLIENT).c_str ());
    dr_abort ();
    dr_fprintf(STDERR, "%s\n", droption_parser_t::usage_long (DROPTION_SCOPE_CLIENT).c_str ());
    dr_abort();
  }

  mutex = dr_mutex_create ();
  suspend_event = dr_event_create ();

  // Dummy thread to work around DR i#3973
  dr_create_client_thread (dummy_thread, NULL);

  dr_printf ("Spawning server thread.\n");
  dr_create_client_thread (server_thread, NULL);
  dr_printf ("Done spawning server thread.\n");

  /* register events */
  dr_register_bb_event (event_basic_block);
  dr_register_module_load_event (event_module_loaded);
}

static void clean_call (void* addr) {
  void *drcontext = dr_get_current_drcontext ();
  dr_mcontext_t mcontext = {
    sizeof (mcontext),
    DR_MC_ALL,
  };
  dr_get_mcontext (drcontext, &mcontext);
  dr_printf ("Executing block at %#Lx, rax=%#Lx, pc=%#Lx\n", addr, mcontext.rax, mcontext.pc);
}

static void update_fastforward_count (app_pc addr, app_pc start, app_pc end) {
  bool stale = false;

  dr_mutex_lock (*mutex);
  if (breakpoints.count (addr)) {
    auto new_count = --breakpoints.at (addr);
    breakpoints.at (addr) = new_count;

    if (new_count == 0) {
      breakpoints.erase (addr);
      dr_printf ("Done fast forwarding!\n");

      if (stop_events->count (EventType::Breakpoint)) {
        suspend_helper (std::make_optional (EventType::Breakpoint), start, end);
      }
    } else {
      dr_printf ("Count at %d\n", new_count);
    }
  } else {
    dr_printf ("Removing stale instrumentation at %#Lx\n", addr);
    stale = true;
  }

  dr_mutex_unlock (*mutex);
  // We need to flush without holding the lock
  if (stale) total_flush ();
}

static void relevant_block_clean_call (app_pc start, app_pc end) {
  if (stop_events->count (EventType::RelevantBlock)) {
    dr_printf ("relevant_block! %#Lx %#Lx\n", start, end);
    suspend_helper (std::make_optional (EventType::RelevantBlock), start, end);
  } else {
    dr_printf("Ignoring stale instrumentation\n");
  }
}

static void
event_module_loaded (void *drcontext, const module_data_t *modinfo, bool loaded)
{
  bool need_flush = false;

  const char* name = dr_module_preferred_name (modinfo);
  dr_printf ("Image loaded %s\n", name);

  std::set<RelAddr> to_delete;

  dr_mutex_lock (*mutex);
  // Are there any deferred breakpoints in this module that we can activate?
  for (const auto &p : deferred_breakpoints) {
    if (p.first.modulename == name) {
      AbsOrRelAddr a;
      a.__set_reladdr (p.first);
      auto addr = AbsOrRelAddr_to_AbsAddr (a, true);
      DR_ASSERT_MSG (addr, "Internal error: expected to resolve address");
      DR_ASSERT_MSG (*addr >= modinfo->start && *addr <= modinfo->end, "Deferred breakpoint offset was invalid");

      breakpoints [*addr] = p.second;
      to_delete.insert (p.first);
    }
  }

  for (const auto &dbp : to_delete) {
    deferred_breakpoints.erase (dbp);
  }
  dr_mutex_unlock (*mutex);

  if (need_flush) total_flush ();
}

static dr_emit_flags_t
event_basic_block (void *drcontext, void *tag, instrlist_t *bb,
                   bool for_trace, bool translating)
{

  instr_t *first = instrlist_first_app (bb);
  instr_t *last = instrlist_last_app (bb);
  app_pc firstpc = instr_get_app_pc (first);
  app_pc lastpc = instr_get_app_pc (last) + instr_length (drcontext, last);

  dr_mutex_lock (*mutex);

  // Absolute breakpoints
  if (std::any_of (breakpoints.begin (),
                   breakpoints.end (),
                   [&] (const auto &p) { return firstpc <= p.first && p.first <= lastpc; })) {
    for (instr_t *instr = instrlist_first(bb); instr != NULL; instr = instr_get_next(instr)) {

      if (breakpoints.count (instr_get_app_pc (instr))) {
        dr_printf("Found one...\n");
        dr_insert_clean_call (drcontext, bb, instr, (void*) update_fastforward_count, false, 3, OPND_CREATE_INTPTR (instr_get_app_pc (instr)), OPND_CREATE_INTPTR (firstpc), OPND_CREATE_INTPTR (lastpc));
      }
    }
  }

  // Relative breakpoints
  // module_data_t* mod = dr_lookup_module (firstpc);
  // if (mod) {
  //   std::string modname = dr_module_preferred_name (mod);

  //   if (std::any_of (deferred_breakpoints.begin (),
  //                  deferred_breakpoints.end (),
  //                  [&] (const auto &p) {
  //                      p.first.modulename == modname &&
  //                        firstpc <= mod->end &&
  //                        lastpc >= mod->start
  //                  })) {
  //     for (instr_t *instr = instrlist_first(bb); instr != NULL; instr = instr_get_next(instr)) {
  //       app_pc offset = instr_get_app_pc (instr) - mod->start;
  //       if (breakpoints.count (instr_get_app_pc (instr))) {
  //         dr_printf("Found one...\n");
  //         dr_insert_clean_call (drcontext, bb, instr, (void*) update_fastforward_count, false, 3, OPND_CREATE_INTPTR (instr_get_app_pc (instr)), OPND_CREATE_INTPTR (firstpc), OPND_CREATE_INTPTR (lastpc));
  //       }
  //     }
  //   }
  // }

  if (stop_events->count (EventType::RelevantBlock)) {
    instr_t *instr = instrlist_first (bb);
    dr_insert_clean_call (drcontext, bb, instr, (void*) relevant_block_clean_call, false, 2, OPND_CREATE_INTPTR (firstpc), OPND_CREATE_INTPTR (lastpc));
  }

  dr_mutex_unlock (*mutex);

  return DR_EMIT_DEFAULT;
}
