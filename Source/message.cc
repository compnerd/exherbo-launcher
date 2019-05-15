// Copyright 2019 Saleem Abdulrasool <compnerd@compnerd.org>

#include "message.hh"

#include <string>

namespace message {
std::wstring format(const id id) noexcept {
  PWSTR buffer;
  std::wstring string;
  if (::FormatMessageW(FORMAT_MESSAGE_FROM_HMODULE |
                           FORMAT_MESSAGE_ALLOCATE_BUFFER,
                       nullptr, static_cast<DWORD>(id), 0,
                       reinterpret_cast<PWSTR>(&buffer), 0, nullptr)) {
    string = buffer;
    ::HeapFree(GetProcessHeap(), 0, buffer);
  }
  return string;
}

std::wstring format(const HRESULT hr) noexcept {
  PWSTR buffer;
  std::wstring string;
  if (::FormatMessageW(
          FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr,
          hr, 0, reinterpret_cast<PWSTR>(&buffer), 0, nullptr)) {
    string = buffer;
    ::HeapFree(GetProcessHeap(), 0, buffer);
  }
  return string;
}
} // namespace message
