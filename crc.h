// -*- mode: c++ -*-
#include <cstddef>
#include <iostream>

namespace crc {
  const std::size_t LOG_LIMIT = 16384;
  const std::size_t UNLOGGED = LOG_LIMIT * 2;

  struct erased_storage {
  public:
    std::size_t count = 0;
    virtual void increment() = 0;
    // Return if the object is dead.
    virtual bool decrement() = 0;
    virtual ~erased_storage() { }
  };

  template<typename T>
  struct crc_storage : public erased_storage {
  public:
    crc_storage(const T& d) : data(d) { }
    T data;
    void increment() { count++; }
    bool decrement() { return !count || --count == 0; }
    ~crc_storage() { data.~T(); }
  };

  struct erased_crc {
  public:
    std::size_t logged = UNLOGGED;
    virtual erased_storage *storage() = 0;
  };

  extern bool collecting;
  extern void collect();
  extern void add_to_zct(erased_storage *s);
  extern std::size_t add_to_log(erased_crc *p, erased_storage *s);
  extern void maybe_collect();
  extern void collect();
  extern void unlog_dead_ptr(std::size_t index);

  template<typename T>
  struct cooler_shared_ptr : public erased_crc {
  public:
    cooler_shared_ptr(const T& d) : erased_crc() {
      write_barrier(new crc_storage<T>(d));
      add_to_zct(ref);
    }
    cooler_shared_ptr(const cooler_shared_ptr<T>& p) {
      write_barrier(p.storage());
    }
    cooler_shared_ptr<T>& operator=(const cooler_shared_ptr<T>& other) {
      if (this == &other) return *this;
      cooler_shared_ptr<T>& o = const_cast<cooler_shared_ptr<T>&>(other);
      write_barrier(o.ref);
      return *this;
    }
    ~cooler_shared_ptr() {
      if (collecting && ref->decrement()) add_to_zct(ref);
      if (!collecting) {
        if (this->logged != UNLOGGED)
          unlog_dead_ptr(this->logged);
        else
          add_to_log(nullptr, ref);
      }
    }
    T& operator*() { return &ref->data; }
    T* operator->() { return &ref->data;; }
  
    T *get() { return &ref->data; }
    erased_storage *storage() { return ref; }
  
  private:
    void write_barrier(crc_storage<T> *n) {
      if (this->logged == UNLOGGED) {
        this->logged = add_to_log(this, ref);
        ref = n;
        maybe_collect();
      } else {
        ref = n;
      }
    }
    crc_storage<T> *ref = nullptr;
  };
};
