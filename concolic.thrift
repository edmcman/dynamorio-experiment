typedef binary RegisterValue
typedef string RegisterID

typedef map<RegisterID,RegisterValue> RegisterContext

// Absolute address
typedef i64 AbsAddr

// Relative address inside a module
struct RelAddr {
  1: string modulename,
  2: Offset offset
}

typedef i64 Offset

// Absolute or relative address.  At least one must be set.
struct AbsOrRelAddr {
  1: optional AbsAddr absaddr,
  2: optional RelAddr reladdr
}

struct Breakpoint {
  1: AbsOrRelAddr addr,
  2: i64 count
}

struct ModuleInfo {
  1: RelAddr addr,
  2: AbsAddr base
}

struct CodeBlock {
  1: AbsAddr addr,
  2: binary bytes,
  3: optional ModuleInfo moduleinfo
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

  CodeBlock getCodeBlock () throws (1: Exception exc)

  // XXX: Specify or return thread id?
  RegisterContext getContext () throws (1: Exception exc)

  /*
  void introduceTaintToMemory(1: Addr addr, 2: i32 numBytes = 1)

  // XXX: Need a way to represent all bytes
  void introduceTaintToRegister(1: RegisterID reg, 2: i32 numBytes = 8)

  byte getByteFromMemory(1: Addr addr)

  binary getBytesFromMemory(1: Addr addr, 2: i32 numBytes = 1)
  */
}