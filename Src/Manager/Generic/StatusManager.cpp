
#include <DxLib.h>
#include <fstream>
#include <sstream>
#include "StatusManager.h"

const std::string StatusManager::PATH_CSV = "Data/CSV/";
const std::string StatusManager::PATH_DAT = "Data/DAT/";

void StatusManager::Initialize() {
    // --------------------------------------------------
      // 1. プレイヤーCSVの変換チェック
      // --------------------------------------------------
    int fileHandle = FileRead_open((PATH_CSV + "PlayerStatus.csv").c_str());
    if (fileHandle != 0) {
        FileRead_close(fileHandle);
        ConvertPlayerCsv((PATH_CSV + "PlayerStatus.csv").c_str(), (PATH_DAT + "PlayerStatus.dat").c_str());
    }

    // --------------------------------------------------
    // 2. エネミーCSVの変換チェック
    // --------------------------------------------------
    fileHandle = FileRead_open((PATH_CSV + "EnemyStatus.csv").c_str());
    if (fileHandle != 0) {
        FileRead_close(fileHandle);
        ConvertEnemyCsv((PATH_CSV + "EnemyStatus.csv").c_str(), (PATH_DAT + "EnemyStatus.dat").c_str());
    }

    // --------------------------------------------------
    // 3. ボスCSVの変換チェック
    // --------------------------------------------------
    fileHandle = FileRead_open((PATH_CSV + "BossStatus.csv").c_str());
    if (fileHandle != 0) {
        FileRead_close(fileHandle);
        ConvertBossCsv((PATH_CSV + "BossStatus.csv").c_str(), (PATH_DAT + "BossStatus.dat").c_str());
    }
}

// 文字列分割処理
std::vector<std::string> StatusManager::SplitCSVLine(const std::string& line) {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (!item.empty() && item.back() == '\r') {
            item.pop_back(); // Windowsの改行コード除去
        }
        result.push_back(item);
    }
    return result;
}

// --------------------------------------------------
// プレイヤー用の変換 & 読み込み
// --------------------------------------------------
bool StatusManager::ConvertPlayerCsv(const std::string& csvPath, const std::string& datPath) {
    std::ifstream file(csvPath);
    if (!file.is_open()) return false;

    std::string line;
    std::getline(file, line); // ヘッダー行をスキップ

    std::vector<PlayerStatus> list;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = SplitCSVLine(line);
        if (tokens.size() < 7) continue;

        PlayerStatus st;
        st.id = std::stoi(tokens[0]);
        st.name = tokens[1];
        st.hp = std::stoi(tokens[2]);
        st.attack = std::stoi(tokens[3]);
        list.push_back(st);
    }
    file.close();

    // DATファイルへ書き出し
    std::ofstream outFile(datPath, std::ios::binary);
    if (!outFile.is_open()) return false;

    size_t count = list.size();
    outFile.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& st : list) {
        outFile.write(reinterpret_cast<const char*>(&st.id), sizeof(st.id));
        size_t nameLen = st.name.size();
        outFile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        outFile.write(st.name.c_str(), nameLen);
        outFile.write(reinterpret_cast<const char*>(&st.hp), sizeof(st.hp));
        outFile.write(reinterpret_cast<const char*>(&st.attack), sizeof(st.attack));
    }
    return true;
}

bool StatusManager::ConvertEnemyCsv(const std::string& csvPath, const std::string& datPath)
{
    return false;
}

PlayerStatus StatusManager::LoadPlayerStatus(const std::string& datPath) {
    PlayerStatus st{};
    std::ifstream inFile(datPath, std::ios::binary);
    if (!inFile.is_open()) return st;

    size_t count = 0;
    inFile.read(reinterpret_cast<char*>(&count), sizeof(count));
    if (count > 0) {
        inFile.read(reinterpret_cast<char*>(&st.id), sizeof(st.id));
        size_t nameLen = 0;
        inFile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        st.name.resize(nameLen);
        inFile.read(&st.name[0], nameLen);
        inFile.read(reinterpret_cast<char*>(&st.hp), sizeof(st.hp));
    }
    return st;
}

EnemyStatus StatusManager::LoadEnemyStatus(const std::string& datPath)
{
    return EnemyStatus();
}

// --------------------------------------------------
// ボス用の変換 & 読み込み
// --------------------------------------------------
bool StatusManager::ConvertBossCsv(const std::string& csvPath, const std::string& datPath) {
    std::ifstream file(csvPath);
    if (!file.is_open()) return false;

    std::string line;
    std::getline(file, line); // ヘッダー行をスキップ

    std::vector<BossStatus> list;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = SplitCSVLine(line);
        if (tokens.size() < 7) continue;

        BossStatus st;
        st.id = std::stoi(tokens[0]);
        st.name = tokens[1];
        st.hp = std::stoi(tokens[2]);
        list.push_back(st);
    }
    file.close();

    std::ofstream outFile(datPath, std::ios::binary);
    if (!outFile.is_open()) return false;

    size_t count = list.size();
    outFile.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& st : list) {
        outFile.write(reinterpret_cast<const char*>(&st.id), sizeof(st.id));
        size_t nameLen = st.name.size();
        outFile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        outFile.write(st.name.c_str(), nameLen);
        outFile.write(reinterpret_cast<const char*>(&st.hp), sizeof(st.hp));
    }
    return true;
}

BossStatus StatusManager::LoadBossStatus(const std::string& datPath) {
    BossStatus st{};
    std::ifstream inFile(datPath, std::ios::binary);
    if (!inFile.is_open()) return st;

    size_t count = 0;
    inFile.read(reinterpret_cast<char*>(&count), sizeof(count));
    if (count > 0) {
        inFile.read(reinterpret_cast<char*>(&st.id), sizeof(st.id));
        size_t nameLen = 0;
        inFile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        st.name.resize(nameLen);
        inFile.read(&st.name[0], nameLen);
        inFile.read(reinterpret_cast<char*>(&st.hp), sizeof(st.hp));
    }
    return st;
}