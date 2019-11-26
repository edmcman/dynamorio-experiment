typedef binary RegisterValue
typedef string RegisterID

typedef map<RegisterID,RegisterValue> RegisterContext

typedef i64 Addr

struct Breakpoint {
  1: Addr addr,
  2: i64 count
}

enum EventType {
  Breakpoint,
  Timeout,
  Termination,
  RelevantBlock, /* A block is relevant, for instance because all blocks are relevant, or because it involves tainted data. */
  BeforeSystemCall,
  AfterSystemCall,
}

typedef set<EventType> EventTypes

exception Exception {
  1: string msg
}

service ConcreteEvaluator {

  void addBreakpoint (1: Breakpoint bp) throws (1: Exception exc)

  EventType executeUntilEvent(1: EventTypes stopEvents)

  /*
  void introduceTaintToMemory(1: Addr addr, 2: i32 numBytes = 1)

  // XXX: Need a way to represent all bytes
  void introduceTaintToRegister(1: RegisterID reg, 2: i32 numBytes = 8)

  byte getByteFromMemory(1: Addr addr)

  list<byte> getBytesFromMemory(1: Addr addr, 2: i32 numBytes = 1)
  */
}