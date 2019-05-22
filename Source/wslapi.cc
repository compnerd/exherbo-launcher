// Copyright 2019 Saleem Abdulrasool <compnerd@compnerd.org>

#include "wslapi.hh"

namespace wsl {
api::api() : hModule_(nullptr) {
  // NOTE(compnerd) use LOAD_LIBRARY_SEARCH_SYSTEM32 to avoid the normal search,
  // this is a system library.
  hModule_ =
      ::LoadLibraryExW(L"wslapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
  if (!hModule_)
    return;
  pfnWslIsDistributionRegistered_ =
      reinterpret_cast<decltype(pfnWslIsDistributionRegistered_)>(
          ::GetProcAddress(hModule_, "WslIsDistributionRegistered"));
  pfnWslRegisterDistribution_ =
      reinterpret_cast<decltype(pfnWslRegisterDistribution_)>(
          ::GetProcAddress(hModule_, "WslRegisterDistribution"));
  pfnWslLaunchInteractive_ =
      reinterpret_cast<decltype(pfnWslLaunchInteractive_)>(
          ::GetProcAddress(hModule_, "WslLaunchInteractive"));
  pfnWslLaunch_ = reinterpret_cast<decltype(pfnWslLaunch_)>(
      ::GetProcAddress(hModule_, "WslLaunch"));
  pfnWslConfigureDistribution_ =
      reinterpret_cast<decltype(pfnWslConfigureDistribution_)>(
          ::GetProcAddress(hModule_, "WslConfigureDistribution"));
}

api::~api() {
  if (hModule_)
    ::FreeLibrary(hModule_);
}

bool api::supports_required_interfaces() const {
  if (!pfnWslIsDistributionRegistered_)
    return false;
  if (!pfnWslRegisterDistribution_)
    return false;
  if (!pfnWslLaunchInteractive_)
    return false;
  if (!pfnWslLaunch_)
    return false;
  if (!pfnWslConfigureDistribution_)
    return false;
  return true;
}
} // namespace wsl
