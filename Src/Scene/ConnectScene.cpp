#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/NetManager.h"
#include "../Net/NetStructures.h"
#include "ConnectScene.h"

ConnectScene::ConnectScene(void)
{
	imgStart_ = -1;
	imgTitle_ = -1;
}

ConnectScene::~ConnectScene(void)
{
}

void ConnectScene::Init(void)
{
	//NetManager::GetInstance().TryConnect();
	netMng_.ChangeGameState(GAME_STATE::CONNECTING);
}

void ConnectScene::Update(void)
{

	
	if (netMng_.GetMode() == NET_MODE::HOST)
	{

		auto& players = netMng_.GetNetUsers();
		if (players.size() > 1)
		{

			auto& ins = InputManager::GetInstance();
			if (ins.IsClickMouseLeft())
			{

				Vector2 moPos = ins.GetMousePos();

				if (B1_S_POS.x <= moPos.x && B1_E_POS.x >= moPos.x
					&& B1_S_POS.y <= moPos.y && B1_E_POS.y >= moPos.y)
				{
					//NetManager::GetInstance().TryGoToGame();
					netMng_.ChangeGameState(GAME_STATE::GOTO_GAME);
				}

			}

		}

	}

	if (netMng_.IsSameGameState(GAME_STATE::GOTO_GAME))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}

}

void ConnectScene::Draw(void)
{
	DrawFormatString(10, 10, 0xffffff, "Connect SCENE");

	int HX = Application::SCREEN_SIZE_X / 2;
	int HY = Application::SCREEN_SIZE_Y / 2;

	std::string msg = "ê⁄ë±íÜ";
	if (NetManager::GetInstance().GetMode() == NET_MODE::HOST)
	{
		msg = "ê⁄ë±ë“Çø";
	}
	DrawString(HX - 100, HY, msg.c_str(), 0xffffff);

	int y = HY + 50;
	auto& players = NetManager::GetInstance().GetNetUsers();
	for (const auto& users : players)
	{

		IPDATA ip = users.second.ip;

		std::string out = "";
		out += std::to_string(users.second.key) + " ";
		out += std::to_string(ip.d1);
		out += ".";
		out += std::to_string(ip.d2);
		out += ".";
		out += std::to_string(ip.d3);
		out += ".";
		out += std::to_string(ip.d4);
		out += " : " + std::to_string(users.second.port);

		DrawString(HX - 100, y, out.c_str(), 0xffffff);
		y += 20;

	}

	if (NetManager::GetInstance().GetMode() == NET_MODE::HOST)
	{
		auto& players = NetManager::GetInstance().GetNetUsers();
		if (players.size() > 1)
		{
			DrawBox(B1_S_POS.x, B1_S_POS.y, B1_E_POS.x, B1_E_POS.y, 0x000000, true);
			DrawString(B1_S_POS.x + 50, B1_S_POS.y + 5, "êiÇﬁ", 0xffffff);
		}
	}

}

void ConnectScene::Release(void)
{
}
