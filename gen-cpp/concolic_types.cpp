/**
 * Autogenerated by Thrift Compiler (0.13.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "concolic_types.h"

#include <algorithm>
#include <ostream>

#include <thrift/TToString.h>



int _kEventTypeValues[] = {
  EventType::Breakpoint,
  EventType::Timeout,
  EventType::Termination,
  EventType::RelevantBlock,
  EventType::BeforeSystemCall,
  EventType::AfterSystemCall
};
const char* _kEventTypeNames[] = {
  "Breakpoint",
  "Timeout",
  "Termination",
  "RelevantBlock",
  "BeforeSystemCall",
  "AfterSystemCall"
};
const std::map<int, const char*> _EventType_VALUES_TO_NAMES(::apache::thrift::TEnumIterator(6, _kEventTypeValues, _kEventTypeNames), ::apache::thrift::TEnumIterator(-1, NULL, NULL));

std::ostream& operator<<(std::ostream& out, const EventType::type& val) {
  std::map<int, const char*>::const_iterator it = _EventType_VALUES_TO_NAMES.find(val);
  if (it != _EventType_VALUES_TO_NAMES.end()) {
    out << it->second;
  } else {
    out << static_cast<int>(val);
  }
  return out;
}

std::string to_string(const EventType::type& val) {
  std::map<int, const char*>::const_iterator it = _EventType_VALUES_TO_NAMES.find(val);
  if (it != _EventType_VALUES_TO_NAMES.end()) {
    return std::string(it->second);
  } else {
    return std::to_string(static_cast<int>(val));
  }
}


Breakpoint::~Breakpoint() noexcept {
}


void Breakpoint::__set_addr(const Addr val) {
  this->addr = val;
}

void Breakpoint::__set_count(const int64_t val) {
  this->count = val;
}
std::ostream& operator<<(std::ostream& out, const Breakpoint& obj)
{
  obj.printTo(out);
  return out;
}


uint32_t Breakpoint::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->addr);
          this->__isset.addr = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->count);
          this->__isset.count = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t Breakpoint::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("Breakpoint");

  xfer += oprot->writeFieldBegin("addr", ::apache::thrift::protocol::T_I64, 1);
  xfer += oprot->writeI64(this->addr);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("count", ::apache::thrift::protocol::T_I64, 2);
  xfer += oprot->writeI64(this->count);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(Breakpoint &a, Breakpoint &b) {
  using ::std::swap;
  swap(a.addr, b.addr);
  swap(a.count, b.count);
  swap(a.__isset, b.__isset);
}

Breakpoint::Breakpoint(const Breakpoint& other0) {
  addr = other0.addr;
  count = other0.count;
  __isset = other0.__isset;
}
Breakpoint& Breakpoint::operator=(const Breakpoint& other1) {
  addr = other1.addr;
  count = other1.count;
  __isset = other1.__isset;
  return *this;
}
void Breakpoint::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "Breakpoint(";
  out << "addr=" << to_string(addr);
  out << ", " << "count=" << to_string(count);
  out << ")";
}


CodeBlock::~CodeBlock() noexcept {
}


void CodeBlock::__set_addr(const Addr val) {
  this->addr = val;
}

void CodeBlock::__set_bytes(const std::string& val) {
  this->bytes = val;
}
std::ostream& operator<<(std::ostream& out, const CodeBlock& obj)
{
  obj.printTo(out);
  return out;
}


uint32_t CodeBlock::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->addr);
          this->__isset.addr = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->bytes);
          this->__isset.bytes = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t CodeBlock::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("CodeBlock");

  xfer += oprot->writeFieldBegin("addr", ::apache::thrift::protocol::T_I64, 1);
  xfer += oprot->writeI64(this->addr);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("bytes", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeBinary(this->bytes);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(CodeBlock &a, CodeBlock &b) {
  using ::std::swap;
  swap(a.addr, b.addr);
  swap(a.bytes, b.bytes);
  swap(a.__isset, b.__isset);
}

CodeBlock::CodeBlock(const CodeBlock& other2) {
  addr = other2.addr;
  bytes = other2.bytes;
  __isset = other2.__isset;
}
CodeBlock& CodeBlock::operator=(const CodeBlock& other3) {
  addr = other3.addr;
  bytes = other3.bytes;
  __isset = other3.__isset;
  return *this;
}
void CodeBlock::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "CodeBlock(";
  out << "addr=" << to_string(addr);
  out << ", " << "bytes=" << to_string(bytes);
  out << ")";
}


Exception::~Exception() noexcept {
}


void Exception::__set_msg(const std::string& val) {
  this->msg = val;
}
std::ostream& operator<<(std::ostream& out, const Exception& obj)
{
  obj.printTo(out);
  return out;
}


uint32_t Exception::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->msg);
          this->__isset.msg = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t Exception::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("Exception");

  xfer += oprot->writeFieldBegin("msg", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(this->msg);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(Exception &a, Exception &b) {
  using ::std::swap;
  swap(a.msg, b.msg);
  swap(a.__isset, b.__isset);
}

Exception::Exception(const Exception& other4) : TException() {
  msg = other4.msg;
  __isset = other4.__isset;
}
Exception& Exception::operator=(const Exception& other5) {
  msg = other5.msg;
  __isset = other5.__isset;
  return *this;
}
void Exception::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "Exception(";
  out << "msg=" << to_string(msg);
  out << ")";
}

const char* Exception::what() const noexcept {
  try {
    std::stringstream ss;
    ss << "TException - service has thrown: " << *this;
    this->thriftTExceptionMessageHolder_ = ss.str();
    return this->thriftTExceptionMessageHolder_.c_str();
  } catch (const std::exception&) {
    return "TException - service has thrown: Exception";
  }
}


