// Copyright 2019 Saleem Abdulrasool <compnerd@compnerd.org>

#ifndef message_hh
#define message_hh

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace message {
class id {
  DWORD id_;
public:
  explicit id(const DWORD dwID) noexcept : id_(dwID) {}
  operator DWORD() const noexcept { return id_; }
};
std::wstring format(const id id) noexcept;
std::wstring format(const HRESULT hr) noexcept;
}; // namespace message

#endif

