#include "DataStore.h"

#include <fstream>
#include <iostream>

DataStore::DataStore(const std::string& filePath) : m_filePath(filePath) {}

void DataStore::load() {
    std::ifstream file(m_filePath);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        const auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        m_data[line.substr(0, pos)] = line.substr(pos + 1);
    }
}

void DataStore::save() const {
    std::ofstream file(m_filePath);
    for (const auto& [key, value] : m_data)
        file << key << '=' << value << '\n';
}

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

void DataStore::printAll() const {
    if (m_data.empty()) {
        std::cout << "  (저장된 데이터 없음)\n";
        return;
    }
    for (const auto& [key, value] : m_data) {
        if (key.starts_with("__")) continue; // 내부 메타키 숨김
        std::cout << "  [" << key << "] = " << value << '\n';
    }
}
