#include "CollisionComponent.h"
#include "Actor.h"

CollisionComponent::CollisionComponent(class Actor* owner)
:Component(owner)
,mWidth(0.0f)
,mHeight(0.0f)
,mDepth(0.0f)
{
	
}

CollisionComponent::~CollisionComponent()
{
	
}

bool CollisionComponent::Intersect(const CollisionComponent* other)
{
	Vector3 aMin = GetMin();
	Vector3 aMax = GetMax();
	Vector3 bMin = other->GetMin();
	Vector3 bMax = other->GetMax();

	bool no = aMax.x < bMin.x ||
		aMax.y < bMin.y ||
		aMax.z < bMin.z ||
		bMax.x < aMin.x ||
		bMax.y < aMin.y ||
		bMax.z < aMin.z;

	return !no;
}

Vector3 CollisionComponent::GetMin() const
{
	Vector3 v = mOwner->GetPosition();
	v.x -= mDepth * mOwner->GetScale() / 2.0f;
	v.y -= mWidth * mOwner->GetScale() / 2.0f;
	v.z -= mHeight * mOwner->GetScale() / 2.0f;
	return v;
}

Vector3 CollisionComponent::GetMax() const
{
	Vector3 v = mOwner->GetPosition();
	v.x += mDepth * mOwner->GetScale() / 2.0f;
	v.y += mWidth * mOwner->GetScale() / 2.0f;
	v.z += mHeight * mOwner->GetScale() / 2.0f;
	return v;
}

const Vector3& CollisionComponent::GetCenter() const
{
	return mOwner->GetPosition();
}

CollSide CollisionComponent::GetMinOverlap(
	const CollisionComponent* other, Vector3& offset)
{
	offset = Vector3::Zero;
    if (this->Intersect(other) == false) {
        return CollSide::None;
    }
    else {
        float dy1 = other->GetMin().y - this->GetMax().y;
        float dy2 = other->GetMax().y - this->GetMin().y;
        float dx1 = other->GetMin().x - this->GetMax().x;
        float dx2 = other->GetMax().x - this->GetMin().x;
        float dz1 = other->GetMin().z - this->GetMax().z;
        float dz2 = other->GetMax().z - this->GetMin().z;
        if (abs(dz1)<=abs(dz2) && abs(dz1)<=abs(dy1) && abs(dz1)<=abs(dx1) && abs(dz1)<=abs(dy2) && abs(dz1)<=abs(dx2)) {
            offset = Vector3(0,0,dz1);
            return CollSide::Bottom;
        }
        else if (abs(dz1)>=abs(dz2) && abs(dz2)<=abs(dy2) && abs(dz2)<=abs(dx2) && abs(dz2)<=abs(dy1) && abs(dz2)<=abs(dx1)) {
            offset = Vector3(0,0,dz2);
            return CollSide::Top;
        }
        else {
            if (abs(dx1)<=abs(dy1) && abs(dx1)<=abs(dy2) && abs(dx1)<=abs(dx2)) {
                offset = Vector3(dx1,0,0);
                return CollSide::SideX1;
            }
            else if (abs(dx2)<=abs(dy1) && abs(dx2)<=abs(dy2) && abs(dx2)<=abs(dx1)){
                offset = Vector3(dx2,0,0);
                return CollSide::SideX2;
            }
            else if (abs(dy1)<=abs(dy2) && abs(dy1)<=abs(dx1) && abs(dy1)<=abs(dx2)) {
                offset = Vector3(0,dy1,0);
                return CollSide::SideY1;
            }
//            else if (abs(dy2)<=abs(dy1) && abs(dy2)<=abs(dx1) && abs(dy2)<=abs(dx2)) {
            else {
                offset = Vector3(0,dy2,0);
                return CollSide::SideY2;
            }
        }
    }
}
