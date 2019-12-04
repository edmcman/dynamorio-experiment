#include "gen-cpp/concolic_types.h"

bool AbsOrRelAddr::operator< (const AbsOrRelAddr &a) const {
  if (absaddr != a.absaddr) return absaddr < a.absaddr;
  if (reladdr != a.reladdr) return reladdr < a.reladdr;
  return false;
}

bool RelAddr::operator< (const RelAddr &r) const {
  if (modulename != r.modulename) return modulename < r.modulename;
  if (offset != r.offset) return offset < r.offset;
  return false;
}

bool OffsetOrSymbol::operator< (const OffsetOrSymbol &o) const {
  if (offset != o.offset) return offset < o.offset;
  if (symbol != o.symbol) return symbol < o.symbol;
  return false;
}

bool Breakpoint::operator< (const Breakpoint &b) const {
  if (addr != b.addr) return addr < b.addr;
  if (count != b.count) return count < b.count;
  return false;
}
