#pragma once
#include "MeshComponent.h"
#include "SegmentCast.h"
#include <vector>
#include "Math.h"

class LaserComponent : public MeshComponent
{
public:
    LaserComponent(class Actor* owner);
    void Update(float deltaTime) override;
    void Draw(class Shader* shader) override;
    Matrix4 LineToMatrix(LineSegment line);
private:
    LineSegment mLineSegment;
    std::vector<LineSegment> mLineSegVect;
};
