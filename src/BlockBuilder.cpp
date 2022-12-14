#include "BlockBuilder.hpp"
#include "Camera.hpp"
#include "Error.hpp"

// Generate textures coordinates for all block types and store in shared buffer
BlockBuilder::BlockBuilder() {
    generateBlockTexture(Dirt, 242, 242, 242);
    generateBlockTexture(Grass, 240, 243, 242);
    generateBlockTexture(Plank, 244, 244, 244);
    generateBlockTexture(Brick, 247, 247, 247);
    generateBlockTexture(Cobblestone, 224, 224, 224);
    generateBlockTexture(Sandstone, 32, 32, 32);
    generateBlockTexture(Mossystone, 212, 212, 212);
    generateBlockTexture(LightBlueWool, 33, 33, 33);
    generateBlockTexture(OrangeWool, 34, 34, 34);
    generateBlockTexture(Snow, 178, 180, 180);
	lightingEnabled = 0;
}

BlockBuilder::~BlockBuilder() {}

// Generate texture coordinates for given texture in atlas
FaceTexture BlockBuilder::generateFaceTexture(int faceAtlasIndex) {
    int numRows = 16;
	int numCols = 16;
	int rowIdx = faceAtlasIndex / numCols;
	int colIdx = faceAtlasIndex % numCols;

	// UV 0, 0 is top left of atlas
    float leftU = (float) colIdx / numCols + .001f;
	float rightU = (colIdx + 1.0f) / numCols - .001f;
	float topV = (float) (numRows - rowIdx) / numRows - .001f;
	float bottomV = (numRows - rowIdx - 1.0f) / numRows + .001f;

    // std::cout << "Face Atlas Idx: " << faceAtlasIndex << std::endl;
    // std::cout << "Row idx: " << rowIdx << std::endl;
	// std::cout << "Col idx: " << colIdx << std::endl;
    // std::cout << "Left U: " << leftU << std::endl;
	// std::cout << "Right U: " << rightU << std::endl;
	// std::cout << "Top V: " << topV << std::endl;
	// std::cout << "Bottom V: " << bottomV << std::endl;
    return (FaceTexture) {leftU, rightU, topV, bottomV};
}

// Generate texture coordinates for the three face textures of a block and add to texture buffer
void BlockBuilder::generateBlockTexture(BlockType blockType, int topAtlasIndex, int sideAtlasIndex, int bottomAtlasIndex) {
    FaceTexture topFace = generateFaceTexture(topAtlasIndex);
    FaceTexture sideFace = generateFaceTexture(sideAtlasIndex);
    FaceTexture bottomFace = generateFaceTexture(bottomAtlasIndex);
    sideFace.addToBuffer(m_blockTextures);     // Front face
    sideFace.addToBuffer(m_blockTextures);     // Back face
    topFace.addToBuffer(m_blockTextures);      // Top face
    bottomFace.addToBuffer(m_blockTextures);   // Bottom face
    sideFace.addToBuffer(m_blockTextures);     // Right face
    sideFace.addToBuffer(m_blockTextures);     // Left face
}

// Initialization of BlockBuilder
//
// This could be called in the constructor or
// otherwise 'explicitly' called this
// so we create our BlockBuilders at the correct time
void BlockBuilder::InitializeBlockData(std::string atlasFileName) {
	m_vertices = {
		// Front face
		0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, // x,y,z and nx,ny,nz
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,

		// Back face
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

		// Top face
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
		0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

		// Bottom face
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,

		// Right face
		0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

		// Left face
		-0.5f,  0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f
    };

	m_indices = {
		0, 1, 2, 0, 2, 3,       // Front face
		4, 5, 6, 4, 6, 7,       // Back face
		8, 9, 10, 8, 10, 11,    // Top face
		12, 13, 14, 12, 14, 15, // Bottom face
		16, 17, 18, 16, 18, 19, // Right face
		20, 21, 22, 20, 22, 23  // Left face
	};

	// Create a buffer and set the stride of information
	// Buffer 1 - positions and normals
	// Buffer 2 - texture coordinates
	m_vertexBufferLayout.CreateTextureBufferLayout(
        m_vertices.size(), m_blockTextures.size(), m_indices.size(),
        m_vertices.data(), m_blockTextures.data(), m_indices.data()
    );

	// Load our actual texture
	// We are using the input parameter as our texture to load
	m_texture.LoadTexture(atlasFileName.c_str());

	// Setup shaders
	std::string vertexShader = m_shader.LoadShader("./shaders/vert.glsl");
	std::string fragmentShader = m_shader.LoadShader("./shaders/frag.glsl");

    // Actually create our shader
	m_shader.CreateShader(vertexShader, fragmentShader);

    m_vertexBufferLayout.Bind();
	m_texture.Bind();
	m_shader.Bind();

    m_shader.SetUniformMatrix1i("u_Texture", 0);
}

void BlockBuilder::Update(BlockData& blockData, unsigned int screenWidth, unsigned int screenHeight) {
    // Here we apply the 'view' matrix which creates perspective.
	// The first argument is 'field of view'
	// Then perspective
	// Then the near and far clipping plane.
	// Note I cannot see anything closer than 0.1f units from the screen.
	m_projectionMatrix = glm::perspective(45.0f, (float)screenWidth/(float)screenHeight, 0.1f, 150.0f);
	// Set the uniforms in our current shader
	m_shader.SetUniformMatrix4fv("model", blockData.m_transform.GetTransformMatrix());
    m_shader.SetUniformMatrix4fv("view", &Camera::Instance().GetWorldToViewmatrix()[0][0]);
	m_shader.SetUniformMatrix4fv("projection", &m_projectionMatrix[0][0]);
}

void BlockBuilder::Render(BlocksArray& blocksArray) {
	// Select this BlockBuilders buffer to render
	m_vertexBufferLayout.Bind();
	// Select this BlockBuilders texture to render
	m_texture.Bind();
	// Select this BlockBuilders shader to render
	m_shader.Bind();
	// Set uniforms for directional light
	m_shader.SetUniformMatrix1i("lightingEnabled", lightingEnabled);
    m_shader.SetUniform3f("lights[0].lightColor", 1.0f, 1.0f, 1.0f);
    m_shader.SetUniform3f("lights[0].lightDir", -0.5f, -1.0f, -0.5f);
    m_shader.SetUniform1f("lights[0].ambientIntensity", 0.4f);
    m_shader.SetUniform1f("lights[0].specularStrength", 0.3f);
    // Render data
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int z = 0; z < DEPTH; z++) {
                BlockData block = blocksArray.getBlock(x, y, z);
                if (block.isVisible) {
					// Set texture offset in texture buffer based on block type
                    glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(float)*2, (GLvoid*)(48 * block.blockType * sizeof(GLfloat)));
                    Update(block, 1280, 720); // Apply transforms for each block
                    glDrawElements(GL_TRIANGLES,
                        m_indices.size(),   // The number of indices, not triangles.
                        GL_UNSIGNED_INT,    // Make sure the data type matches
                        nullptr);           // Offset pointer to the data. nullptr
                                            // because we are currently bound:
                }
            }
        }
    }
}

// Returns the actual transform stored in our BlockBuilder
// which can then be modified
Transform& BlockBuilder::GetTransform() {
    return m_transform;
}

// Toggle uniform to enable directional light in shader
void BlockBuilder::ToggleLighting() {
	lightingEnabled = !lightingEnabled;
}

// Reload shader while program is running for debugging
void BlockBuilder::ReloadShaders() {
	std::string vertexShader = m_shader.LoadShader("./shaders/vert.glsl");
	std::string fragmentShader = m_shader.LoadShader("./shaders/frag.glsl");
	m_shader.CreateShader(vertexShader, fragmentShader);
}
