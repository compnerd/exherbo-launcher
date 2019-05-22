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

#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "distribution.hh"
#include "message.hh"
#include "messages.h"
#include "wslapi.hh"

namespace {
static const wchar_t *kInstall = L"--install";
static const wchar_t *kRoot = L"--root";
static const wchar_t *kDefaultUser = L"--default-user";

static const wchar_t *kRun = L"run";
static const wchar_t *kConfig = L"config";
static const wchar_t *kShortRun = L"-c";
} // namespace

int wmain(int argc, wchar_t *argv[]) {
  wsl::api &wsl = wsl::api::instance();

  std::vector<std::wstring_view> arguments{&argv[1], &argv[argc]};
  HRESULT hr;

  SetConsoleTitleW(distribution::name);

  // Ensure that WSL is available and new enough
  if (!wsl.installed()) {
    std::wcout << message::format(message::id(MSG_MISSING_OPTIONAL_COMPONENT));
    if (arguments.empty()) {
      std::wcout << message::format(message::id(MSG_PRESS_A_KEY));
      _getwch();
    }
    return EXIT_FAILURE;
  }
  if (!wsl.supports_required_interfaces()) {
    std::wcout << message::format(message::id(MSG_OPTIONAL_COMPONENT_TOO_OLD));
    if (arguments.empty()) {
      std::wcout << message::format(message::id(MSG_PRESS_A_KEY));
      _getwch();
    }
    return EXIT_FAILURE;
  }

  // Handle `--install`
  bool bInstall = (arguments.size() > 0) && arguments[0] == kInstall;
  if (!wsl.WslIsDistributionRegistered(distribution::name)) {
    // If `--root` is specified, do not create a user
    bool bRoot = bInstall && (arguments.size() > 1) && arguments[1] == kRoot;
    HRESULT hr = distribution::install(L"image.tar.gz", !bRoot);
    if (FAILED(hr)) {
      std::wcout << message::format(hr);
      return hr;
    }
    std::wcout << message::format(message::id(MSG_INSTALL_SUCCESS));
  }
  if (bInstall)
    return EXIT_SUCCESS;

  if (arguments.empty()) {
    DWORD dwExitCode;
    hr = wsl.WslLaunchInteractive(distribution::name, L"", false, &dwExitCode);
    if (FAILED(hr)) {
      std::wcout << hr;
      return hr;
    }

    return dwExitCode == 0;
  }

  if (arguments[0] == kRun || arguments[0] == kShortRun) {
    DWORD dwExitCode;
    std::wostringstream woss;
    std::copy(std::begin(arguments), std::end(arguments),
              std::ostream_iterator<std::wstring_view, wchar_t>(woss, L" "));
    std::wstring command = woss.str();
    hr = wsl.WslLaunchInteractive(distribution::name, command.c_str(), true,
                                  &dwExitCode);
    if (FAILED(hr)) {
      std::wcout << hr;
      return hr;
    }

    return dwExitCode == 0;
  }

  if (arguments[0] == kConfig) {
    hr = E_INVALIDARG;
    if (arguments.size() == 3) {
      if (arguments[1] == kDefaultUser) {
        if (distribution::set_default_user(std::wstring(arguments[2])))
          return EXIT_FAILURE;
        return EXIT_SUCCESS;
      }
    }
  }

  std::wcout << message::format(message::id(MSG_USAGE));
  return EXIT_SUCCESS;
}
