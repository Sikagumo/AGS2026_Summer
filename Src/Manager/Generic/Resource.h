#pragma once
#include <string>
#include <vector>

class Resource
{
public:

	enum class LOAD_TYPE
	{
		NONE = -1,

		IMAGE,  // 単一画像
		IMAGES, // 複数画像
		MODEL,  // 3Dモデル
		MOVIE,  // 映像ファイル
		ANIM,   // アニメーションファイル
		EFFECT, // エフェクト
		SOUND,  // 音声
		VERTEX_SHADER, // 頂点シェーダ
		PIXEL_SHADER,  // ピクセルシェーダ
	};


	Resource(void);

	/// @brief コンストラクタ
	/// @param _type 読み込むファイルの種類
	/// @param _path ファイルパス
	Resource(LOAD_TYPE _type, const std::string &_path);

	/// @brief 複数画像時のコンストラクタ
	/// @param _type 読み込むファイルの種類
	/// @param _path ファイルパス
	/// @param _allNum 画像数
	/// @param _numX 横画像数
	/// @param _numY 縦画像数
	Resource(LOAD_TYPE _type, const std::string& _path
			 , int _allNum, int _numX, int _numY);

	~Resource(void) = default;


	void Load(void);
	void Release(void);

	/// @brief ハンドルコピー処理
	/// @param _images 画像ハンドル
	void CopyHandle(int* _images);

	/// @brief 重複させる3DモデルID割り当て処理
	void SetDuplicateModelId(int _id);

	/// @brief リソースのIDを取得
	int GetHandleId(void)const { return handleId_; };

	/// @brief 複数画像の個別取得
	/// @param 取得する画像番号
	int GetHandleImagesId(int _imageNum)const;

	/// @brief リソースのハンドルパスを取得 
	std::string GetHandlePath(void)const { return path_; };

	/// @brief 読み込むファイルの対象の種類の取得処理 
	LOAD_TYPE GetLoadType(void)const { return resType_; };


private:

	// 読み込むファイルの種類
	LOAD_TYPE resType_;

	// 読み込み先
	std::string path_;
	
	// 単一読み込みハンドルID
	int handleId_;


	/* 複数画像読み込み用 */

	// 画像数
	const int allNum_;

	// 軸別の最大画像数
	const int numX_;
	const int numY_;


	// 複数画像のハンドルID
	std::vector<int> handleIds_;

	// モデル複製用リスト
	std::vector<int> duplicationModelIds_;


	/* 読み込み処理 */
	void Load_ImageAndMovie(void);
	void Load_Images(void);
	void Load_ModelAndAnim(void);
	void Load_Effect(void);
	void Load_Sound(void);
	void Load_VertexShader(void);
	void Load_PixelShader(void);

	/* 解放処理 */
	void Release_ImageAndMovie(void)const;
	void Release_Images(void);
	void Release_ModelAndAnim(void)const;
	void Release_Effect(void)const;
	void Release_Sound(void)const;
	void Release_VertexShader(void)const;
	void Release_PixelShader(void)const;
};