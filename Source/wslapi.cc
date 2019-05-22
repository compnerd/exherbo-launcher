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
