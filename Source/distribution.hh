// Copyright 2019 Saleem Abdulrasool <compnerd@compnerd.org>

#ifndef distribution_hh
#define distribution_hh

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>
#include <system_error>

namespace wsl {
class api;
}

namespace distribution {
static const constexpr wchar_t *name = L"exherbo";
HRESULT install(const wsl::api &wslapi, const wchar_t *image, bool create_user);
std::error_code wsl_set_default_user(const wsl::api &wslapi,
                                     const std::wstring &username);
} // namespace distribution

#endif
