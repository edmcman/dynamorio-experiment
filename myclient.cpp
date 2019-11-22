#include "dr_api.h"
#include "droption.h"
#include <iostream>
#include <boost/lexical_cast.hpp>

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

std::pair <uintptr_t, size_t> fastforward_params = {0, 0};

static bool fastforwarding = false;

typedef struct _per_thread_t {
    /* Holds shadow values for general purpose registers. The shadow memory
     * currently uses UMBRA_MAP_SCALE_DOWN_4X, which implies that each 4-byte
     * aligned location is represented as one byte. We imitate this here.
     */
    byte shadow_gprs[DR_NUM_GPR_REGS];
} per_thread_t;

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

DR_EXPORT void
dr_client_main(client_id_t id, int argc, const char *argv[])
{
  std::string parse_err;
  int last_index;
  if (!droption_parser_t::parse_argv(DROPTION_SCOPE_CLIENT, argc, argv, &parse_err, &last_index)) {
    dr_fprintf(STDERR, "Usage error: %s\n", parse_err.c_str());
    dr_fprintf(STDERR, "%s\n", droption_parser_t::usage_long (DROPTION_SCOPE_CLIENT).c_str ());
    dr_abort();
  }

  uintptr_t fastforward_addr = boost::lexical_cast<HexTo<uintptr_t>> (fastforward_opt.get_value ().first);
  size_t fastforward_count = boost::lexical_cast<size_t> (fastforward_opt.get_value ().second);
  fastforward_params = std::make_pair (fastforward_addr, fastforward_count);
  fastforwarding = true;

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
  if (--fastforward_params.second == 0) {
    fastforwarding = false;
    dr_printf ("Done fast forwarding!\n");
  }
}

static dr_emit_flags_t
event_basic_block (void *drcontext, void *tag, instrlist_t *bb,
                   bool for_trace, bool translating)
{
  if (fastforwarding) {
    for (instr_t *instr = instrlist_first(bb); instr != NULL; instr = instr_get_next(instr)) {

      if (instr_get_app_pc (instr) == (void*) fastforward_params.first) {
        dr_insert_clean_call (drcontext, bb, instr, (void*) update_fastforward_count, false, 0);
        break;
      }
    }
  }
  else {
    dr_insert_clean_call (drcontext, bb, instrlist_first (bb), (void*) clean_call, false, 1,
                          OPND_CREATE_INTPTR (instr_get_app_pc (instrlist_first (bb))));


    for (instr_t *instr = instrlist_first(bb); instr != NULL; instr = instr_get_next(instr)) {
      static char buf[1000];
      instr_disassemble_to_buffer (drcontext, instr, buf, 1000);
      dr_printf ("Instr @%#Lx %s\n", instr_get_app_pc (instr), buf);

      // Dst
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
  return DR_EMIT_DEFAULT;
}
