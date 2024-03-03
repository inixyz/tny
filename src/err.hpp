#pragma once

#include <cstdlib>
#include <ctime>

#include <string>
#include <chrono>
#include <algorithm>
#include <iostream>

namespace err {

typedef int Code;

////////////////////////////////////////////////////////////////////////////////

std::string time_str() {
    const auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::string out = std::ctime(&now_c);
    std::erase(out, '\n');

    return out;
}

////////////////////////////////////////////////////////////////////////////////

void fatal(const std::string& message, const int exit_code = EXIT_FAILURE) {
    std::cerr << "[ERROR] [" << time_str() << "] " << message << std::endl;
    std::exit(exit_code);
}

////////////////////////////////////////////////////////////////////////////////

void warn(const std::string& message) {
    std::cerr << "[WARNING] [" << time_str() << "] " << message << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

} // namespace er