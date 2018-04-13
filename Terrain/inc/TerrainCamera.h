#pragma once;
#include "Camera.h"

class Terrain;

class TerrainCamera : public Camera
{
public:
    TerrainCamera();

    void SetTerrain( Terrain* pTerrain );

    void WalkForward( float fAmount );
    void Strafe( float fAmount );
    void Jump( float fAmount );

    // Add the mouse X and Y movement to look around
    void MouseMove( float dx, float dy );

    // Update causes the camera to be pushed back down onto the terrain
    void Update( float fDeltaTime );
    void ApplyViewTransform();

protected:

private:
    Terrain*    m_pTerrain;
    float       m_fHeightOffset;

    // Force to be applied this frame
    glm::vec3   m_Force;
    // Current velocity of the camera
    glm::vec3   m_Velocity;

    // Friction to be applied to velocity
    float       m_fFriction;
    bool        m_bIsOnGround;
};