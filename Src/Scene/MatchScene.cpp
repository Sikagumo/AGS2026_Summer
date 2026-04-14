#include<string>

#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/NetManager.h"
#include "../Net/NetStructures.h"
#include "../Common/InputTextArea.h"
#include "MatchScene.h"

MatchScene::MatchScene(void)
	: inputTextArea1_(nullptr),
	inputTextArea2_(nullptr),
	inputTextArea3_(nullptr), 
	inputTextArea4_(nullptr)
{
}

MatchScene::~MatchScene(void)
{
}

void MatchScene::Init(void)
{
	// 入力エリア
	inputTextArea1_ = new InputTextArea(
		{ B1_E_POS.x + 10, B2_S_POS.y }, { 50, HEIGHT }, 3);
	inputTextArea2_ = new InputTextArea(
		{ B1_E_POS.x + 70, B2_S_POS.y }, { 50, HEIGHT }, 3);
	inputTextArea3_ = new InputTextArea(
		{ B1_E_POS.x + 130, B2_S_POS.y }, { 50, HEIGHT }, 3);
	inputTextArea4_ = new InputTextArea(
		{ B1_E_POS.x + 190, B2_S_POS.y }, { 50, HEIGHT }, 3);

	// デフォルトのホストIPアドレス
	auto hostIp = NetManager::GetInstance().GetHostIp();
	inputTextArea1_->SetText(std::to_string(hostIp.d1));
	inputTextArea2_->SetText(std::to_string(hostIp.d2));
	inputTextArea3_->SetText(std::to_string(hostIp.d3));
	inputTextArea4_->SetText(std::to_string(hostIp.d4));
}

void MatchScene::Update(void)
{
	auto& ins = InputManager::GetInstance();
	if (ins.IsClickMouseLeft())
	{



		Vector2 moPos = ins.GetMousePos();

		// ホスト
		if (B1_S_POS.x <= moPos.x && B1_E_POS.x >= moPos.x
			&& B1_S_POS.y <= moPos.y && B1_E_POS.y >= moPos.y)
		{
			NetManager::GetInstance().Run(NET_MODE::HOST);
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CONNECT);
		}

		// クライアント
		if (B2_S_POS.x <= moPos.x && B2_E_POS.x >= moPos.x
			&& B2_S_POS.y <= moPos.y && B2_E_POS.y >= moPos.y)
		{
			IPDATA hostIp;
			hostIp.d1 = std::stoi(inputTextArea1_->GetText());
			hostIp.d2 = std::stoi(inputTextArea2_->GetText());
			hostIp.d3 = std::stoi(inputTextArea3_->GetText());
			hostIp.d4 = std::stoi(inputTextArea4_->GetText());
			NetManager::GetInstance().SetHostIp(hostIp);
			NetManager::GetInstance().Run(NET_MODE::CLIENT);
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CONNECT);
		}

	}

	inputTextArea1_->Update();
	inputTextArea2_->Update();
	inputTextArea3_->Update();
	inputTextArea4_->Update();
}

void MatchScene::Draw(void)
{
	// ネットワーク情報背景
	DrawBox(
		0, B1_S_POS.y - 10,
		Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);

	// ホスト
	DrawBox(B1_S_POS.x, B1_S_POS.y, B1_E_POS.x, B1_E_POS.y, 0xffffff, false);
	DrawString(B1_S_POS.x + 50, B1_S_POS.y + 7, "HOST", 0xffffff);

	// クライアント
	DrawBox(B2_S_POS.x, B2_S_POS.y, B2_E_POS.x, B2_E_POS.y, 0xffffff, false);
	DrawString(B2_S_POS.x + 50, B2_S_POS.y + 7, "CLIENT", 0xffffff);

	// ホストIPアドレス
	DrawBox(B1_E_POS.x + 10, B1_S_POS.y, B1_E_POS.x + 30, B1_E_POS.y, 0x000000, true);
	DrawString(B1_E_POS.x + 12, B1_S_POS.y + 12, "ホストのIPアドレス", 0xffffff);


	inputTextArea1_->Draw();
	inputTextArea2_->Draw();
	inputTextArea3_->Draw();
	inputTextArea4_->Draw();


	
	//DrawRotaGraph(
	//	Application::SCREEN_SIZE_X / 2,
	//	Application::SCREEN_SIZE_Y / 2, 
	//	1.0, angle_, imgSpot_, true);

	


	

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawFormatString(10, 10, 0xffffff, "Match SCENE");
}

void MatchScene::Release(void)
{
	inputTextArea1_->Release();
	delete inputTextArea1_;
	inputTextArea2_->Release();
	delete inputTextArea2_;
	inputTextArea3_->Release();
	delete inputTextArea3_;
	inputTextArea4_->Release();
	delete inputTextArea4_;
}
