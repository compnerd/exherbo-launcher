// Copyright 2019 Saleem Abdulrasool <compnerd@compnerd.org>

#include "distribution.hh"
#include "message.hh"
#include "messages.h"
#include "utility.hh"
#include "wslapi.hh"

#include <iostream>

namespace lxss {
using uid_t = ULONG;

std::error_code useraddw(const std::wstring &username) {
  wsl::api &wsl = wsl::api::instance();

  HRESULT hr;
  DWORD dwExitCode;

  std::wstring command =
      L"/usr/bin/useradd -m -G adm,disk,wheel,cdrom,audio,video,usb,users " +
      username;

  hr = wsl.WslLaunchInteractive(distribution::name, command.c_str(), true,
                                &dwExitCode);
  if (FAILED(hr)) {
    DWORD dwErrorCode;
    if (!WIN32_FROM_HRESULT(hr, &dwErrorCode))
      return std::error_code(-1, std::generic_category());
    return std::error_code(dwErrorCode, std::system_category());
  }
  if (dwExitCode)
    return std::error_code(dwExitCode, std::generic_category());

  return std::error_code();
}

std::error_code getuidbynamew(const std::wstring &username, uid_t *uid) {
  wsl::api &wsl = wsl::api::instance();

  char buffer[64];
  scoped_handle hRead;
  scoped_handle hWrite;
  scoped_handle hChild;
  SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, true};
  std::wstring command = L"/usr/bin/id -u " + username;
  DWORD dwExitCode;
  DWORD dwBytesRead;
  HRESULT hr;

  if (!CreatePipe(&hRead, &hWrite, &sa, 0))
    return std::error_code(GetLastError(), std::system_category());

  hr = wsl.WslLaunch(distribution::name, command.c_str(), true,
                     GetStdHandle(STD_INPUT_HANDLE), hWrite,
                     GetStdHandle(STD_ERROR_HANDLE), &hChild);
  if (FAILED(hr)) {
    DWORD dwErrorCode;
    if (!WIN32_FROM_HRESULT(hr, &dwErrorCode))
      return std::error_code(-1, std::generic_category());
    return std::error_code(dwErrorCode, std::system_category());
  }

  WaitForSingleObject(hChild, INFINITE);
  if (!GetExitCodeProcess(hChild, &dwExitCode))
    return std::error_code(-1, std::generic_category());
  if (dwExitCode)
    return std::error_code(dwExitCode, std::generic_category());

  if (!ReadFile(hRead, buffer, sizeof(buffer) - 1, &dwBytesRead, nullptr))
    return std::error_code(GetLastError(), std::system_category());

  buffer[dwBytesRead] = '\0';
  try {
    *uid = std::stoul(buffer, nullptr, 10);
  } catch (...) {
    return std::error_code(-1, std::generic_category());
  }
  return std::error_code();
}
}

namespace distribution {
std::error_code set_default_user(const std::wstring &username) {
  wsl::api &wsl = wsl::api::instance();

  HRESULT hr;
  lxss::uid_t uid;

  // Query the UID of the given user name and configure the distribution to use
  // this UID as the default.

  if (auto error = lxss::getuidbynamew(username, &uid))
    return error;

  hr = wsl.WslConfigureDistribution(distribution::name, uid,
                                    WSL_DISTRIBUTION_FLAGS_DEFAULT);
  if (FAILED(hr)) {
    DWORD dwErrorCode;
    if (!WIN32_FROM_HRESULT(hr, &dwErrorCode))
      return std::error_code(-1, std::generic_category());
    return std::error_code(dwErrorCode, std::system_category());
  }

  return std::error_code();
}

HRESULT install(const wchar_t *image, bool create_user) {
  wsl::api &wsl = wsl::api::instance();

  HRESULT hr;
  DWORD dwExitCode;
  std::wstring username;

  std::wcout << message::format(message::id(MSG_STATUS_INSTALLING));
  hr = wsl.WslRegisterDistribution(distribution::name, image);
  if (FAILED(hr))
    return hr;

  // Delete /etc/resolv.conf to permit WSL to generate a version based upon
  // Windows networking information.
  hr = wsl.WslLaunchInteractive(distribution::name, L"/bin/rm /etc/resolv.conf",
                                TRUE, &dwExitCode);
  if (FAILED(hr))
    return hr;

  if (!create_user)
    return hr;

  std::wcout<< message::format(message::id(MSG_CREATE_USER_PROMPT));
  do {
    std::wcout << message::format(message::id(MSG_ENTER_USERNAME));
    std::wcin >> username;
  } while (lxss::useraddw(username));

  // Set the new user as the default user
  if (set_default_user(username))
    return E_FAIL;

  return hr;
}
} // namespace distribution
