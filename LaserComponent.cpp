#include "LaserComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Game.h"
#include "Player.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "SegmentCast.h"
#include "PlayerMove.h"
#include "Block.h"
#include <iostream>

LaserComponent::LaserComponent(class Actor* owner)
:MeshComponent(owner, true)
{
    SetMesh(mOwner->GetGame()->GetRenderer()->GetMesh("Assets/Laser.gpmesh"));
}

void LaserComponent::Update(float deltaTime)
{
    // Set vector
    mLineSegVect.clear();

    CastInfo outInfo;
    Vector3 position = mOwner->GetWorldPosition();
    Vector3 fwd = mOwner->GetWorldForward();
    Actor* ignoreActor = mOwner->mParent;
    bool noReflection = false;
    
    while (!noReflection) {
        LineSegment lineSeg;
        
        // Set line segment variables
        lineSeg.mStart = position;
        lineSeg.mEnd = position + fwd * 500;
        
        // Does the laser hit the player?
        if (SegmentCast(mOwner->GetGame()->mPlayer, lineSeg, outInfo)) {
            lineSeg.mEnd = outInfo.mPoint;
            noReflection = true;
            Mix_Chunk *sample = mOwner->GetGame()->GetSound("Assets/Sounds/Laser.wav");
            if(Mix_PlayChannel(-1, sample, 0)==-1) {
                printf("Mix_PlayChannel: %s\n",Mix_GetError());
            }
            mOwner->GetGame()->mPlayer->GetComponent<PlayerMove>()->Respawn();
        }
        else {
            // Does the laser hit a block?
            if (SegmentCast(mOwner->GetGame()->GetBlocks(), lineSeg, outInfo, ignoreActor)) {
                lineSeg.mEnd = outInfo.mPoint;
                // Is the block a mirror?
                if (((Block*)outInfo.mActor)->GetMirror()) {
                    // Start point is the point you just intersected at
                    position = outInfo.mPoint;
                    // The forward vector is the old forward vector reflected off the mNormal you get from the castInfo
                    fwd = Vector3::Reflect(fwd, outInfo.mNormal);
                    ignoreActor = outInfo.mActor;
                }
                else {
                    noReflection = true;
                }
            }
            else {
                noReflection = true;
            }
        }
        mLineSegVect.push_back(lineSeg);
    }
}

void LaserComponent::Draw(Shader* shader)
{
    if (mMesh)
    {
        // Set the active texture
        Texture* t = mMesh->GetTexture(mTextureIndex);
        if (t)
        {
            t->SetActive();
        }
        // Set the mesh's vertex array as active
        VertexArray* va = mMesh->GetVertexArray();
        va->SetActive();
        
        for (auto i : mLineSegVect) {
            // Set the world transform
            shader->SetMatrixUniform("uWorldTransform", LineToMatrix(i));

            // Draw
            glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
        }
    }
}

Matrix4 LaserComponent::LineToMatrix(LineSegment line)
{
    Quaternion quat;
    Vector3 oldForward = Vector3::UnitX;
    Vector3 newForward = line.mEnd - line.mStart;
    newForward.Normalize();
    
    float dot = Vector3::Dot(oldForward, newForward);
    Vector3 cross = Vector3::Cross(oldForward, newForward);
    float angle = acos(dot);
    Vector3 axis = cross;
    axis.Normalize();
    
    // The newRot is the same as oldRot
    if (dot == 1) {
        quat = Quaternion::Identity;
    }
    else if (dot == -1) {
        quat = Quaternion(Vector3::UnitZ, Math::Pi);
    }
    else {
        quat = Quaternion(axis, angle);
    }
    
    Vector3 transVect = Vector3::Lerp(line.mStart, line.mEnd, 0.5);
    
    Matrix4 scale = Matrix4::CreateScale(line.Length(), 1.0f, 1.0f);
    Matrix4 rot = Matrix4::CreateFromQuaternion(quat);
    Matrix4 translate = Matrix4::CreateTranslation(transVect);
    
    Matrix4 transform = scale * rot * translate;
    
    return transform;
}
