/**
 * @file manual_test.cpp
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

        // 4. Test error_and_stop (currently not forcefully stopping)
        Logger::error_and_stop("This error won't block the program because stop_on_error is not enabled.");

        // 5. Enable force stop mode and trigger error_and_stop again
        Logger::set_stop_on_error(true);
        Logger::info("---- Preparing to test force stop mode (throwing exception) ----");
        Logger::error_and_stop("Fatal Error: Failed to write .map file, aborting all subsequent processes!");

        // Because an exception is thrown above, this line will not be executed
        Logger::info("If you see this message, the blocking mechanism failed!");

    } catch (const FatalError& e) {
        // Since we already printed the red raw error to std::cerr in error_and_stop
        // we just use a normal output here to prove the exception was successfully thrown and caught
        std::cout << "\n[Catch Block] Successfully caught the fatal exception thrown by the program: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "\n[Catch Block] An unknown standard exception occurred: " << e.what() << "\n";
        return 2;
    }

    return 0;
}
