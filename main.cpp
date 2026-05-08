#include <iostream>
#include <string>
#include <vector>
#include "RecordStore.h"

static void printSeparator() {
    std::cout << "  " << std::string(46, '-') << '\n';
}

static void printRecord(const Record& r) {
    std::cout << "  ID   : " << r.id    << '\n'
              << "  제목 : " << r.name  << '\n'
              << "  내용 : " << r.value << '\n';
}

static void printList(const std::vector<Record>& records) {
    if (records.empty()) {
        std::cout << "  (항목 없음)\n";
        return;
    }
    printSeparator();
    for (const auto& r : records)
        std::cout << "  [" << r.id << "] " << r.name << " \xE2\x80\x94 " << r.value << '\n';
    printSeparator();
}

static void printMenu() {
    std::cout << "\n[메뉴]\n"
                 "  1. 전체 목록 보기   (Read All)\n"
                 "  2. 검색             (Read by ID / 키워드)\n"
                 "  3. 새 항목 추가     (Create)\n"
                 "  4. 항목 수정        (Update)\n"
                 "  5. 항목 삭제        (Delete)\n"
                 "  0. 저장 후 종료\n"
                 "선택 > ";
}

static int readInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);
        try { return std::stoi(line); }
        catch (...) { std::cout << "  숫자를 입력하세요.\n"; }
    }
}

static std::string selectDataFile() {
    std::cout << "================================\n"
              << " 데이터 영속성 CRUD PoC\n"
              << "================================\n"
              << "저장 형식을 선택하세요:\n"
              << "  1. TXT  (persistent_data.txt)\n"
              << "  2. JSON (persistent_data.json)\n"
              << "선택 > ";
    std::string line;
    std::getline(std::cin, line);
    return (line == "2") ? "persistent_data.json" : "persistent_data.txt";
}

int main() {
    const std::string DATA_FILE = selectDataFile();
    const bool        isJson    = DATA_FILE.ends_with(".json");

    RecordStore store(DATA_FILE);
    store.load();

    std::cout << "\n저장 파일 : " << DATA_FILE << '\n'
              << "저장 형식 : " << (isJson ? "JSON" : "TXT") << '\n'
              << "레코드 수 : " << store.readAll().size() << "개\n";

    int choice = -1;
    while (choice != 0) {
        printMenu();
        std::string line;
        std::getline(std::cin, line);
        try { choice = std::stoi(line); } catch (...) { choice = -1; }

        switch (choice) {

        // ── Read All ─────────────────────────────────────
        case 1:
            std::cout << '\n';
            printList(store.readAll());
            break;

        // ── Read (Search) ────────────────────────────────
        case 2: {
            std::cout << "  ID 검색(1) / 키워드 검색(2) > ";
            std::string sub; std::getline(std::cin, sub);

            if (sub == "1") {
                const int id = readInt("  ID > ");
                if (const auto* r = store.findById(id)) {
                    std::cout << '\n'; printRecord(*r);
                } else {
                    std::cout << "  ID " << id << "를 찾을 수 없습니다.\n";
                }
            } else if (sub == "2") {
                std::cout << "  키워드 > ";
                std::string kw; std::getline(std::cin, kw);
                std::cout << '\n';
                printList(store.findByKeyword(kw));
            }
            break;
        }

        // ── Create ───────────────────────────────────────
        case 3: {
            std::string name, value;
            std::cout << "  제목 > "; std::getline(std::cin, name);
            std::cout << "  내용 > "; std::getline(std::cin, value);
            if (name.empty()) {
                std::cout << "  제목은 비워둘 수 없습니다.\n";
            } else {
                const int id = store.create(name, value);
                std::cout << "  추가 완료 (ID: " << id << ")\n";
            }
            break;
        }

        // ── Update ───────────────────────────────────────
        case 4: {
            const int id = readInt("  수정할 ID > ");
            if (const auto* r = store.findById(id)) {
                const std::string oldName  = r->name;
                const std::string oldValue = r->value;
                std::cout << "  현재 제목: " << oldName  << '\n'
                          << "  현재 내용: " << oldValue << '\n';
                std::string newName, newValue;
                std::cout << "  새 제목 (Enter = 유지) > "; std::getline(std::cin, newName);
                std::cout << "  새 내용 (Enter = 유지) > "; std::getline(std::cin, newValue);
                store.update(id,
                    newName.empty()  ? oldName  : newName,
                    newValue.empty() ? oldValue : newValue);
                std::cout << "  수정 완료.\n";
            } else {
                std::cout << "  ID " << id << "를 찾을 수 없습니다.\n";
            }
            break;
        }

        // ── Delete ───────────────────────────────────────
        case 5: {
            const int id = readInt("  삭제할 ID > ");
            if (const auto* r = store.findById(id)) {
                const std::string name = r->name;
                std::cout << "  [" << id << "] " << name
                          << " \xE2\x80\x94 삭제하시겠습니까? (y/N) > ";
                std::string confirm; std::getline(std::cin, confirm);
                if (confirm == "y" || confirm == "Y") {
                    store.remove(id);
                    std::cout << "  삭제 완료.\n";
                } else {
                    std::cout << "  취소되었습니다.\n";
                }
            } else {
                std::cout << "  ID " << id << "를 찾을 수 없습니다.\n";
            }
            break;
        }

        case 0:
            break;

        default:
            std::cout << "  올바른 메뉴를 선택하세요.\n";
        }
    }

    store.save();
    std::cout << "\n데이터가 '" << DATA_FILE << "'에 저장되었습니다.\n"
                 "다음 실행 시 자동으로 복원됩니다.\n";

    return 0;
}
