// Copyright 2019 Saleem Abdulrasool <compnerd@compnerd.org>

#ifndef singleton_hh
#define singleton_hh

template <typename T_>
class singleton {
protected:
  singleton() = default;

public:
  singleton(const singleton &) = delete;
  singleton & operator=(const singleton &) = delete;

  singleton(singleton &&) = delete;
  singleton & operator=(singleton &&) = delete;

  static T_ & instance() {
    static T_ instance;
    return instance;
  }
};

#endif
