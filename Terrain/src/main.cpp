//*******Terrain with multitexturing support ****
/*
//textures and skydome from www.swanton007.artstation.com/projects/lGK65
// multitexturing and lighting guide
//www.jerome.jouvie.free.fr/opengl-tutorials/Tutorial22.php//
//www.3dgep.com/texturing-and-lighting-with-open-gl-and-glsl/
//www.keithlantz.net/2011/10/lighting-and-environment-mapping-with-glsl/
//Terrain heighmap generation using www.planetside.co.uk/
// sky and atomospher rendering http://vterrain.org/Atmosphere/
//Terragen user tutorial www.youtube.com/watch?v=8k4Azpr9Crw&list=PLaH1PMiI_7AiqtJht63dr1jHVAvH_BtfV
// Sound track from https://itunes.apple.com/us/album/forest-of-secrets/id1086871547
*/


#include "TerrainPCH.h"
#include "Skydome.h"
#include "Terrain.h"
#include "TerrainCamera.h"
#include "ElapsedTime.h"
#include <bass.h>
#include<stack>

Skydome g_SkyDome;
Terrain g_Terrain(30, 2);
TerrainCamera g_Camera;

glm::vec3 g_DefaultCameraTranslate( 0, 25, 0 );
glm::vec3 g_DefaultCameraRotate( 0, 0, 0 );
glm::vec3 g_DefaultCameraPivot( 0, 0, 0 );

bool g_bLeftMouseDown = false;
bool g_bRightMouseDown = false;

float g_Forward = 0.0f;
float g_Back = 0.0f;
float g_Left = 0.0f;
float g_Right = 0.0f;

// Walk speed in units/second.
const float g_WalkSpeed = 1000.0f;

glm::vec2 g_MouseCurrent = glm::vec2(0);
glm::vec2 g_MousePrevious = glm::vec2(0);
glm::vec2 g_MouseDelta = glm::vec2(0);

int g_iWindowWidth = 3840;
int g_iWindowHeight = 2160;
int g_iGLUTWindowHandle = 0;
//int g_iErrorCode = 0;

GLfloat g_LighDir[] = { 1.0f, 1.0f, 1.0f, 0.0f }; 
GLfloat g_LightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat g_LightDiffuse[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat g_LightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat g_LighAttenuation0 = 0.0f;
GLfloat g_LighAttenuation1 = 0.0f;
GLfloat g_LighAttenuation2 = 0.0f;

void InitGL( int argc, char* argv[] );
void DisplayGL();
void IdleGL();
void KeyboardGL( unsigned char c, int x, int y );
void KeyboardUpGL(unsigned char c, int x, int y);
void MouseGL( int button, int state, int x, int y );
void MotionGL( int x, int y );
void PassiveMotionGL( int x, int y );
void ReshapeGL(int w, int h)
{
	std::cout << "ReshapGL( " << w << ", " << h << " );" << std::endl;

	h = std::max(h, 1);

	g_iWindowWidth = w;
	g_iWindowHeight = h;

	g_Camera.SetViewport(0, 0, w, h);
	g_Camera.ApplyViewport();

	g_Camera.SetProjection(60.0f, w / (float)h, 0.1f, 1000.0f);
	g_Camera.ApplyProjectionTransform();

	glutPostRedisplay();
}
void Cleanup();

int main( int argc, char* argv[] )
{
    atexit(Cleanup);

    InitGL( argc, argv );

    g_Camera.SetTranslate( g_DefaultCameraTranslate );
    g_Camera.SetRotate( g_DefaultCameraRotate );

    g_SkyDome.Init();
    g_SkyDome.SetCamera(&g_Camera);
	//loaD THE SKYDOME TEXTURE
    if ( !g_SkyDome.LoadTexture( "Data/Textures/skydome_moon.png" ) )
    {
        std::cerr << "skydome texture not loaded!" << std::endl;
    }//LOAD TERRAIN RAW FILe

    if ( !g_Terrain.LoadHeightmap( "Data/Terrain/terrain0-16bbp-257x257.raw", 16, 257, 257 ) )
    {
        std::cerr << "Failed to load heightmap for terrain!" << std::endl;
    }
	// textures from https://www.textures.com/download/grass0138/44354
	//https://www.pinterest.dk/search/pins/?q=snow&rs=typed&term_meta[]=snow%7Ctyped
	//load grass, snow and rock texture for blending and applying it on terrain
    if ( !g_Terrain.LoadTexture( "Data/Textures/grass.jpg", 0 ) )
    {
        std::cerr << "Texture stage 0 failed!" << std::endl;        
    }
    if ( !g_Terrain.LoadTexture( "Data/Textures/rock.jpg", 1 ) )
    {
        std::cerr <<"Texture stage 1 failed!" << std::endl;        
    }
    if ( !g_Terrain.LoadTexture( "Data/Textures/snow.jpg", 2 ) )
    {
        std::cerr << "Texture stage 2 failed!" << std::endl;        
    }

    g_Camera.SetTerrain( &g_Terrain );
	
    glutMainLoop();
}

// clean up and deallocate the rewsources
void Cleanup()
{
    if ( g_iGLUTWindowHandle != 0 )
    {
        glutDestroyWindow( g_iGLUTWindowHandle );
        g_iGLUTWindowHandle = 0;
    }
}

HSAMPLE loadSample(char * filename) {
	HSAMPLE sam;
	if (sam = BASS_SampleLoad(FALSE, filename, 0, 0, 3, BASS_SAMPLE_OVER_POS))
		std::cerr << "sample " << filename << " loaded!\n";
	else {
		std::cerr << "Can't load sample";
		exit(0);
	}
	return sam;
}

// intiitate gl
void InitGL( int argc, char* argv[] )
{
    std::cout << "Initialise OpenGL..." << std::endl;

    glutInit(&argc, argv);
    int iScreenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int iScreenHeight = glutGet(GLUT_SCREEN_HEIGHT);

    glutInitDisplayMode( GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH );

    glutInitWindowPosition( ( iScreenWidth - g_iWindowWidth ) / 2,
                            ( iScreenHeight - g_iWindowHeight ) / 2 );
    glutInitWindowSize( g_iWindowWidth, g_iWindowHeight );

    g_iGLUTWindowHandle = glutCreateWindow( "Terrain" );
    if ( glewInit() != GLEW_OK )
    {
        std::cout << "Failed to initilalize GLEW!" << std::endl;
        exit(-1);
    }

    // Check for the supported extensions
	// check for support 
	// of "GL_ARB_multitexture ""GL_ARB_texture_env_combine ""GL_ARB_vertex_buffer_object "
    if ( !glewIsSupported( "GL_ARB_multitexture "
                           "GL_ARB_texture_env_combine " 
                           "GL_ARB_vertex_buffer_object " 
                           ) )
    {
        std::cout << "Required OpenGL extension support is missing." << std::endl;
        exit(-1);
    }

    // Register GLUT callbacks
    glutDisplayFunc(DisplayGL);
    glutIdleFunc(IdleGL);
    glutMouseFunc(MouseGL);
    glutMotionFunc(MotionGL);
    glutPassiveMotionFunc(PassiveMotionGL);
    glutKeyboardFunc(KeyboardGL);
	glutKeyboardUpFunc(KeyboardUpGL);
    glutReshapeFunc(ReshapeGL);

    // Setup initial GL State
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    glClearDepth( 1.0f );

    glShadeModel( GL_SMOOTH );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_DEPTH_TEST );

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, g_LightAmbient );

    glLightfv( GL_LIGHT0, GL_DIFFUSE, g_LightDiffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, g_LightSpecular );
    glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, g_LighAttenuation0 );
    glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, g_LighAttenuation1 );
    glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, g_LighAttenuation2 );

    glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );

	HSAMPLE sample;
	HCHANNEL channel;

	//Initialise BASS Audio 
	if (!BASS_Init(-1, 44100, 0, 0, NULL))
		std::cerr << "Can't initialize device";
	// music purchased from itunes Forest Of forgetfulness
	
	sample = loadSample("Forest Of forgetfulness.mp3");

	channel = BASS_SampleGetChannel(sample, FALSE);
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_FREQ, 0);
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, 0.75);
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_PAN, -1);
	BASS_ChannelPlay(channel, TRUE);


    std::cout << "\nInitialise OpenGL: Success!" << std::endl;
}

void DrawAxis( float fScale, glm::vec3 translate = glm::vec3(0) )
{
    glPushAttrib( GL_ENABLE_BIT );

//    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glDisable( GL_TEXTURE_2D );

    glPushMatrix();
    glTranslatef( translate.x, translate.y, translate.z );
    glScalef( fScale, fScale, fScale );

    glBegin( GL_LINES );
    {
        glColor3f( 1.0f, 0.0f, 0.0f );
        glVertex3f( 0.0f, 0.0f, 0.0 );
        glVertex3f( 1.0f, 0.0f, 0.0f );

        glColor3f( 0.0f, 1.0f, 0.0f );
        glVertex3f( 0.0f, 0.0f, 0.0f );
        glVertex3f( 0.0f, 1.0f, 0.0f );

        glColor3f( 0.0f, 0.0f, 1.0f );
        glVertex3f( 0.0f, 0.0f, 0.0f );
        glVertex3f( 0.0f, 0.0f, 1.0f );
    }
    glEnd();

    glPopMatrix();
    glPopAttrib();
}

void DisplayGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
	// Clear the color buffer, and the depth buffer.

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    g_Camera.ApplyViewTransform();
    g_SkyDome.Render();

    //DrawAxis( 20.0f, g_Camera.GetPivot() );

    glLightfv( GL_LIGHT0, GL_POSITION, g_LighDir );
    g_Terrain.Render();

    glutSwapBuffers();
    glutPostRedisplay();
}

void IdleGL()
{
    static ElapsedTime elapsedTime;
    float fDeltaTime = elapsedTime.GetElapsedTime();

	g_Camera.WalkForward(fDeltaTime * (g_Forward + g_Back) * g_WalkSpeed);
	g_Camera.Strafe(fDeltaTime * (g_Left + g_Right) * g_WalkSpeed);

    g_Camera.Update( fDeltaTime );

    glutPostRedisplay();
}

void KeyboardGL( unsigned char c, int x, int y )
{
    switch ( c )
    {
    case 'w':
    case 'W':
		g_Forward = 1.0f;
        break;
    case 'a':
    case 'A':
		g_Left = -1.0f;
        break;
    case 's':
    case 'S':
		g_Back = -1.0f;
        break;
    case 'd':
    case 'D':
		g_Right = 1.0f;
        break;
    case ' ':
		g_Camera.Jump(5000.0f);
        break;
    case 'r':
    case 'R':
        {
            std::cout << "Reset Parameters..." << std::endl;
            g_Camera.SetTranslate( g_DefaultCameraTranslate );
            g_Camera.SetRotate( g_DefaultCameraRotate );
            //g_Camera.SetPivot( g_DefaultCameraPivot );
        }
        break;
    case '\033': // escape quits
        exit(0);
        break;
    }
}

void KeyboardUpGL(unsigned char c, int x, int y)
{
	switch (c)
	{
	case 'w':
	case 'W':
		g_Forward = 0.0f;
		break;
	case 'a':
	case 'A':
		g_Left = 0.0f;
		break;
	case 's':
	case 'S':
		g_Back = 0.0f;
		break;
	case 'd':
	case 'D':
		g_Right = 0.0f;
		break;
	}

}


void MouseGL( int button, int state, int x, int y )
{
    if ( button == GLUT_LEFT_BUTTON )
    {
        g_bLeftMouseDown = ( state == GLUT_DOWN );
    }
    else if ( button == GLUT_RIGHT_BUTTON )
    {
        g_bRightMouseDown = ( state == GLUT_DOWN );
    }
    g_MousePrevious = glm::vec2( x, y );

}

void MotionGL( int x, int y )
{
    g_MouseCurrent = glm::vec2( x, y );
    g_MouseDelta = ( g_MousePrevious - g_MouseCurrent );

    // Update the camera
    if ( g_bLeftMouseDown || g_bRightMouseDown )
    {
        g_Camera.MouseMove( g_MouseDelta.x, -g_MouseDelta.y );
    }

    g_MousePrevious = g_MouseCurrent;
}

void PassiveMotionGL( int x, int y )
{

}


