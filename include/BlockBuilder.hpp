#ifndef BlockBuilder_HPP
#define BlockBuilder_HPP

#include <glad/glad.h>

#include <vector>
#include <string>

#include "Shader.hpp"
#include "VertexBufferLayout.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "BlockData.hpp"

#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Purpose:
// An abstraction to create multiple BlockBuilders
class BlockBuilder {
public:
    // BlockBuilder Constructor
    BlockBuilder();
    // BlockBuilder destructor
    ~BlockBuilder();
    // Create a textured quad
    void MakeTexturedQuad(std::string fileName);
    // Updates and transformatinos applied to BlockBuilder
    void Update(BlockData& blockData, unsigned int screenWidth, unsigned int screenHeight);
    // How to draw the BlockBuilder
    void Render(BlocksArray& BlocksArray);
    // Returns an BlockBuilders transform
    Transform& GetTransform();
private:
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
};


#endif
