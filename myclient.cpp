#include "dr_api.h"
#include "droption.h"
#include "options.hpp"
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

//std::pair <uintptr_t, size_t> fastforward_params = {0, 0};

static bool fastforwarding = false;

typedef struct _per_thread_t {
    /* Holds shadow values for general purpose registers. The shadow memory
     * currently uses UMBRA_MAP_SCALE_DOWN_4X, which implies that each 4-byte
     * aligned location is represented as one byte. We imitate this here.
     */
    byte shadow_gprs[DR_NUM_GPR_REGS];
} per_thread_t;

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

extern "C" {
  void __ctype_init (void);
}

static void suspend (void) {
  uint num_unsuspended;
  suspend_params = std::make_pair ((void**) 0, 0U);
  DR_ASSERT (dr_suspend_all_other_threads (&(suspend_params->first), &(suspend_params->second), &num_unsuspended));
}

static void server_thread (void* param) {
  dr_printf ("Server thread\n");
  dr_client_thread_set_suspendable (false);

  suspend ();

  __ctype_init ();

  start_server ();
}

// static void worker_thread (void* param) {
//   //void** drcontexts;
//   //__ctype_init ();

//   dr_printf("Worker thread\n");
//   uint num_unsuspended;
//   suspend_params = std::make_pair ((void**) 0, 0U);
//   DR_ASSERT (dr_suspend_all_other_threads (&(suspend_params->first), &(suspend_params->second), &num_unsuspended));

//   while (true) {
//     dr_printf("Worker thread is sleeping\n");
//     dr_sleep (5000);
//     // assert (dr_resume_all_other_threads (drcontexts, num_suspended));
//     // dr_sleep (50);
//   }
// };

static void
event_thread_init (void *drcontext) {
  per_thread_t *data = (per_thread_t *) dr_thread_alloc (drcontext, sizeof (per_thread_t));
  memset (data, 0, sizeof (per_thread_t));
  dr_set_tls_field (drcontext, data);
}

static void
event_thread_exit (void *drcontext) {
  per_thread_t *data = (per_thread_t *) dr_get_tls_field (drcontext);
  dr_thread_free (drcontext, data, sizeof (per_thread_t));
}

static dr_emit_flags_t
event_basic_block (void *drcontext, void *tag, instrlist_t *bb,
                   bool for_trace, bool translating);

void suspender_thread (void *) {
  dr_printf ("Suspender thread\n");
  dr_client_thread_set_suspendable (false);

  while (true) {
    dr_printf ("Waiting in suspender thread\n");
    dr_event_wait (*suspend_event);
    dr_printf ("Suspender thread awake!\n");
    suspend ();
  }
}

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

  suspend_event = dr_event_create ();

  dr_printf ("Spawning suspender thread\n");
  dr_create_client_thread (suspender_thread, NULL);
  dr_printf ("Done spawning suspender thread\n");
  
  dr_printf ("Spawning server thread.\n");
  dr_create_client_thread (server_thread, NULL);
  dr_printf ("Done spawning server thread.\n");

  // uintptr_t fastforward_addr = boost::lexical_cast<HexTo<uintptr_t>> (fastforward_opt.get_value ().first);
  // size_t fastforward_count = boost::lexical_cast<size_t> (fastforward_opt.get_value ().second);
  // fastforward_params = std::make_pair (fastforward_addr, fastforward_count);
  // fastforwarding = true;

  /* register events */
  dr_register_bb_event (event_basic_block);
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

static void update_fastforward_count (void) {
  dr_printf("Updating count\n");
  if (--fastforward_params->second == 0) {
    fastforwarding = false;
    dr_printf ("Done fast forwarding!\n");
    dr_event_signal (*suspend_event);
    dr_sleep (1000);
    // XXX: Suspend all threads except for the server thread
  }
}

static dr_emit_flags_t
event_basic_block (void *drcontext, void *tag, instrlist_t *bb,
                   bool for_trace, bool translating)
{
  if (fastforward_params) {
    for (instr_t *instr = instrlist_first(bb); instr != NULL; instr = instr_get_next(instr)) {

      if (instr_get_app_pc (instr) == (void*) fastforward_params->first) {
        dr_printf("Found it...\n");
        dr_insert_clean_call (drcontext, bb, instr, (void*) update_fastforward_count, false, 0);
        break;
      }
    }
  }
   {
    // dr_insert_clean_call (drcontext, bb, instrlist_first (bb), (void*) clean_call, false, 1,
    //                       OPND_CREATE_INTPTR (instr_get_app_pc (instrlist_first (bb))));


    for (instr_t *instr = instrlist_first(bb); instr != NULL; instr = instr_get_next(instr)) {
      static char buf[1000];
      instr_disassemble_to_buffer (drcontext, instr, buf, 1000);
      dr_printf ("Instr @%#Lx %s\n", instr_get_app_pc (instr), buf);

      // Dst
      if (false) {
      auto num_dsts = instr_num_dsts (instr);
      for (unsigned int i = 0; i < num_dsts; i++) {
        opnd_t op = instr_get_dst (instr, i);
        static char opbuf[1000];
        opnd_disassemble_to_buffer (drcontext, op, opbuf, 1000);
        dr_printf ("Dstop %d = %s\n", i, opbuf);
        if (opnd_is_memory_reference (op)) {
          dr_printf ("Dstop %d is a memory reference\n", i);
        }
        if (opnd_is_reg (op)) {
          reg_id_t r = opnd_get_reg (op);
          dr_printf ("Dstop %d is a register %s\n", i, get_register_name (opnd_get_reg (op)));
        }
      }

      // Src
      auto num_srcs = instr_num_srcs (instr);
      for (unsigned int i = 0; i < num_srcs; i++) {
        opnd_t op = instr_get_src (instr, i);
        static char opbuf[1000];
        opnd_disassemble_to_buffer (drcontext, op, opbuf, 1000);
        dr_printf ("Srcop %d = %s\n", i, opbuf);
        if (opnd_is_memory_reference (op)) {
          dr_printf ("Srcop %d is a memory reference\n", i);
        }
        if (opnd_is_reg (op)) {
          reg_id_t r = opnd_get_reg (op);
          dr_printf ("Srcop %d is a register %s\n", i, get_register_name (opnd_get_reg (op)));
        }
      }
      }

    }
  }
  return DR_EMIT_DEFAULT;
}
