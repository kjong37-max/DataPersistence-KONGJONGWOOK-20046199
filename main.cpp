#include <ctime>
#include <iostream>
#include <string>

#include "DataStore.h"

static std::string currentTimestamp() {
    const std::time_t now = std::time(nullptr);
    char buf[32]{};
    struct tm tm_info {};
    localtime_s(&tm_info, &now);
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_info);
    return buf;
}

static void printMenu() {
    std::cout << "\n[메뉴]\n"
                 "  1. 전체 조회\n"
                 "  2. 데이터 저장\n"
                 "  3. 데이터 삭제\n"
                 "  0. 저장 후 종료\n"
                 "선택 > ";
}

int main() {
    constexpr auto DATA_FILE = "persistent_data.txt";

    DataStore store(DATA_FILE);
    store.load();

    // 실행 횟수 및 마지막 실행 시각 자동 갱신
    const int runCount = std::stoi(store.get("__run_count__", "0")) + 1;
    store.set("__run_count__", std::to_string(runCount));
    store.set("__last_run__", currentTimestamp());

    std::cout << "============================\n"
              << " 데이터 영속성 PoC\n"
              << "============================\n"
              << "저장 파일 : " << DATA_FILE << '\n'
              << "실행 횟수 : " << runCount << "회\n"
              << "최근 실행 : " << store.get("__last_run__") << '\n';

    int choice = -1;
    while (choice != 0) {
        printMenu();
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1:
            std::cout << '\n';
            store.printAll();
            break;

        case 2: {
            std::string key, value;
            std::cout << "  키   > "; std::getline(std::cin, key);
            std::cout << "  값   > "; std::getline(std::cin, value);
            if (!key.empty()) {
                store.set(key, value);
                std::cout << "  저장 완료.\n";
            }
            break;
        }

        case 3: {
            std::string key;
            std::cout << "  삭제할 키 > "; std::getline(std::cin, key);
            if (store.has(key)) {
                store.remove(key);
                std::cout << "  삭제 완료.\n";
            } else {
                std::cout << "  키를 찾을 수 없습니다.\n";
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
    std::cout << "\n데이터가 '" << DATA_FILE
              << "'에 저장되었습니다.\n"
                 "다음 실행 시 자동으로 복원됩니다.\n";

    return 0;
}
