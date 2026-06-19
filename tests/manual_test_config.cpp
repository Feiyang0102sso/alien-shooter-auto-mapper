/**
 * @file manual_test_config.cpp
 * @brief Manual test file to verify basic components (config, logger, version) work properly.
 */
#include "auto_mapper/common/config.h"
#include "auto_mapper/common/logger.h"
#include "auto_mapper/common/version.h"
#include <iostream>

using namespace auto_mapper;

int main() {
    try {
        // 1. Initialize environment config at the very beginning
        // (This internally initializes the Logger and prints the current EXE and Log paths)
        Config::init_env();

        Logger::info("============= Auto Mapper v{} =============", VERSION);
        Logger::info("Current App Name: {}", APP_NAME);
        
        // 3. Test output colors and formatting for the four levels
        Logger::debug("This is a Debug level test log");
        Logger::info("This is an Info level test log");
        Logger::warning("This is a Warning level test log");
        Logger::error("This is an Error level test log");

    } catch (const std::exception& e) {
        std::cerr << "\n[Catch Block] An unknown standard exception occurred: " << e.what() << "\n";
        return 2;
    }

    return 0;
}
