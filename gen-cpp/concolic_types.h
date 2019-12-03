/**
 * Autogenerated by Thrift Compiler (0.13.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef concolic_TYPES_H
#define concolic_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/TBase.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <functional>
#include <memory>




struct EventType {
  enum type {
    Breakpoint = 0,
    Timeout = 1,
    Termination = 2,
    RelevantBlock = 3,
    BeforeSystemCall = 4,
    AfterSystemCall = 5
  };
};

extern const std::map<int, const char*> _EventType_VALUES_TO_NAMES;

std::ostream& operator<<(std::ostream& out, const EventType::type& val);

std::string to_string(const EventType::type& val);

typedef std::string RegisterValue;

typedef std::string RegisterID;

typedef std::map<RegisterID, RegisterValue>  RegisterContext;

typedef int64_t AbsAddr;

typedef int64_t Offset;

typedef std::set<EventType::type>  EventTypes;

class RelAddr;

class AbsOrRelAddr;

class Breakpoint;

class ModuleInfo;

class CodeBlock;

class Exception;

typedef struct _RelAddr__isset {
  _RelAddr__isset() : modulename(false), offset(false) {}
  bool modulename :1;
  bool offset :1;
} _RelAddr__isset;

class RelAddr : public virtual ::apache::thrift::TBase {
 public:

  RelAddr(const RelAddr&);
  RelAddr& operator=(const RelAddr&);
  RelAddr() : modulename(), offset(0) {
  }

  virtual ~RelAddr() noexcept;
  std::string modulename;
  Offset offset;

  _RelAddr__isset __isset;

  void __set_modulename(const std::string& val);

  void __set_offset(const Offset val);

  bool operator == (const RelAddr & rhs) const
  {
    if (!(modulename == rhs.modulename))
      return false;
    if (!(offset == rhs.offset))
      return false;
    return true;
  }
  bool operator != (const RelAddr &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const RelAddr & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(RelAddr &a, RelAddr &b);

std::ostream& operator<<(std::ostream& out, const RelAddr& obj);

typedef struct _AbsOrRelAddr__isset {
  _AbsOrRelAddr__isset() : absaddr(false), reladdr(false) {}
  bool absaddr :1;
  bool reladdr :1;
} _AbsOrRelAddr__isset;

class AbsOrRelAddr : public virtual ::apache::thrift::TBase {
 public:

  AbsOrRelAddr(const AbsOrRelAddr&);
  AbsOrRelAddr& operator=(const AbsOrRelAddr&);
  AbsOrRelAddr() : absaddr(0) {
  }

  virtual ~AbsOrRelAddr() noexcept;
  AbsAddr absaddr;
  RelAddr reladdr;

  _AbsOrRelAddr__isset __isset;

  void __set_absaddr(const AbsAddr val);

  void __set_reladdr(const RelAddr& val);

  bool operator == (const AbsOrRelAddr & rhs) const
  {
    if (__isset.absaddr != rhs.__isset.absaddr)
      return false;
    else if (__isset.absaddr && !(absaddr == rhs.absaddr))
      return false;
    if (__isset.reladdr != rhs.__isset.reladdr)
      return false;
    else if (__isset.reladdr && !(reladdr == rhs.reladdr))
      return false;
    return true;
  }
  bool operator != (const AbsOrRelAddr &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const AbsOrRelAddr & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(AbsOrRelAddr &a, AbsOrRelAddr &b);

std::ostream& operator<<(std::ostream& out, const AbsOrRelAddr& obj);

typedef struct _Breakpoint__isset {
  _Breakpoint__isset() : addr(false), count(false) {}
  bool addr :1;
  bool count :1;
} _Breakpoint__isset;

class Breakpoint : public virtual ::apache::thrift::TBase {
 public:

  Breakpoint(const Breakpoint&);
  Breakpoint& operator=(const Breakpoint&);
  Breakpoint() : count(0) {
  }

  virtual ~Breakpoint() noexcept;
  AbsOrRelAddr addr;
  int64_t count;

  _Breakpoint__isset __isset;

  void __set_addr(const AbsOrRelAddr& val);

  void __set_count(const int64_t val);

  bool operator == (const Breakpoint & rhs) const
  {
    if (!(addr == rhs.addr))
      return false;
    if (!(count == rhs.count))
      return false;
    return true;
  }
  bool operator != (const Breakpoint &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Breakpoint & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(Breakpoint &a, Breakpoint &b);

std::ostream& operator<<(std::ostream& out, const Breakpoint& obj);

typedef struct _ModuleInfo__isset {
  _ModuleInfo__isset() : addr(false), base(false) {}
  bool addr :1;
  bool base :1;
} _ModuleInfo__isset;

class ModuleInfo : public virtual ::apache::thrift::TBase {
 public:

  ModuleInfo(const ModuleInfo&);
  ModuleInfo& operator=(const ModuleInfo&);
  ModuleInfo() : base(0) {
  }

  virtual ~ModuleInfo() noexcept;
  RelAddr addr;
  AbsAddr base;

  _ModuleInfo__isset __isset;

  void __set_addr(const RelAddr& val);

  void __set_base(const AbsAddr val);

  bool operator == (const ModuleInfo & rhs) const
  {
    if (!(addr == rhs.addr))
      return false;
    if (!(base == rhs.base))
      return false;
    return true;
  }
  bool operator != (const ModuleInfo &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ModuleInfo & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(ModuleInfo &a, ModuleInfo &b);

std::ostream& operator<<(std::ostream& out, const ModuleInfo& obj);

typedef struct _CodeBlock__isset {
  _CodeBlock__isset() : addr(false), bytes(false), moduleinfo(false) {}
  bool addr :1;
  bool bytes :1;
  bool moduleinfo :1;
} _CodeBlock__isset;

class CodeBlock : public virtual ::apache::thrift::TBase {
 public:

  CodeBlock(const CodeBlock&);
  CodeBlock& operator=(const CodeBlock&);
  CodeBlock() : addr(0), bytes() {
  }

  virtual ~CodeBlock() noexcept;
  AbsAddr addr;
  std::string bytes;
  ModuleInfo moduleinfo;

  _CodeBlock__isset __isset;

  void __set_addr(const AbsAddr val);

  void __set_bytes(const std::string& val);

  void __set_moduleinfo(const ModuleInfo& val);

  bool operator == (const CodeBlock & rhs) const
  {
    if (!(addr == rhs.addr))
      return false;
    if (!(bytes == rhs.bytes))
      return false;
    if (__isset.moduleinfo != rhs.__isset.moduleinfo)
      return false;
    else if (__isset.moduleinfo && !(moduleinfo == rhs.moduleinfo))
      return false;
    return true;
  }
  bool operator != (const CodeBlock &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const CodeBlock & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(CodeBlock &a, CodeBlock &b);

std::ostream& operator<<(std::ostream& out, const CodeBlock& obj);

typedef struct _Exception__isset {
  _Exception__isset() : msg(false) {}
  bool msg :1;
} _Exception__isset;

class Exception : public ::apache::thrift::TException {
 public:

  Exception(const Exception&);
  Exception& operator=(const Exception&);
  Exception() : msg() {
  }

  virtual ~Exception() noexcept;
  std::string msg;

  _Exception__isset __isset;

  void __set_msg(const std::string& val);

  bool operator == (const Exception & rhs) const
  {
    if (!(msg == rhs.msg))
      return false;
    return true;
  }
  bool operator != (const Exception &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Exception & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
  mutable std::string thriftTExceptionMessageHolder_;
  const char* what() const noexcept;
};

void swap(Exception &a, Exception &b);

std::ostream& operator<<(std::ostream& out, const Exception& obj);



#endif
