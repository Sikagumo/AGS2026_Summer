//==============================================================
// rain_ps.hlsl
// 雨が降っているように見えるピクセルシェーダー(背景不要版)
// - 疑似乱数によるストリーク(雨筋)のみを計算し、アルファ値付きで出力する
// - 手前層/奥層の2レイヤーを重ねて奥行き感を出す
// - 出力の alpha はそのまま雨の不透明度なので、
//   呼び出し側でアルファブレンド(加算 or 通常)して任意の背景に重ねて使う
//==============================================================

#include "../Common/Pixel/PixelShader2DHeader.hlsli"

// ----- 定数バッファ -----
cbuffer RainCB : register(b4)
{
    float2 g_resolution;   // 画面解像度 (px)
    float  g_time;         // 経過時間 (秒)
    float dummy1;
    
    float g_intensity; // 雨の強さ 0.0～1.0
    float g_intensityBack; // 後ろのレイヤーの雨の強さ 0.0～1.0
    float dummy2;
    float dummy3;
    
    float4 g_rainColor;
};

// 0～1の疑似乱数 (整数座標から生成)
float Hash21(float2 p)
{
    // 乱数の係数のハッシュ値
    const float RAND_COEF_1 = 123.34f;
    const float RAND_COEF_2 = 456.21f;
    
    // 乱数の加算オフセット値
    const float RAND_COEF_OFFSET = 45.32f;
    
    p = frac(p * float2(RAND_COEF_1, RAND_COEF_1));
    p += dot(p, p + RAND_COEF_OFFSET);
    return frac(p.x * p.y);
}

// 1レイヤー分の雨筋を計算する
// columns : 縦の列数(密度)
// speed   : 落下速度
// streak  : 1本の筋の長さ(0?1、大きいほど長い)
// widthPx : 筋の太さ(ピクセル)
float RainLayer(float2 uv, float columns, float speed, float streakLen, float widthPx)
{
    // 列インデックス
    float colF = uv.x * columns;
    float colId = floor(colF);
    float colFrac = frac(colF);

    // 列ごとにランダムな横位置オフセット・速度・位相をつける
    float rand = Hash21(float2(colId, 0.0));
    float xOffset = (rand - 0.5) * (1.0 / columns) * 0.6;
    float colSpeed = speed * (0.6 + rand * 0.8);
    float phase = Hash21(float2(colId, 1.0));

    // 縦方向にループするスクロール座標 (0～1)
    float y = frac(uv.y + phase - g_time * colSpeed);

    // 筋の中心からの距離(y方向、上端が濃く下に伸びるイメージ)
    float streakMask = smoothstep(streakLen, streakLen * 0.5, y);

    // 横方向の太さ判定
    float distX = abs(colFrac - 0.5 + xOffset * columns);
    float pxWidth = widthPx / g_resolution.x * columns;
    float lineMask = smoothstep(pxWidth, 0.0, distX);

    // 列ごとに出現/非表示をランダムに間引く(まばらな雨粒感)
    float visible = step(0.35f, Hash21(float2(colId, floor(g_time * 0.5 + phase * 10.0))));

    return lineMask * streakMask * visible;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.uv;

    // 奥のレイヤー(細く速く、密度高め、暗め)
    float back = RainLayer(uv, 140.0, 1.6, 0.55, 1.0) * g_intensityBack;

    // 手前のレイヤー(太く速く、密度低め、明るめ)
    float front = RainLayer(uv * float2(1.0, 1.0) + float2(0.37, 0.0), 60.0, 2.4, 0.35, 2.0);

    float rain = saturate(back + front) * g_intensity;

    return float4(g_rainColor.rgb, rain * 0.5f);
}
