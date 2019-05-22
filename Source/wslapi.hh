// Copyright 2019 Saleem Abdulrasool <compnerd@compnerd.org>

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
