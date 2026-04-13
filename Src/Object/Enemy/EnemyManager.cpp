#include "EnemyManager.h"
#include <string>
#include <fstream>
#include "./EnemyBase.h"
#include "./Charactor/EnemyRat.h"
#include "./Charactor/EnemyRobot.h"
#include "../Player/Player.h"
#include "../Common/Transform.h"
#include "../../Application.h"
#include "../../Utility/AsoUtility.h"

EnemyManager::EnemyManager(Player* _player):
	player_(*_player)
{
}

void EnemyManager::Init(void)
{
	// 敵のデータ読み込み
	LoadCsvData();
}
void EnemyManager::LoadCsvData(void)
{
	// ファイルの読込
	std::ifstream ifs = std::ifstream(Application::PATH_CSV + "EnemyData.csv");

	// エラーが発生
	if (!ifs) { return; }

	// ファイルを１行ずつ読み込む
	std::string line;// 1行の文字情報
	std::vector<std::string> strSplit; // 1行を1文字の動的配列に分割
	bool isHeader = true;

	while (getline(ifs, line))
	{
		if (isHeader)
		{
			isHeader = false;
			continue;
		}

		// １行をカンマ区切りで分割
		strSplit = AsoUtility::Split(line, ',');
		EnemyBase* enemy = nullptr;

		// 構造体に合わせて読込データを格納
		EnemyBase::EnemyData data = EnemyBase::EnemyData();
		int idx = 0;

		// ID
		data.id = stoi(strSplit[idx++]);
		// 種別
		data.type = static_cast<EnemyBase::TYPE>(stoi(strSplit[idx++]));
		// HP
		data.hp = stoi(strSplit[idx++]);

		// 初期座標
		data.defaultPos = {stof(strSplit[idx++]),
						   stof(strSplit[idx++]),
						   stof(strSplit[idx++])};

		// 移動範囲
		data.moveRange = stoi(strSplit[idx++]);


		// 敵生成
		Create(data);
	}
	ifs.close();
}
EnemyBase* EnemyManager::Create(const EnemyBase::EnemyData& data)
{
	EnemyBase* enemy = nullptr;

	EnemyBase::TYPE type = static_cast<EnemyBase::TYPE>(data.type);
	switch (type)
	{
		case EnemyBase::TYPE::RAT:
		{
			enemy = new EnemyRat(data, player_);
		}
		break;

		case EnemyBase::TYPE::ROBOT:
		{
			enemy = new EnemyRobot(data, player_);
		}
		break;
	}

	enemy->Init();
	enemies_.emplace_back(enemy);
	return enemy;
}

void EnemyManager::Update(void)
{
	if (enemies_.empty()) { return; }
	for (auto& enemy : enemies_)
	{
		enemy->Update();
	}
}

void EnemyManager::Draw(void)
{
	if (enemies_.empty()) { return; }
	for (auto& enemy : enemies_)
	{
		enemy->Draw();
	}
}

void EnemyManager::Release(void)
{
	if (enemies_.empty()) { return; }
	for (auto& enemy : enemies_)
	{
		enemy->Release();
		delete enemy;
	}
}


void EnemyManager::AddHitCollider(const ColliderBase* hitCollider)
{
	if (enemies_.empty()) { return; }
	for (auto& enemy : enemies_)
	{
		enemy->AddHitCollider(hitCollider);
	}
}
