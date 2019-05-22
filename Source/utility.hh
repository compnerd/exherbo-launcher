/*
 * Copyright © 2019 Saleem Abdulrasool <compnerd@compnerd.org>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
