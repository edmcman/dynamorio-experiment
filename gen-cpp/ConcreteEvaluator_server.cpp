// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

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

    if (fastforward_params) {
      Exception e;
      e.msg = "Only one breakpoint is supported and one already exists.";
      throw e;
    }

    auto addr = bp.addr;
    auto count = bp.count;
    fastforward_params = std::make_pair (addr, count);
  }

  EventType::type executeUntilEvent(const EventTypes& stopEvents) {
    // Your implementation goes here
    printf("executeUntilEvent\n");

    if (!suspend_params) {
      Exception e;
      e.msg = "Execution is not currently suspended.";
      throw e;
    }

    assert (dr_resume_all_other_threads (suspend_params->first, suspend_params->second));

    // XXX: Block until we stop
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
