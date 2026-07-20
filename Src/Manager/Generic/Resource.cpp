#include "Resource.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>

Resource::Resource(void)
	: resType_(LOAD_TYPE::NONE)
	, path_("")
	, allNum_(0), numX_(0), numY_(0)
	, handleId_(-1), handleIds_{}
{
}

Resource::Resource(LOAD_TYPE _type, const std::string& _path)
	: resType_(_type)
	, path_(_path)
	, allNum_(0), numX_(0), numY_(0)
	, handleId_(-1), handleIds_{}
{
}

Resource::Resource(LOAD_TYPE _type, const std::string& _path
				   , int _allNum, int _numX, int _numY)
	: resType_(_type)
	, path_(_path)
	, allNum_(_allNum), numX_(_numX), numY_(_numY)
	, handleId_(-1), handleIds_(_allNum, -1)
{
	
}



void Resource::Load(void)
{
	/* リソース読み込み処理 */

	switch (resType_)
	{
		case LOAD_TYPE::IMAGE: case LOAD_TYPE::MOVIE:
			Load_ImageAndMovie();
		break;

		case LOAD_TYPE::IMAGES:
			Load_Images();
		break;

		case LOAD_TYPE::MODEL: case LOAD_TYPE::ANIM:
			Load_ModelAndAnim();
		break;
		
		case LOAD_TYPE::EFFECT:
			Load_Effect();
		break;

		case LOAD_TYPE::SOUND:
			Load_Sound();
		break;
	}

}

void Resource::Load_ImageAndMovie(void)
{
	/* 単一画像・映像ファイルの読み込み */
	handleId_ = LoadGraph(path_.c_str());
}
void Resource::Load_Images(void)
{
	/* 複数画像の読み込み */
	int handle = LoadGraph(path_.c_str());
	int sizeX, sizeY = 0;

	// 画像単体のサイズを取得
	GetGraphSize(handle, &sizeX, &sizeY);
	sizeX /= numX_;
	sizeY /= numY_;

	LoadDivGraph(path_.c_str(),
		allNum_, numX_, numY_,
		sizeX, sizeY,
		(&handleIds_[0]));
}
void Resource::Load_ModelAndAnim(void)
{
	/* 3Dモデル・アニメーションファイルの読み込み */
	handleId_ = MV1LoadModel(path_.c_str());
}
void Resource::Load_Effect(void)
{
	/* エフェクトの読み込み */
	// 1. 現在の非同期フラグの状態を一時的に保存しつつ、一時的に「同期読み込み(false)」にする
	int prevAsyncFlag = GetUseASyncLoadFlag();
	SetUseASyncLoadFlag(FALSE);

	// 2. 安全にその場でエフェクトを読み込む
	handleId_ = LoadEffekseerEffect(path_.c_str());

	// 3. 読み込みが終わったら、元の非同期フラグの状態に戻す
	SetUseASyncLoadFlag(prevAsyncFlag);
}
void Resource::Load_Sound(void)
{
	/* 音声ファイルの読み込み */
	handleId_ = LoadSoundMem(path_.c_str());
}


void Resource::Release(void)
{
	/* リソース解放処理 */

	switch (resType_)
	{
		case LOAD_TYPE::IMAGE: case LOAD_TYPE::MOVIE:
			Release_ImageAndMovie();
		break;

		case LOAD_TYPE::IMAGES:
			Release_Images();
		break;

		case LOAD_TYPE::MODEL: case LOAD_TYPE::ANIM:
			Release_ModelAndAnim();
		break;

		case LOAD_TYPE::EFFECT:
			Release_Effect();
		break;

		case LOAD_TYPE::SOUND:
			Release_Sound();
		break;
	}
}
void Resource::Release_ImageAndMovie(void)
{
	/* 単一画像・映像ファイルの解放 */
	DeleteGraph(handleId_);
}
void Resource::Release_Images(void)
{
	/* 複数画像の解放 */
	for (int& handle : handleIds_)
	{
		DeleteGraph(handle);
	}
}
void Resource::Release_ModelAndAnim(void)
{
	/* 3Dモデル・アニメーションファイルの解放 */
	MV1DeleteModel(handleId_);

	auto& ids = duplicationModelIds_;

	for (auto& id : ids)
	{
#if _DEBUG
		if (id == -1)
		{
			OutputDebugString("\nモデルが未割当です\n");
		}
#endif
		MV1DeleteModel(id);
	}
}
void Resource::Release_Effect(void)
{
	/* エフェクトの解放 */
	DeleteEffekseerEffect(handleId_);
}
void Resource::Release_Sound(void)
{
	/* 音声ファイルの解放 */
	if (CheckSoundMem(handleId_) == 1)
	{
		// 再生時、再生中の音声を停止
		StopSoundMem(handleId_);
	}

	// 音声ファイルを解放
	DeleteSoundMem(handleId_);
}


void Resource::CopyHandle(int* _images)
{
	/* ハンドルコピー処理 */
	for (int i = 0; i < allNum_; i++)
	{
		_images[i] = handleIds_[i];
	}
}

void Resource::SetDuplicateModelId(int _id)
{
	// IDをリストの末尾に追加
	duplicationModelIds_.push_back(_id);
}

int Resource::GetHandleImagesId(int _imageNum) const
{
	int handle = -1;
	if (_imageNum >= handleIds_.size() ||
		_imageNum < 0)
	{
		return handle;
	}

	handle = handleIds_[_imageNum];

	return handle;
}
