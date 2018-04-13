#pragma once;

class Terrain
{
public:

    Terrain( float heightScale = 500.0f, float blockScale = 2.0f );
	//float heightScale: determines the maximum height of the terrain in world units.
	//float blockScale: determines the space between terrain vertices in world units for both the
	//X and Z axes.
    virtual ~Terrain();

    void Terminate();// Deletes any dynamically allocated memory and deletes textures and vertex 
	//buffer objects that are stored in GPU
    bool LoadHeightmap( const std::string& filename, unsigned char bitsPerPixel, unsigned int width, unsigned int height );
   //bool loadHeightMap  Load the terrain height map from a RAW file.  This method will accept 8-bit or 16-bit RAW files exported from terragen
    bool LoadTexture( const std::string& filename, unsigned int textureStage = 0 );
	//bool LoadTexture-The terrain supports up to 3 texture stages (0-2).  Each texture stage will be blended with the next to produce a realistic looking terrain
	// texture to be used can be specified for each stage, method returns true if textures are successfully loaded
	// unsigned int textureStage- 
	//texture stage we want to bind texture to, values between 0-3(total3) 
	//texture 0 for lowest point
	//texture 2 for the highest 
	// and texture 3 for intermediate level or the space between these tex0 and tex2


    // Get the height of the terrain at a position in world space
    float GetHeightAt( const glm::vec3& position );
	//glm::vec3 posiition the world space position to test for the terrain height

    void Render();
    // In debug builds, normals will be rendered for convenience but can be disabled 
	//kyle I've disabled them 
	
    void DebugRender();

protected:
    void GenerateIndexBuffer();
    void GenerateNormals();
    // Generates the vertex buffer objects from the 
    // position, normal, texture, and color buffers
    void GenerateVertexBuffers();

    void RenderNormals();

private:
    typedef std::vector<glm::vec3>  PositionBuffer;
    typedef std::vector<glm::vec4>  ColorBuffer;
    typedef std::vector<glm::vec3>  NormalBuffer;
    typedef std::vector<glm::vec2>  TexCoordBuffer;
    typedef std::vector<GLuint>     IndexBuffer;

    PositionBuffer  m_PositionBuffer;
    ColorBuffer     m_ColorBuffer;
    NormalBuffer    m_NormalBuffer;
    TexCoordBuffer  m_Tex0Buffer;
    IndexBuffer     m_IndexBuffer;

    // ID's for the VBO's
    GLuint  m_GLVertexBuffer;
    GLuint  m_GLNormalBuffer;
    GLuint  m_GLColorBuffer;
    GLuint  m_GLTex0Buffer;
    GLuint  m_GLTex1Buffer;
    GLuint  m_GLTex2Buffer;
    GLuint  m_GLIndexBuffer;

    static const unsigned int m_uiNumTextures = 3;
    GLuint  m_GLTextures[m_uiNumTextures];

    glm::mat4x4 m_LocalToWorldMatrix;
    glm::mat4x4 m_InverseLocalToWorldMatrix;

    // The dimensions of the heightmap texture
    glm::uivec2 m_HeightmapDimensions;

    // The height-map value will be multiplied by this value
    // before it is assigned to the vertex's Y-coordinate
    float   m_fHeightScale;
    // The vertex's X and Z coordinates will be multiplied by this
    // for each step when building the terrain
    float   m_fBlockScale;
};