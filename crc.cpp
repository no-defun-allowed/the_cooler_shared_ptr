#include <utility>
#include <unordered_set>
#include <vector>
#include "crc.h"

namespace crc {
  bool collecting = false;
  std::size_t collections = 0;
  
  // Garbage collector

  std::vector<erased_storage*> zero_count_table;
  void add_to_zct(erased_storage *s) { zero_count_table.push_back(s); }

  std::vector<std::pair<erased_crc*, erased_storage*> > logged_writes;
  void maybe_collect() { if (logged_writes.size() >= LOG_LIMIT) collect(); }
  std::size_t add_to_log(erased_crc *p, erased_storage *s) {
    std::size_t last = logged_writes.size();
    logged_writes.emplace_back(p, s);
    return last;
  }

  // Clear the log to ensure the GC doesn't follow a dangling pointer to
  // what was a root.
  void unlog_dead_ptr(std::size_t index) {
    // Try to reuse log space if this will become <nullptr, nullptr>.
    if (index + 1 == logged_writes.size() && !logged_writes[index].second) {
      logged_writes.pop_back();
    } else {
      logged_writes[index].first = nullptr;
    }
  }

  void collect() {
    std::vector<erased_storage*> zct(zero_count_table);
    zero_count_table.clear();
    collecting = true;
    collections++;

    // Process increments
    for (auto &e : logged_writes)
      if (e.first) {
        e.first->storage()->increment();
        e.first->logged = UNLOGGED;
      }

    // Process decrements
    for (auto &e : logged_writes) {
      if (e.second && e.second->decrement()) zct.push_back(e.second);
    }

    // Sweep zero count table
    while (true) {
      for (auto &e : zct) if (e->count == 0) delete e;
      if (zero_count_table.size() == 0) break;
      zct = zero_count_table;
      zero_count_table.clear();
    }

    logged_writes.clear();
    collecting = false;
  }

};
