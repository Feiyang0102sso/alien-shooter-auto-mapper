/**
* @file test_utils.h
 * @brief utils for testing
 */


#pragma once

#include "auto_mapper/core/wall_builder.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <cctype>

namespace auto_mapper::test {

// resolve relative test path by searching upwards from current workdir
inline std::string resolve_test_path(const std::string& relative_path) {
    namespace fs = std::filesystem;
    fs::path rel(relative_path);
    fs::path current = fs::current_path();

    for (int depth = 0; depth < 5; ++depth) {
        fs::path candidate = current / rel;
        if (fs::exists(candidate)) {
            std::string abs_path = fs::absolute(candidate).string();
            std::cout << "[PathResolver] Resolved '" << relative_path << "' to: " << abs_path << std::endl;
            return abs_path;
        }
        if (current.has_parent_path()) {
            current = current.parent_path();
        } else {
            break;
        }
    }
    
    std::string fallback = fs::absolute(fs::current_path() / rel).string();
    std::cout << "[PathResolver] WARNING: Could not resolve '" << relative_path << "' upwards. Fallback to: " << fallback << std::endl;
    return fallback;
}

// Get absolute path of the project root directory
inline std::string get_project_root() {
    namespace fs = std::filesystem;
    std::string resolved = resolve_test_path("tests/golden/wall_builder.gold.json");
    return fs::path(resolved).parent_path().parent_path().parent_path().string();
}

// compare 2 .map file
inline bool compare_binary_files(const std::string& path1, const std::string& path2) {
    namespace fs = std::filesystem;
    std::ifstream f1(path1, std::ios::binary);
    std::ifstream f2(path2, std::ios::binary);

    if (!f1.is_open() || !f2.is_open()) {
        std::cerr << "[Compare] Error opening files!" << std::endl;
        std::cerr << "  path1: " << path1 << " (Absolute: " << (fs::exists(path1) ? fs::absolute(path1).string() : "NOT_FOUND") << ")" << std::endl;
        std::cerr << "  path2: " << path2 << " (Absolute: " << (fs::exists(path2) ? fs::absolute(path2).string() : "NOT_FOUND") << ")" << std::endl;
        std::cerr << "  Current WorkDir: " << fs::current_path().string() << std::endl;
        return false;
    }

    // 1. file size
    f1.seekg(0, std::ios::end);
    f2.seekg(0, std::ios::end);
    if (f1.tellg() != f2.tellg()) {
        std::cerr << "[Compare] File size mismatch: " << path1 << " (" << f1.tellg() 
                  << " bytes) vs " << path2 << " (" << f2.tellg() << " bytes)" << std::endl;
        return false;
    }

    // 2. back to start and compare bytes
    f1.seekg(0, std::ios::beg);
    f2.seekg(0, std::ios::beg);

    char c1, c2;
    size_t byte_idx = 0;
    while (f1.get(c1) && f2.get(c2)) {
        if (c1 != c2) {
            std::cerr << "[Compare] Mismatch at byte " << byte_idx 
                      << ": " << path1 << " has 0x" << std::hex << (int)(unsigned char)c1
                      << ", but " << path2 << " has 0x" << (int)(unsigned char)c2 << std::dec << std::endl;
            return false;
        }
        byte_idx++;
    }

    return true;
}


// RAII class for removing temp file

class TempFileCleaner {
public:
    explicit TempFileCleaner(std::string filepath) : filepath_(std::move(filepath)) {}

    ~TempFileCleaner() {
        if (!filepath_.empty() && std::filesystem::exists(filepath_)) {
            std::error_code ec;
            std::filesystem::remove(filepath_, ec);
            if (ec) {
                std::cerr << "[Cleaner] Failed to remove temp file " << filepath_ << ": " << ec.message() << std::endl;
            }
        }
    }

    // no copy
    TempFileCleaner(const TempFileCleaner&) = delete;
    TempFileCleaner& operator=(const TempFileCleaner&) = delete;

private:
    std::string filepath_;
};

// store test scene after processing
struct TestScene {
    float map_size_x = 600.0f;
    float map_size_y = 600.0f;
    std::vector<core::Segment> segments;
};

enum class TokenType {
    LBrace, RBrace, LBracket, RBracket, Colon, Comma, String, Number, EndOfFile
};

struct Token {
    TokenType type;
    std::string value;
};

// json to token
inline std::vector<Token> tokenize(const std::string& source) {
    std::vector<Token> tokens;
    size_t i = 0;
    while (i < source.size()) {
        char c = source[i];
        if (std::isspace(c)) {
            i++;
            continue;
        }
        if (c == '{') {
            tokens.push_back({TokenType::LBrace, "{"});
            i++;
        } else if (c == '}') {
            tokens.push_back({TokenType::RBrace, "}"});
            i++;
        } else if (c == '[') {
            tokens.push_back({TokenType::LBracket, "["});
            i++;
        } else if (c == ']') {
            tokens.push_back({TokenType::RBracket, "]"});
            i++;
        } else if (c == ':') {
            tokens.push_back({TokenType::Colon, ":"});
            i++;
        } else if (c == ',') {
            tokens.push_back({TokenType::Comma, ","});
            i++;
        } else if (c == '"') {
            std::string s;
            i++;
            while (i < source.size() && source[i] != '"') {
                s += source[i];
                i++;
            }
            if (i < source.size()) {
                i++;
            }
            tokens.push_back({TokenType::String, s});
        } else if (std::isdigit(c) || c == '-' || c == '.') {
            std::string num;
            while (i < source.size() && (std::isdigit(source[i]) || source[i] == '.' || source[i] == '-' || source[i] == 'e' || source[i] == 'E' || source[i] == '+')) {
                num += source[i];
                i++;
            }
            tokens.push_back({TokenType::Number, num});
        } else {
            i++;
        }
    }
    tokens.push_back({TokenType::EndOfFile, ""});
    return tokens;
}


// build Token to TestScene
inline TestScene parse_test_scene(const std::vector<Token>& tokens) {
    TestScene scene;
    size_t i = 0;

    while (i < tokens.size() && tokens[i].type != TokenType::EndOfFile) {
        if (tokens[i].type == TokenType::String) {
            std::string key = tokens[i].value;
            if (key == "map_size_x") {
                if (i + 2 < tokens.size() && tokens[i+1].type == TokenType::Colon && tokens[i+2].type == TokenType::Number) {
                    scene.map_size_x = std::stof(tokens[i+2].value);
                    i += 3;
                } else {
                    i++;
                }
            } else if (key == "map_size_y") {
                if (i + 2 < tokens.size() && tokens[i+1].type == TokenType::Colon && tokens[i+2].type == TokenType::Number) {
                    scene.map_size_y = std::stof(tokens[i+2].value);
                    i += 3;
                } else {
                    i++;
                }
            } else if (key == "segments") {
                if (i + 2 < tokens.size() && tokens[i+1].type == TokenType::Colon && tokens[i+2].type == TokenType::LBracket) {
                    i += 3;
                    while (i < tokens.size() && tokens[i].type != TokenType::RBracket && tokens[i].type != TokenType::EndOfFile) {
                        if (tokens[i].type == TokenType::LBrace) {
                            i++;
                            core::Segment seg;
                            seg.wall_type = 0;
                            seg.floor_type = 0;
                            while (i < tokens.size() && tokens[i].type != TokenType::RBrace && tokens[i].type != TokenType::EndOfFile) {
                                if (tokens[i].type == TokenType::String) {
                                    std::string s_key = tokens[i].value;
                                    if (s_key == "start" || s_key == "end") {
                                        core::GridPoint pt{0, 0};
                                        if (i + 3 < tokens.size() && tokens[i+1].type == TokenType::Colon && tokens[i+2].type == TokenType::LBrace) {
                                            i += 3;
                                            while (i < tokens.size() && tokens[i].type != TokenType::RBrace) {
                                                if (tokens[i].type == TokenType::String) {
                                                    std::string coord_key = tokens[i].value;
                                                    if (i + 2 < tokens.size() && tokens[i+1].type == TokenType::Colon && tokens[i+2].type == TokenType::Number) {
                                                        int val = std::stoi(tokens[i+2].value);
                                                        if (coord_key == "x") {
                                                            pt.x = val;
                                                        } else if (coord_key == "y") {
                                                            pt.y = val;
                                                        }
                                                        i += 3;
                                                    } else {
                                                        i++;
                                                    }
                                                } else {
                                                    i++;
                                                }
                                            }
                                            if (i < tokens.size() && tokens[i].type == TokenType::RBrace) {
                                                i++;
                                            }
                                        } else {
                                            i++;
                                        }
                                        if (s_key == "start") {
                                            seg.start = pt;
                                        } else {
                                            seg.end = pt;
                                        }
                                    } else if (s_key == "wall_type") {
                                        if (i + 2 < tokens.size() && tokens[i+1].type == TokenType::Colon && tokens[i+2].type == TokenType::Number) {
                                            seg.wall_type = std::stoi(tokens[i+2].value);
                                            i += 3;
                                        } else {
                                            i++;
                                        }
                                    } else if (s_key == "floor_type") {
                                        if (i + 2 < tokens.size() && tokens[i+1].type == TokenType::Colon && tokens[i+2].type == TokenType::Number) {
                                            seg.floor_type = std::stoi(tokens[i+2].value);
                                            i += 3;
                                        } else {
                                            i++;
                                        }
                                    } else {
                                        i++;
                                    }
                                } else {
                                    i++;
                                }
                            }
                            if (i < tokens.size() && tokens[i].type == TokenType::RBrace) {
                                i++;
                            }
                            scene.segments.push_back(seg);
                        } else {
                            i++;
                        }
                    }
                    if (i < tokens.size() && tokens[i].type == TokenType::RBracket) {
                        i++;
                    }
                } else {
                    i++;
                }
            } else {
                i++;
            }
        } else {
            i++;
        }
    }
    return scene;
}


// load file and tokenize helper
inline std::vector<Token> load_json_tokens(const std::string& json_path) {
    namespace fs = std::filesystem;
    std::ifstream file(json_path);
    if (!file.is_open()) {
        std::cerr << "[Loader] Failed to open JSON file: " << json_path << std::endl;
        std::cerr << "  Absolute: " << (fs::exists(json_path) ? fs::absolute(json_path).string() : "NOT_FOUND") << std::endl;
        std::cerr << "  Current WorkDir: " << fs::current_path().string() << std::endl;
        return {};
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return tokenize(buffer.str());
}

// load json and build TestScene
inline TestScene load_test_scene(const std::string& json_path) {
    return parse_test_scene(load_json_tokens(json_path));
}

} // namespace auto_mapper::test
