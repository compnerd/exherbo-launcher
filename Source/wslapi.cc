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

  bind("WslIsDistributionRegistered", &pfnWslIsDistributionRegistered_);
  bind("WslRegisterDistribution", &pfnWslRegisterDistribution_);
  bind("WslLaunchInteractive", &pfnWslLaunchInteractive_);
  bind("WslLaunch", &pfnWslLaunch_);
  bind("WslConfigureDistribution", &pfnWslConfigureDistribution_);
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
