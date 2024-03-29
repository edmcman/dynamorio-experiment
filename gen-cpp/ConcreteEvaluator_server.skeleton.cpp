// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "ConcreteEvaluator.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

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
  }

  void delBreakpoint(const Breakpoint& bp) {
    // Your implementation goes here
    printf("delBreakpoint\n");
  }

  void getBreakpoints(std::set<Breakpoint> & _return) {
    // Your implementation goes here
    printf("getBreakpoints\n");
  }

  EventType::type executeUntilEvent(const EventTypes& stopEvents) {
    // Your implementation goes here
    printf("executeUntilEvent\n");
  }

  void getCodeBlock(CodeBlock& _return) {
    // Your implementation goes here
    printf("getCodeBlock\n");
  }

  void getContext(RegisterContext& _return) {
    // Your implementation goes here
    printf("getContext\n");
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  ::std::shared_ptr<ConcreteEvaluatorHandler> handler(new ConcreteEvaluatorHandler());
  ::std::shared_ptr<TProcessor> processor(new ConcreteEvaluatorProcessor(handler));
  ::std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  ::std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  ::std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

