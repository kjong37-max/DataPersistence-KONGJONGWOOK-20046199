#pragma once
#include <string>
#include <unordered_map>
#include <vector>

// key=value 텍스트 파일로 데이터를 영속 저장하는 단순 저장소
class DataStore {
public:
    explicit DataStore(const std::string& filePath);

    void load();
    void save() const;

    void                     set(const std::string& key, const std::string& value);
    std::string              get(const std::string& key, const std::string& defaultValue = "") const;
    bool                     has(const std::string& key) const;
    void                     remove(const std::string& key);
    std::vector<std::string> keys() const;

    void printAll() const;

private:
    std::string                                  m_filePath;
    std::unordered_map<std::string, std::string> m_data;
};
