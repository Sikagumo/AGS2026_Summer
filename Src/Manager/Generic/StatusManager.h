#pragma once
#include <string>
#include <vector>

// --------------------------------------------------
// 1. 各キャラクター専用のステータス構造体
// --------------------------------------------------

// プレイヤー用
struct PlayerStatus {
    int id = 0;
    std::string name = "";
    int hp = 0;
    int attack = 0;
};

// 通常エネミー用
struct EnemyStatus {
    int id = 0;
    std::string name = "";
    int hp = 0;
};

// ボス用
struct BossStatus {
    int id = 0;
    std::string name = "";
    int hp = 0;
};

// --------------------------------------------------
// 2. ステータス管理クラス
// --------------------------------------------------
class StatusManager {
public:
    // CSVをそれぞれの専用DATに自動変換する
    static void Initialize();

    //読み込み関数
    static PlayerStatus LoadPlayerStatus(const std::string& datPath);
    static EnemyStatus  LoadEnemyStatus(const std::string& datPath);
    static BossStatus   LoadBossStatus(const std::string& datPath);

private:

	static const std::string PATH_CSV;
	static const std::string PATH_DAT;


    //各専用の変換関数
    static bool ConvertPlayerCsv(const std::string& csvPath, const std::string& datPath);
    static bool ConvertEnemyCsv(const std::string& csvPath, const std::string& datPath);
    static bool ConvertBossCsv(const std::string& csvPath, const std::string& datPath);

    // CSV文字列分解用のヘルパー関数
    static std::vector<std::string> SplitCSVLine(const std::string& line);
};