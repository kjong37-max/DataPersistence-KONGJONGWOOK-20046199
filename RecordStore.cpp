#include "RecordStore.h"
#include <algorithm>

RecordStore::RecordStore(const std::string& filePath)
    : m_store(filePath) {}

void RecordStore::load() {
    m_store.load();
    syncFromStore();
}

void RecordStore::save() const {
    m_store.save();
}

// 파일 형식: rec.{id}.name / rec.{id}.value / __next_id__
void RecordStore::syncFromStore() {
    m_records.clear();
    m_nextId = std::stoi(m_store.get("__next_id__", "1"));

    for (const auto& key : m_store.keys()) {
        // "rec.{id}.name" 패턴만 처리
        if (!key.starts_with("rec.") || !key.ends_with(".name"))
            continue;
        const std::string idStr = key.substr(4, key.size() - 9);
        Record r;
        r.id    = std::stoi(idStr);
        r.name  = m_store.get("rec." + idStr + ".name");
        r.value = m_store.get("rec." + idStr + ".value");
        m_records.push_back(r);
    }

    std::sort(m_records.begin(), m_records.end(),
              [](const Record& a, const Record& b) { return a.id < b.id; });
}

void RecordStore::syncToStore() {
    // 기존 레코드 키 전체 제거 (snapshot 후 순회해야 반복자 무효화 방지)
    const auto snapshot = m_store.keys();
    for (const auto& key : snapshot)
        if (key.starts_with("rec."))
            m_store.remove(key);

    for (const auto& r : m_records) {
        const std::string idStr = std::to_string(r.id);
        m_store.set("rec." + idStr + ".name",  r.name);
        m_store.set("rec." + idStr + ".value", r.value);
    }
    m_store.set("__next_id__", std::to_string(m_nextId));
}

int RecordStore::create(const std::string& name, const std::string& value) {
    const int id = m_nextId++;
    m_records.push_back({ id, name, value });
    syncToStore();
    return id;
}

std::vector<Record> RecordStore::readAll() const {
    return m_records;
}

const Record* RecordStore::findById(int id) const {
    for (const auto& r : m_records)
        if (r.id == id) return &r;
    return nullptr;
}

std::vector<Record> RecordStore::findByKeyword(const std::string& keyword) const {
    std::vector<Record> result;
    for (const auto& r : m_records)
        if (r.name.find(keyword) != std::string::npos ||
            r.value.find(keyword) != std::string::npos)
            result.push_back(r);
    return result;
}

bool RecordStore::update(int id, const std::string& name, const std::string& value) {
    for (auto& r : m_records) {
        if (r.id != id) continue;
        r.name  = name;
        r.value = value;
        syncToStore();
        return true;
    }
    return false;
}

bool RecordStore::remove(int id) {
    const auto it = std::remove_if(m_records.begin(), m_records.end(),
                                   [id](const Record& r) { return r.id == id; });
    if (it == m_records.end()) return false;
    m_records.erase(it, m_records.end());
    syncToStore();
    return true;
}
