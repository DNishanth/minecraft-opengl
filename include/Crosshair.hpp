#ifndef Crosshair_HPP
#define Crosshair_HPP

#include <glad/glad.h>

#include <string>
#include <vector>

#include "glm/vec3.hpp"

#include "Shader.hpp"
#include "VertexBufferLayout.hpp"

class Crosshair {
public:
    // Crosshair Constructor
    Crosshair();
    // Crosshair destructor
    ~Crosshair();
    // Create a textured quad
    void MakeTexturedQuad(float screenWidth, float screenHeight);
    // Updates and transformatinos applied to Crosshair
    // void Update(BlockData& blockData, unsigned int screenWidth, unsigned int screenHeight);
    // How to draw the Crosshair
    void Render();
    // Returns an Crosshairs transform
private:
    // Crosshair vertices
    std::vector<GLfloat> m_vertices;
    // Crosshair indices
    std::vector<GLuint> m_indices;
    // For now we have one shader per Crosshair.
    Shader m_shader;
    // For now we have one buffer per Crosshair.
    VertexBufferLayout m_vertexBufferLayout;
    // For now we have one texture per Crosshair
};


#endif
