#pragma once
#include <string>
#include <vector>
#include "DataStore.h"

struct Record {
    int         id{};
    std::string name;   // 제목
    std::string value;  // 내용
};

// DataStore 위에서 ID 기반 레코드 CRUD를 관리하는 계층
class RecordStore {
public:
    explicit RecordStore(const std::string& filePath);

    void load();
    void save() const;

    int                 create(const std::string& name, const std::string& value);
    std::vector<Record> readAll() const;
    const Record*       findById(int id) const;
    std::vector<Record> findByKeyword(const std::string& keyword) const;
    bool                update(int id, const std::string& name, const std::string& value);
    bool                remove(int id);

private:
    DataStore           m_store;
    std::vector<Record> m_records;
    int                 m_nextId{ 1 };

    void syncFromStore();
    void syncToStore();
};
