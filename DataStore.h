#pragma once
#include <string>
#include <unordered_map>
#include <vector>

enum class StoreFormat { TXT, JSON };

// 파일 확장자(.txt / .json)로 형식을 자동 감지하는 key=value 영속 저장소
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

    StoreFormat format() const;

    void printAll() const;

private:
    std::string                                  m_filePath;
    StoreFormat                                  m_format;
    std::unordered_map<std::string, std::string> m_data;

    void loadTxt();
    void loadJson();
    void saveTxt()  const;
    void saveJson() const;

    static StoreFormat detectFormat(const std::string& filePath);
    static std::string jsonEscape(const std::string& s);
};
