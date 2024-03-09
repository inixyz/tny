#pragma once

// C headers
#include <cstdlib>

// C++ headers
#include <string>

namespace err {

typedef int Code;

std::string time_str();

void fatal(const std::string& message, const int exit_code = EXIT_FAILURE);

void warn(const std::string& message);

} // namespace err