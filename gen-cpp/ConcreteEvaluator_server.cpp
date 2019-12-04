// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "../helpers.hpp"
#include "../options.hpp"
#include "dr_api.h"
#include "ConcreteEvaluator.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <utility>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

class ConcreteEvaluatorHandler : virtual public ConcreteEvaluatorIf {
 public:
  ConcreteEvaluatorHandler() {
    // Your initialization goes here
  }

  void addBreakpoint(const Breakpoint& bp) {
    // Your implementation goes here
    printf("addBreakpoint\n");

    auto addr = AbsOrRelAddr_to_AbsAddr (bp.addr, false);

    dr_mutex_lock (*mutex);
    // We were able to resolve the address now
    if (addr) {
      assert_helper (breakpoints.count (*addr) == 0 || breakpoints[*addr] == 0, "Breakpoint already exists", true);

      breakpoints [*addr] = bp.count;
    } else {
      // We were unable to resolve the address
      assert_helper (bp.addr.__isset.reladdr, "Internal error: Expected a relative address", true);

      RelAddr ra = bp.addr.reladdr;

      assert_helper (deferred_breakpoints.count (ra) == 0, "Breakpoint already exists", true);
      deferred_breakpoints [ra] = bp.count;
    }
    dr_mutex_unlock (*mutex);

  }

  void delBreakpoint(const Breakpoint& bp) {
    // Your implementation goes here
    printf("delBreakpoint\n");

    auto addr = AbsOrRelAddr_to_AbsAddr (bp.addr, false);

    dr_mutex_lock (*mutex);
    if (addr) {
      assert_helper (breakpoints.count (*addr), "Breakpoint not found", true);
      breakpoints.erase (*addr);
      total_flush ();
    } else {
      // We were unable to resolve the address
      assert_helper (bp.addr.__isset.reladdr, "Internal error: Expected a relative address", true);

      RelAddr ra = bp.addr.reladdr;

      assert_helper (deferred_breakpoints.count (ra), "Breakpoint not found", true);
      deferred_breakpoints.erase (ra);
      // There is no reason to flush for a deferred breakpoint because
      // we haven't instrumented anything yet!
    }

    dr_mutex_unlock (*mutex);
  }

  void getBreakpoints(std::set<Breakpoint> & _return) {
    // Your implementation goes here
    printf("getBreakpoints\n");

    dr_mutex_lock (*mutex);
    for (const auto &p : breakpoints) {
      Breakpoint bp;
      AbsOrRelAddr a;
      a.__set_absaddr ((AbsAddr) p.first);
      bp.addr = a;
      bp.count = p.second;
      _return.insert (bp);
    }
    dr_mutex_unlock (*mutex);
  }

  EventType::type executeUntilEvent(const EventTypes& stopEvents) {
    // Your implementation goes here
    printf("executeUntilEvent\n");

    stop_events = stopEvents;

    dr_mutex_lock (*mutex);
    assert_helper ((bool) suspend_params, "Execution is not currently suspended.", true);

    // We must release the mutex before calling
    // resume_application_threads or DR complains about deadlocks
    dr_mutex_unlock (*mutex);

    total_flush ();

    resume_application_threads ();

    // Block until we stop. The caller will obtain the mutex.
    dr_event_wait (*suspend_event);

    // Suspend
    suspend_application_threads ();

    printf("Stop!\n");

    assert_helper ((bool) stopped_context, "Unable to locate stopped context.", true);

    return stopped_context->first;
  }

  void getCodeBlock(CodeBlock& _return) {
    // Your implementation goes here
    dr_printf("getCodeBlock\n");

    dr_mutex_lock (*mutex);

    assert_helper ((bool) stopped_block, "Unable to locate stopped block.", true);

    size_t size = stopped_block->second - stopped_block->first;

    _return.addr = (AbsAddr) stopped_block->first;
    _return.bytes = std::string (stopped_block->first, stopped_block->second);

    dr_mutex_unlock (*mutex);

    module_data_t *moddata = dr_lookup_module ((byte*) _return.addr);
    if (moddata) {
      ModuleInfo mi;
      mi.base = (AbsAddr) moddata->start;

      RelAddr ra;
      ra.modulename = dr_module_preferred_name (moddata);
      OffsetOrSymbol offset;
      offset.__set_offset (_return.addr - mi.base);
      ra.offset = offset;
      mi.addr = ra;
      _return.__set_moduleinfo (mi);
      dr_free_module_data (moddata);
    }
  }

  void getContext(RegisterContext& _return) {
    // Your implementation goes here
    printf("getContext\n");

    dr_mutex_lock (*mutex);

    assert_helper ((bool) stopped_context, "Unable to locate stopped context.", true);

    dr_mcontext_t mc = stopped_context->second;

    dr_mutex_unlock (*mutex);

    for (int i = DR_REG_START_GPR; i < DR_REG_STOP_GPR; i++) {
      const char *name = get_register_name (i);
      reg_t value = reg_get_value (i, &mc);
      dr_printf ("%s = %#Lx\n", name, value);
      std::string svalue ((char*) &value, (char*) (&value) + opnd_size_in_bytes (reg_get_size (i)));
      _return [name] = svalue;
    }

    std::string svalue ((char*) &mc.flags, (char*) (&mc.flags + 1));
    _return ["flags"] = svalue;

  }

};

void start_server () {
  int port = 9090;
  ::std::shared_ptr<ConcreteEvaluatorHandler> handler(new ConcreteEvaluatorHandler());
  ::std::shared_ptr<TProcessor> processor(new ConcreteEvaluatorProcessor(handler));
  ::std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  ::std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  ::std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
}

