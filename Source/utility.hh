// Copyright 2019 Saleem Abdulrasool <compnerd@compnerd.org>

#ifndef utility_hh
#define utility_hh

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class scoped_handle final {
  HANDLE hHandle_;

public:
  scoped_handle() noexcept : hHandle_(INVALID_HANDLE_VALUE) {}
  scoped_handle(HANDLE hHandle) noexcept : hHandle_(hHandle) {}

  scoped_handle(const scoped_handle &) = delete;
  scoped_handle(const scoped_handle &&) = delete;

  scoped_handle &operator=(const scoped_handle &) = delete;
  scoped_handle &operator=(const scoped_handle &&) = delete;

  HANDLE *operator&() noexcept { return &hHandle_; }

  operator HANDLE() noexcept { return hHandle_; }

  ~scoped_handle() noexcept {
    if (hHandle_ != INVALID_HANDLE_VALUE)
      ::CloseHandle(hHandle_);
  }

  HANDLE get() const noexcept { return hHandle_; }

  void reset(HANDLE hHandle) noexcept {
    if (hHandle_ != INVALID_HANDLE_VALUE)
      ::CloseHandle(hHandle_);
    hHandle_ = hHandle;
  }
};

BOOL WIN32_FROM_HRESULT(HRESULT hr, DWORD *pdwErrorCode) {
  if ((hr & 0xffff0000) == MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0)) {
    *pdwErrorCode = HRESULT_CODE(hr);
    return TRUE;
  }

  if (hr == S_OK) {
    *pdwErrorCode = HRESULT_CODE(hr);
    return TRUE;
  }

  return FALSE;
}

#endif
