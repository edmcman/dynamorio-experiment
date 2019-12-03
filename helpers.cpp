#include "gen-cpp/concolic_types.h"

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
