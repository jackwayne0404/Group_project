#include "TerrainPCH.h"
#include "Terrain.h"
#include "TerrainCamera.h"

TerrainCamera::TerrainCamera()
: m_pTerrain(NULL)
, m_fHeightOffset( 5.0f )
, m_Force(0)
, m_Velocity(0)
, m_fFriction(0.9f)
, m_bIsOnGround(false)
{}

void TerrainCamera::SetTerrain( Terrain* pTerrain )
{
    m_pTerrain = pTerrain;
}

void TerrainCamera::WalkForward( float fAmount )
{
    glm::vec3 atVector( 0.0f, 0.0f, -fAmount+(+0.5f) );
    glm::quat rotation( glm::vec3( 0.0f, glm::radians(m_Rotate.y), 0.0f) );

    m_Force += ( rotation * atVector );
}

void TerrainCamera::Strafe( float fAmount )
{
    glm::vec3 rightVector( fAmount, 0.0f, 0.0f );
    glm::quat rotation( glm::vec3( 0.0f, glm::radians(m_Rotate.y), 0.0f) );
    m_Force += ( rotation * rightVector );
}

void TerrainCamera::Jump( float fAmount )
{
    if ( m_bIsOnGround )
    {      
        glm::vec3 upVector( 0.0f, fAmount, 0.0f );
        m_Force += upVector;
        m_bIsOnGround = false;
    }
}

void TerrainCamera::MouseMove( float dx, float dy )
{
    // TODO: Parameratize MinPitch and MaxPitch
    m_Rotate.x = glm::clamp<float>( m_Rotate.x + dy, -80, 80 );
    m_Rotate.y += dx;
    // TODO: wrap rotate.y to -360..360 range?
}

void TerrainCamera::Update( float fDeltaTime )
{
    // Add the force of gravity
    m_Force += glm::vec3( 0.0f, -98.1f, 0.0f );
    m_Velocity += ( m_Force * fDeltaTime );
    m_Translate += ( m_Velocity * fDeltaTime );

    // Make sure we don't fall through the terrain
    if ( m_pTerrain != NULL )
    {
        float fHeight = m_pTerrain->GetHeightAt( m_Translate );
        if ( m_Translate.y < ( fHeight + m_fHeightOffset ) ) 
        {   
            m_bIsOnGround = true;
            m_Translate.y = fHeight + m_fHeightOffset;
        }
    }

    // Reset the force vector
    m_Force = glm::vec3(0);
    // Slow down our movement by friction amount
    const float fTargetFrameRate = 60.0f;
    m_Velocity *= powf( m_fFriction, fTargetFrameRate * fDeltaTime );
}

void TerrainCamera::ApplyViewTransform()
{
    glMatrixMode( GL_MODELVIEW );
    glRotatef( m_Rotate.x, -1.0f, 0.0f, 0.0f );
    glRotatef( m_Rotate.y, 0.0f, -1.0f, 0.0f );
    glRotatef( m_Rotate.z, 0.0f, 0.0f, -1.0f );
    glTranslatef( -m_Translate.x, -m_Translate.y, -m_Translate.z );
}
