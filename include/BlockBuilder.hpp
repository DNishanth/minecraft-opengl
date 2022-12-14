#ifndef BlockBuilder_HPP
#define BlockBuilder_HPP

#include <glad/glad.h>

#include <string>
#include <vector>

#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Shader.hpp"
#include "VertexBufferLayout.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "BlockData.hpp"

struct FaceTexture {
    float leftU;
    float rightU;
    float topV;
    float bottomV;

    // Fill buffer with xyzst components
    void addToBuffer(std::vector<GLfloat>& buffer) {
        buffer.insert(buffer.end(),
        {leftU, bottomV, rightU, bottomV, rightU, topV, leftU, topV});
    }
};

// Purpose:
// An abstraction to create multiple BlockBuilders
class BlockBuilder {
public:
    // BlockBuilder Constructor
    BlockBuilder();
    // BlockBuilder destructor
    ~BlockBuilder();
    // Initialize buffers, texture, and shader
    void InitializeBlockData(std::string atlasFileName);
    // Updates and transformations applied to BlockBuilder
    void Update(BlockData& blockData, unsigned int screenWidth, unsigned int screenHeight);
    // How to draw the BlockBuilder
    void Render(BlocksArray& BlocksArray);
    // Returns an BlockBuilders transform
    Transform& GetTransform();
    // Toggle uniform to enable directional light in shader
    void ToggleLighting();
    // Reload shader while program is running for debugging
    void ReloadShaders();
private:
    // Generate texture coordinates for the three face textures of a block and add to texture buffer
    void generateBlockTexture(BlockType blockType, int top, int side, int bottom);
    // Generate texture coordinates for given texture in atlas
    FaceTexture generateFaceTexture(int faceAtlasIndex);
    // BlockBuilder vertices
    std::vector<GLfloat> m_vertices;
    // BlockBuilder indices
    std::vector<GLuint> m_indices;
    // For now we have one shader per BlockBuilder.
    Shader m_shader;
    // For now we have one buffer per BlockBuilder.
    VertexBufferLayout m_vertexBufferLayout;
    // For now we have one texture per BlockBuilder
    Texture m_texture;
    // Store the BlockBuilders transformations
    Transform m_transform;
    // Store the 'camera' projection
    glm::mat4 m_projectionMatrix;
    // Store texture coordinates of each block type
    std::vector<GLfloat> m_blockTextures;
    // Flag for enabling directional light in shader
    int lightingEnabled;
};


#endif
