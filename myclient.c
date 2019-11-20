#include "dr_api.h"

#define BUFSIZE 1000

static void
event_exit (void);
static dr_emit_flags_t
event_basic_block (void *drcontext, void *tag, instrlist_t *bb,
                   bool for_trace, bool translating);

DR_EXPORT void
dr_client_main(client_id_t id, int argc, const char *argv[])
{
  /* register events */
  dr_register_exit_event (event_exit);
  dr_register_bb_event (event_basic_block);
}

static void event_exit (void)
{
  /* empty */
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
    instr_disassemble_to_buffer (drcontext,
                                 instr,
                                 buf,
                                 BUFSIZE);
    dr_printf ("Building %s\n", buf);
  }

  dr_insert_clean_call (drcontext, bb, instrlist_first (bb), clean_call, false, 1,
                        OPND_CREATE_INTPTR (instr_get_app_pc (instrlist_first (bb))));

  return DR_EMIT_DEFAULT;
}
