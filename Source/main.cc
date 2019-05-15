// Copyright 2019 Saleem Abdulrasool <compnerd@compnerd.org>

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
  std::vector<std::wstring_view> arguments{&argv[1], &argv[argc]};
  wsl::api wslapi;
  HRESULT hr;

  SetConsoleTitleW(distribution::name);

  // Ensure that WSL is available and new enough
  if (!wslapi.installed()) {
    std::wcout << message::format(message::id(MSG_MISSING_OPTIONAL_COMPONENT));
    if (arguments.empty()) {
      std::wcout << message::format(message::id(MSG_PRESS_A_KEY));
      _getwch();
    }
    return EXIT_FAILURE;
  }
  if (!wslapi.supports_required_interfaces()) {
    std::wcout << message::format(message::id(MSG_OPTIONAL_COMPONENT_TOO_OLD));
    if (arguments.empty()) {
      std::wcout << message::format(message::id(MSG_PRESS_A_KEY));
      _getwch();
    }
    return EXIT_FAILURE;
  }

  // Handle `--install`
  bool bInstall = (arguments.size() > 0) && arguments[0] == kInstall;
  if (!wslapi.WslIsDistributionRegistered(distribution::name)) {
    // If `--root` is specified, do not create a user
    bool bRoot = bInstall && (arguments.size() > 1) && arguments[1] == kRoot;
    HRESULT hr = distribution::install(wslapi, L"image.tar.gz", !bRoot);
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
    hr = wslapi.WslLaunchInteractive(distribution::name, L"", false,
                                     &dwExitCode);
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
    hr = wslapi.WslLaunchInteractive(distribution::name, command.c_str(), true,
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
        if (distribution::wsl_set_default_user(wslapi,
                                               std::wstring(arguments[2])))
          return EXIT_FAILURE;
        return EXIT_SUCCESS;
      }
    }
  }

  std::wcout << message::format(message::id(MSG_USAGE));
  return EXIT_SUCCESS;
}
