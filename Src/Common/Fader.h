#pragma once

#include <string>

/// @brief フェード処理クラス
class Fader
{

public:

    // フェードが進む速さ
    static constexpr float SPEED_ALPHA = 4.0f;

    // 状態
    enum class STATE
    {
        NONE,
        FADE_OUT,   // 徐々に暗転
        FADE_IN     // 徐々に明転
    };

    /// @brief コンストラクタ
    Fader(void);

    /// @brief デストラクタ
    ~Fader(void);

    /// @brief 状態の取得
    /// @return 現在のフェード状態
    STATE GetState(void) const;

    /// @brief フェード処理が終了しているか
    /// @return 終了している場合はtrue
    bool IsEnd(void) const;

    /// @brief 指定フェードを開始する
    /// @param _state 開始するフェードの状態
    void SetFade(STATE _state);

    /// @brief 初期化
    void Init(void);

    /// @brief 更新
    void Update(void);

    /// @brief 描画
    void Draw(void);

    /// @brief 初回用のフェード画像を読み込む
    void LoadFadeImage(void);

private:

    // 状態関連
    STATE state_;   // 状態
    float alpha_;   // 透明度
    bool isPreEnd_; // 1フレーム判定用
    bool isEnd_;    // フェード処理の終了判定

    // フェードに使用する画像ハンドル
    int fadeImageHandle_;

};