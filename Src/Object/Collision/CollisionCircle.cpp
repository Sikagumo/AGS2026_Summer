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

	const auto* CIRCLE_A = static_cast<const Collider2DCircle*>(_circleA); 
	const auto* CIRCLE_B = static_cast<const Collider2DCircle*>(_circleB); 

	if (CIRCLE_A == nullptr || CIRCLE_B == nullptr)
	{
		return false;
	}

	const Vector2F POS_A = CIRCLE_A->GetWorldCenterPos(); 
	const Vector2F POS_B = CIRCLE_B->GetWorldCenterPos(); 

	const float DIST_X = POS_B.x - POS_A.x;                                 
	const float DIST_Y = POS_B.y - POS_A.y;                                 
	const float DIST_SQUARE = (DIST_X * DIST_X) + (DIST_Y * DIST_Y);       

	const float RADIUS_SUM = CIRCLE_A->GetRadius() + CIRCLE_B->GetRadius(); 
	const float RADIUS_SUM_SQUARE = RADIUS_SUM * RADIUS_SUM; 

	if (DIST_SQUARE < RADIUS_SUM_SQUARE)
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

	const auto* CIRCLE = static_cast<const Collider2DCircle*>(_circle); 
	if (CIRCLE == nullptr)
	{
		CIRCLE = static_cast<const Collider2DCircle*>(_box);
	}

	const auto* BOX = static_cast<const Collider2DBox*>(_box);          
	if (BOX == nullptr)
	{
		BOX = static_cast<const Collider2DBox*>(_circle);
	}

	if (CIRCLE == nullptr || BOX == nullptr)
	{
		return false;
	}

	const Vector2F CIRCLE_POS = CIRCLE->GetWorldCenterPos(); 
	const Vector2F BOX_LEFT_TOP = BOX->GetLeftTop();         
	const Vector2F BOX_RIGHT_BOTTOM = BOX->GetRightBottom(); 

	// ‹éŒ`“à‚Å‰~‚Ì’†S‚ÉÅ‚à‹ß‚¢“_‚ðŽZo
	const float CLOSEST_X = std::max(BOX_LEFT_TOP.x, std::min(CIRCLE_POS.x, BOX_RIGHT_BOTTOM.x));
	const float CLOSEST_Y = std::max(BOX_LEFT_TOP.y, std::min(CIRCLE_POS.y, BOX_RIGHT_BOTTOM.y));

	const float DIST_X = CIRCLE_POS.x - CLOSEST_X;                    
	const float DIST_Y = CIRCLE_POS.y - CLOSEST_Y;                    
	const float DIST_SQUARE = (DIST_X * DIST_X) + (DIST_Y * DIST_Y);  

	const float RADIUS_SQUARE = CIRCLE->GetRadius() * CIRCLE->GetRadius();

	if (DIST_SQUARE < RADIUS_SQUARE)
	{
		return true;
	}

	return false;
}