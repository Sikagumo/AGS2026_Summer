#pragma once
class Vector2
{
public:

	int x; // X座標
	int y; // Y座標

	// デストラクタ
	~Vector2(void) = default;

	// 代入処理
	Vector2 operator=(const Vector2& vec);

	// 加算処理
	Vector2 operator+(const Vector2& vec)const;
	void operator+=(const Vector2& vec);

	// 減算処理
	Vector2 operator-(const Vector2& vec)const;
	void operator-=(const Vector2& vec);

	// 乗算処理
	Vector2 operator*(const Vector2& vec)const;
	void operator*=(const Vector2& vec);

	// 除算処理
	Vector2 operator/(const Vector2& vec)const;
	void operator/=(const Vector2& vec);
};