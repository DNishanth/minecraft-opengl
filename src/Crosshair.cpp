#include "Crosshair.hpp"

Crosshair::Crosshair() {
}

Crosshair::~Crosshair() {}

void Crosshair::MakeTexturedQuad(float screenWidth, float screenHeight) {
	m_vertices = {
		-0.01f, 0.0f, 0.0f,
        0.01f, 0.0f, 0.0f,
		0.0f, -0.01f, 0.0f,
        0.0f, 0.01f, 0.0f,
		// 0.0f, screenHeight/2.0f, 0.0f,
        // screenWidth, screenHeight/2.0f, 0.0f
    };
	// m_vertices = {
	// 	screenWidth/2.0f, 0.0f, 0.0f,
    //     screenWidth/2.0f, screenWidth, 0.0f,
	// 	0.0f, screenHeight/2.0f, 0.0f,
    //     screenWidth, screenHeight/2.0f, 0.0f
    // };

	m_indices = {
		0, 1, 2, 3
	};

	m_vertexBufferLayout.CreatePositionBufferLayout(
        m_vertices.size(), m_indices.size(),
        m_vertices.data(), m_indices.data()
    );

	// Setup shaders
	std::string vertexShader = m_shader.LoadShader("./shaders/crosshair_vert.glsl");
	std::string fragmentShader = m_shader.LoadShader("./shaders/crosshair_frag.glsl");

    // Actually create our shader
	m_shader.CreateShader(vertexShader, fragmentShader);

    // m_vertexBufferLayout.Bind();
	// m_shader.Bind();
}

// void Crosshair::Update(BlockData& blockData, unsigned int screenWidth, unsigned int screenHeight) {}

void Crosshair::Render() {
	// Select this Crosshairs buffer to render
	m_vertexBufferLayout.Bind();
	// Select this Crosshairs shader to render
	m_shader.Bind();
    glDrawElements(GL_LINES,
        m_indices.size(),   // The number of indices, not triangles.
        GL_UNSIGNED_INT,    // Make sure the data type matches
        nullptr);           // Offset pointer to the data. nullptr
                            // because we are currently bound:
	m_shader.Unbind();
	m_vertexBufferLayout.Unbind();
}