#include <DxLib.h>
#include "../../Utility/AsoUtility.h"
#include "../../Utility/MatrixUtility.h"
#include "Transform.h"

Transform::Transform(void)
	:
	modelId(-1),
	scl(AsoUtility::VECTOR_ONE),
	rot(AsoUtility::VECTOR_ZERO),
	pos(AsoUtility::VECTOR_ZERO),
	localPos(AsoUtility::VECTOR_ZERO),
	prePos(AsoUtility::VECTOR_ZERO),
	matScl(MGetIdent()),
	matRot(MGetIdent()),
	matPos(MGetIdent()),
	quaRot(Quaternion().Identity()),
	quaRotLocal(Quaternion().Identity())

{
}


void Transform::Update(void)
{
	// 大きさ
	matScl = MGetScale(scl);

	// 回転
	rot = quaRot.ToEuler();
	matRot = quaRot.ToMatrix();

	// 位置
	matPos = MGetTranslate(pos);

	// 行列の合成
	MATRIX mat = MGetIdent();
	mat = MMult(mat, matScl);
	Quaternion q = quaRot.Mult(quaRotLocal);
	mat = MMult(mat, q.ToMatrix());
	mat = MMult(mat, matPos);

	// 行列をモデルに判定
	if (modelId != -1)
	{
		MV1SetMatrix(modelId, mat);
	}

}

void Transform::DrawModelDir(void)
{
#ifdef _DEBUG
	SetUseLighting(FALSE);
	const float DRAW_OFFSET = 65.0f;
	const float DRAW_OFFSET_LOCAL = 25.0f;
	const float DRAW_RADIUS = 10.0f;
	const float DRAW_RADIUS_LOCAL = 15.0f;
	const int DRAW_DIM = 10;
	const unsigned int SPEC_COLOR = 0x555555;
	VECTOR drawPos;
	VECTOR drawPosLocal;
	unsigned int drawColor;

	// X軸
	drawPos = VAdd(pos, VScale(GetRight(), DRAW_OFFSET));
	drawColor = 0xaa0000;
	DrawSphere3D(drawPos, DRAW_RADIUS, DRAW_DIM, drawColor, SPEC_COLOR, true);
	DrawLine3D(pos, drawPos, drawColor);

	// Y軸
	drawPos = VAdd(pos, VScale(GetUp(), DRAW_OFFSET));
	drawColor = 0x00aa00;
	DrawSphere3D(drawPos, DRAW_RADIUS, DRAW_DIM, drawColor, SPEC_COLOR, true);
	DrawLine3D(pos, drawPos, drawColor);

	// Z軸
	drawPos = VAdd(pos, VScale(GetForward(), DRAW_OFFSET));
	drawColor = 0x0000aa;
	DrawSphere3D(drawPos, DRAW_RADIUS, DRAW_DIM, drawColor, SPEC_COLOR, true);
	DrawLine3D(pos, drawPos, drawColor);

	SetUseLighting(TRUE);
#endif
}

void Transform::Release(void)
{
}

void Transform::SetModel(int model)
{
	modelId = model;
}

void Transform::InitTransform(const VECTOR& _scl, const Quaternion& _rot, const Quaternion& _rotLocal, const VECTOR& _pos)
{
	scl = _scl;
	quaRot = _rot;
	quaRotLocal = _rotLocal;
	pos = _pos;

	Update();
}
void Transform::InitTransform(float _scl, const Quaternion& _rot, const Quaternion& _rotLocal, const VECTOR& _pos)
{
	InitTransform({ _scl,_scl,_scl }, _rot, _rotLocal, _pos);
}
void Transform::InitTransform(float _scl, const Quaternion& _rot, const Quaternion& _rotLocal)
{
	scl = VGet(_scl, _scl, _scl);
	quaRot = _rot;
	quaRotLocal = _rotLocal;

	Update();
}
void Transform::InitTransform(void)
{
	InitTransform(AsoUtility::VECTOR_ONE,
				  Quaternion::Identity(), Quaternion::Identity(),
				  AsoUtility::VECTOR_ZERO);
}


void Transform::Translate(const VECTOR& _movePow)
{
	pos = VAdd(pos, _movePow);
	Update();
}

void Transform::Translate(const VECTOR& _dir, float _movePow)
{
	// 正規化して移動
	VECTOR dir = AsoUtility::VNormalize(_dir);
	VECTOR moveVec = VScale(dir, _movePow);
	Translate(moveVec);
}

void Transform::Rotate(const Quaternion& rot)
{
	quaRot = Quaternion::Mult(quaRot, rot);
	Update();
}
void Transform::Rotate(const VECTOR& _axis, float _pow)
{
	Quaternion qua = Quaternion::AngleAxis(_pow, _axis);
	Rotate(qua);
}

VECTOR Transform::GetForward(void) const
{
	return GetDir(AsoUtility::DIR_F);
}

VECTOR Transform::GetBack(void) const
{
	return GetDir(AsoUtility::DIR_B);
}

VECTOR Transform::GetRight(void) const
{
	return GetDir(AsoUtility::DIR_R);
}

VECTOR Transform::GetLeft(void) const
{
	return GetDir(AsoUtility::DIR_L);
}

VECTOR Transform::GetUp(void) const
{
	return GetDir(AsoUtility::DIR_UP);
}

VECTOR Transform::GetDown(void) const
{
	return GetDir(AsoUtility::DIR_DOWN);
}

VECTOR Transform::GetDir(const VECTOR& dir) const
{
	return quaRot.PosAxis(dir);
}
