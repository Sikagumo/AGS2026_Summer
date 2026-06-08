#include "CollisionBox.h"

#include "../Collider2D/Collider2DBox.h"

bool CollisionBox::CheckBoxVsBox(const Collider2DBase* _boxA, const Collider2DBase* _boxB)
{
	if (!_boxA || !_boxB) { return false; }

	const auto* boxA = dynamic_cast<const Collider2DBox*>(_boxA);
	const auto* boxB = dynamic_cast<const Collider2DBox*>(_boxB);

	if (boxA == nullptr || boxB == nullptr) { return false; }

	Vector2F leftTopA = boxA->GetLeftTop();
	Vector2F rightBottomA = boxA->GetRightBottom();

	Vector2F leftTopB = boxB->GetLeftTop();
	Vector2F rightBottomB = boxB->GetRightBottom();

	if (rightBottomA.x < leftTopB.x) { return false; }
	if (leftTopA.x > rightBottomB.x) { return false; }
	if (rightBottomA.y < leftTopB.y) { return false; }
	if (leftTopA.y > rightBottomB.y) { return false; }

	return true;
}