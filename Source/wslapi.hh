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

#ifndef wslapi_hh
#define wslapi_hh

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wslapi.h>

#include <cassert>

#include "singleton.hh"

namespace wsl {
class api : public singleton<wsl::api> {
  friend class singleton<wsl::api>;

  HMODULE hModule_;
  BOOL (*pfnWslIsDistributionRegistered_)(PCWSTR);
  HRESULT (*pfnWslRegisterDistribution_)(PCWSTR, PCWSTR);
  HRESULT (*pfnWslLaunchInteractive_)(PCWSTR, PCWSTR, BOOL, DWORD *);
  HRESULT (*pfnWslLaunch_)(PCWSTR, PCWSTR, BOOL, HANDLE, HANDLE, HANDLE, HANDLE *);
  HRESULT (*pfnWslConfigureDistribution_)(PCWSTR, ULONG, WSL_DISTRIBUTION_FLAGS);

  template <typename ResultType_, typename ...Arguments_>
  void bind(const char *name, ResultType_ (**pfn)(Arguments_...)) {
    *pfn = reinterpret_cast<decltype(*pfn)>(::GetProcAddress(hModule_, name));
  }

  api();
  ~api();

public:
  bool installed() const { return hModule_; }
  bool supports_required_interfaces() const;

  BOOL WslIsDistributionRegistered(PCWSTR distributionName) const {
    assert(pfnWslIsDistributionRegistered_);
    return (*pfnWslIsDistributionRegistered_)(distributionName);
  }

  HRESULT WslRegisterDistribution(PCWSTR distributionName,
                                  PCWSTR tarGzFilename) const {
    assert(pfnWslRegisterDistribution_);
    return (*pfnWslRegisterDistribution_)(distributionName, tarGzFilename);
  }

  HRESULT WslLaunchInteractive(PCWSTR distributionName, PCWSTR command,
                               BOOL useCurrentWorkingDirectory,
                               DWORD *exitCode) const {
    assert(pfnWslLaunchInteractive_);
    return (*pfnWslLaunchInteractive_)(distributionName, command,
                                       useCurrentWorkingDirectory, exitCode);
  }

  HRESULT WslLaunch(PCWSTR distributionName, PCWSTR command,
                    BOOL useCurrentWorkingDirectory, HANDLE stdIn,
                    HANDLE stdOut, HANDLE stdErr, HANDLE *process) const {
    assert(pfnWslLaunch_);
    return (*pfnWslLaunch_)(distributionName, command,
                            useCurrentWorkingDirectory, stdIn, stdOut, stdErr,
                            process);
  }

  HRESULT
  WslConfigureDistribution(PCWSTR distributionName, ULONG defaultUID,
                           WSL_DISTRIBUTION_FLAGS wslDistributionFlags) const {
    assert(pfnWslConfigureDistribution_);
    return (*pfnWslConfigureDistribution_)(distributionName, defaultUID,
                                           wslDistributionFlags);
  }
};
} // namespace wsl

#endif
