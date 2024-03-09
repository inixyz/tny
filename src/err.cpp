#include "err.hpp"

// C headers
#include <cstdlib>
#include <ctime>

// C++ headers
#include <string>
#include <chrono>
#include <algorithm>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Returns the current time as a string.
*/
std::string err::time_str() {
    const auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::string out = std::ctime(&now_c);
    out.erase(std::remove(out.begin(), out.end(), '\n'), out.cend());

    return out;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Logs a message to the console when the program fails.
*/
void err::fatal(const std::string& message, const int exit_code) {
    std::cerr << "[ERROR] [" << time_str() << "] " << message << std::endl;
    exit(exit_code);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Logs a warning message to the console.
*/
void err::warn(const std::string& message) {
    std::cerr << "[WARNING] [" << time_str() << "] " << message << std::endl;
}

////////////////////////////////////////////////////////////////////////////////