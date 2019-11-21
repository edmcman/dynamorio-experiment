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

#define BUFSIZE 1000

static droption_t<std::string> hit_addr_opt
(DROPTION_SCOPE_CLIENT, "hit_addr", "", "Hit address",
 "Do not instrument anything until this address is hit.");

uintptr_t hit_addr = 0;

static bool hit = false;

static dr_emit_flags_t
event_basic_block (void *drcontext, void *tag, instrlist_t *bb,
                   bool for_trace, bool translating);

static void hitit () {
  dr_printf ("We hit the hit address %#Lx!\n", hit_addr);
  hit = true;
}

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

  //hit_addr = boost::lexical_cast<uintptr_t>("0xdeadbeef"); 
  hit_addr = boost::lexical_cast<HexTo<uintptr_t>> (hit_addr_opt.get_value ());

  //dr_fprintf (STDERR, "Hit address: %s\n", hit_addr.get_value ().c_str ());
  
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

static dr_emit_flags_t
event_basic_block (void *drcontext, void *tag, instrlist_t *bb,
                   bool for_trace, bool translating)
{
  static char buf[BUFSIZE];

  for (instr_t *instr = instrlist_first(bb); instr != NULL; instr = instr_get_next(instr)) {

    if (instr_get_app_pc (instr) == (void*) hit_addr) {
      // XXX: Can we do this without a call?
      // instr_t* new_insn = XINST_CREATE_store_1byte (drcontext,
      //                                          opnd_create_abs_addr ((void*) &hit,
      //                                                                OPSZ_PTR),
      //                                          OPND_CREATE_INT8 (1));
      instrlist_meta_postinsert (bb,
                                 instr,
                                 XINST_CREATE_store_1byte (drcontext,
                                                           opnd_create_abs_addr ((void*) &hit,
                                                                                 OPSZ_1),
                                                           OPND_CREATE_INT8 (1)));
      break;
      //dr_insert_clean_call (drcontext, bb, instr, (void*) hitit, false, 0);
    }

    // instr_disassemble_to_buffer (drcontext,
    //                              instr,
    //                              buf,
    //                              BUFSIZE);
    //std::cout << instr_get_app_pc (instr) << std::endl;
    //dr_printf ("Building %s %#Lx\n", buf, instr_get_app_pc (instr));

  }

  if (hit) {
    dr_insert_clean_call (drcontext, bb, instrlist_first (bb), (void*) clean_call, false, 1,
                          OPND_CREATE_INTPTR (instr_get_app_pc (instrlist_first (bb))));
  }
  return DR_EMIT_DEFAULT;
}
