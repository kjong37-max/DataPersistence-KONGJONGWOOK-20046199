#include "DataStore.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <iterator>

DataStore::DataStore(const std::string& filePath)
    : m_filePath(filePath), m_format(detectFormat(filePath)) {}

// ── 형식 감지 ──────────────────────────────────────────────────────────────

StoreFormat DataStore::detectFormat(const std::string& filePath) {
    const auto dot = filePath.rfind('.');
    if (dot != std::string::npos) {
        std::string ext = filePath.substr(dot + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        if (ext == "json") return StoreFormat::JSON;
    }
    return StoreFormat::TXT;
}

StoreFormat DataStore::format() const { return m_format; }

// ── load / save 진입점 ────────────────────────────────────────────────────

void DataStore::load() {
    switch (m_format) {
    case StoreFormat::JSON: loadJson(); break;
    default:                loadTxt();  break;
    }
}

void DataStore::save() const {
    switch (m_format) {
    case StoreFormat::JSON: saveJson(); break;
    default:                saveTxt();  break;
    }
}

// ── TXT 형식 ──────────────────────────────────────────────────────────────

void DataStore::loadTxt() {
    std::ifstream file(m_filePath);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        const auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        m_data[line.substr(0, pos)] = line.substr(pos + 1);
    }
}

void DataStore::saveTxt() const {
    std::ofstream file(m_filePath);
    for (const auto& [key, value] : m_data)
        file << key << '=' << value << '\n';
}

// ── JSON 형식 ─────────────────────────────────────────────────────────────

void DataStore::loadJson() {
    std::ifstream file(m_filePath);
    if (!file.is_open()) return;

    const std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
    size_t pos = 0;

    auto skipWs = [&]() {
        while (pos < content.size() &&
               std::isspace(static_cast<unsigned char>(content[pos])))
            ++pos;
    };

    // JSON 문자열 한 개를 읽고 반환 (커서 pos 이동)
    auto readString = [&]() -> std::string {
        skipWs();
        if (pos >= content.size() || content[pos] != '"') return {};
        ++pos; // opening "
        std::string result;
        while (pos < content.size() && content[pos] != '"') {
            if (content[pos] == '\\' && pos + 1 < content.size()) {
                ++pos;
                switch (content[pos]) {
                case '"':  result += '"';  break;
                case '\\': result += '\\'; break;
                case '/':  result += '/';  break;
                case 'n':  result += '\n'; break;
                case 'r':  result += '\r'; break;
                case 't':  result += '\t'; break;
                default:   result += content[pos]; break;
                }
            } else {
                result += content[pos];
            }
            ++pos;
        }
        if (pos < content.size()) ++pos; // closing "
        return result;
    };

    skipWs();
    if (pos < content.size() && content[pos] == '{') ++pos;

    while (pos < content.size()) {
        skipWs();
        if (pos >= content.size() || content[pos] == '}') break;
        if (content[pos] == ',') { ++pos; continue; }

        const std::string key = readString();
        skipWs();
        if (pos < content.size() && content[pos] == ':') ++pos;
        const std::string value = readString();

        if (!key.empty()) m_data[key] = value;
    }
}

void DataStore::saveJson() const {
    std::ofstream file(m_filePath);
    file << "{\n";
    bool first = true;
    for (const auto& [key, value] : m_data) {
        if (!first) file << ",\n";
        file << "  \"" << jsonEscape(key) << "\": \""
             << jsonEscape(value) << "\"";
        first = false;
    }
    file << "\n}\n";
}

std::string DataStore::jsonEscape(const std::string& s) {
    std::string result;
    result.reserve(s.size());
    for (const char c : s) {
        switch (c) {
        case '"':  result += "\\\""; break;
        case '\\': result += "\\\\"; break;
        case '\n': result += "\\n";  break;
        case '\r': result += "\\r";  break;
        case '\t': result += "\\t";  break;
        default:   result += c;      break;
        }
    }
    return result;
}

// ── 공통 CRUD ─────────────────────────────────────────────────────────────

void DataStore::set(const std::string& key, const std::string& value) {
    m_data[key] = value;
}

std::string DataStore::get(const std::string& key, const std::string& defaultValue) const {
    const auto it = m_data.find(key);
    return (it != m_data.end()) ? it->second : defaultValue;
}

bool DataStore::has(const std::string& key) const {
    return m_data.contains(key);
}

void DataStore::remove(const std::string& key) {
    m_data.erase(key);
}

std::vector<std::string> DataStore::keys() const {
    std::vector<std::string> result;
    result.reserve(m_data.size());
    for (const auto& [key, _] : m_data)
        result.push_back(key);
    return result;
}

void DataStore::printAll() const {
    if (m_data.empty()) {
        std::cout << "  (저장된 데이터 없음)\n";
        return;
    }
    for (const auto& [key, value] : m_data) {
        if (key.starts_with("__")) continue;
        std::cout << "  [" << key << "] = " << value << '\n';
    }
}
