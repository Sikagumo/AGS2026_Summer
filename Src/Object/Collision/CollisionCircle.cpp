#define NOMINMAX
#include "CollisionCircle.h"
#include <cmath>
#include <algorithm>
#include "../Collider2D/Collider2DCircle.h"
#include "../Collider2D/Collider2DBox.h"
#include "../../Common/Vector2.h"

bool CollisionCircle::CheckCircleVsCircle(const Collider2DBase* _circleA, const Collider2DBase* _circleB)
{
	if (!_circleA || !_circleB)
	{
		return false;
	}

	const auto* circleA = static_cast<const Collider2DCircle*>(_circleA);
	const auto* circleB = static_cast<const Collider2DCircle*>(_circleB);

	if (circleA == nullptr || circleB == nullptr)
	{
		return false;
	}

	const Vector2F posA = circleA->GetWorldCenterPos();
	const Vector2F posB = circleB->GetWorldCenterPos();

	const float distX = posB.x - posA.x;
	const float distY = posB.y - posA.y;
	const float distSquare = (distX * distX) + (distY * distY);

	const float radiusSum = circleA->GetRadius() + circleB->GetRadius();
	const float radiusSumSquare = radiusSum * radiusSum;

	if (distSquare < radiusSumSquare)
	{
		return true;
	}

	return false;
}

bool CollisionCircle::CheckCircleVsBox(const Collider2DBase* _circle, const Collider2DBase* _box)
{
	if (!_circle || !_box)
	{
		return false;
	}

	const auto* circle = static_cast<const Collider2DCircle*>(_circle);
	if (circle == nullptr)
	{
		circle = static_cast<const Collider2DCircle*>(_box);
	}

	const auto* box = static_cast<const Collider2DBox*>(_box);
	if (box == nullptr)
	{
		box = static_cast<const Collider2DBox*>(_circle);
	}

	if (circle == nullptr || box == nullptr)
	{
		return false;
	}

	const Vector2F circlePos = circle->GetWorldCenterPos();
	const Vector2F boxLeftTop = box->GetLeftTop();
	const Vector2F boxRightBottom = box->GetRightBottom();

	// ‹éŒ`“à‚Å‰~‚Ì’†S‚ÉÅ‚à‹ß‚¢“_‚ðŽZo
	const float closestX = std::max(boxLeftTop.x, std::min(circlePos.x, boxRightBottom.x));
	const float closestY = std::max(boxLeftTop.y, std::min(circlePos.y, boxRightBottom.y));

	const float distX = circlePos.x - closestX;
	const float distY = circlePos.y - closestY;
	const float distSquare = (distX * distX) + (distY * distY);

	const float radiusSquare = circle->GetRadius() * circle->GetRadius();

	if (distSquare < radiusSquare)
	{
		return true;
	}

	return false;
}