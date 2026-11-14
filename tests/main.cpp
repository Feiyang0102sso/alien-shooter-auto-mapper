// AutoMapper.Tests.exe entry: core tests (GoogleTest), then editor tests (Qt Test).
//   AutoMapper.Tests.exe [gtest flags] [Qt Test flags]   run every test; exit code 0 only when both pass
//   AutoMapper.Tests.exe --export-golden <dir>           write golden candidates to <dir>, run no tests
// Run with the repository root as working directory: tests look up tests/golden from there.
#include <gtest/gtest.h>
#include <QByteArray>
#include <QtGlobal>
#include <filesystem>
#include <iostream>
#include <string>

// tests/editor/test_editor.cpp
int run_editor_tests(int argc, char **argv);
// tests/golden_candidates.cpp
int export_golden_candidates(const std::filesystem::path &output);

int main(int argc, char **argv) {
    // Golden updates stay separate from normal test runs.
    if (argc >= 2 && std::string(argv[1]) == "--export-golden") {
        if (argc != 3) {
            std::cerr << "Usage: AutoMapper.Tests.exe --export-golden <output-directory>\n";
            return 1;
        }
        return export_golden_candidates(argv[2]);
    }

    // InitGoogleTest removes the --gtest_* flags; what remains goes to Qt Test (for example -o file,txt).
    testing::InitGoogleTest(&argc, argv);
    int core_result = RUN_ALL_TESTS();

    // Editor tests need no screen; set QT_QPA_PLATFORM yourself to watch the windows.
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }
    int editor_result = run_editor_tests(argc, argv);

    std::cout << "[Tests] core exit code " << core_result << ", editor exit code " << editor_result << std::endl;
    if (core_result != 0 || editor_result != 0) {
        std::cout << "[Tests] FAILED" << std::endl;
        return 1;
    }
    std::cout << "[Tests] all passed" << std::endl;
    return 0;
}
