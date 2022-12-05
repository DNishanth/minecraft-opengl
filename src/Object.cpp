#include "Object.hpp"
#include "Error.hpp"

Object::Object() {}

Object::~Object() {}

// Initialization of object as a 'quad'
//
// This could be called in the constructor or
// otherwise 'explicitly' called this
// so we create our objects at the correct time
void Object::MakeTexturedQuad(std::string fileName) {
	float numRows = 16.0f;
	float numCols = 16.0f;
	float rowIdx = 3;
	float colIdx = 7; // wood plank block
	float colIdx2 = 7;
	float rowIdx2 = 3;

	// UV 0, 0 is bottom left of atlas
	float left_U  = colIdx / numCols;
	float right_U = (colIdx + 1.0f) / numCols;
	float top_V = (numRows - rowIdx) / numRows;
	float bottom_V = (numRows - rowIdx - 1.0f) / numRows;

	float left_U2  = colIdx2 / numCols;
	float right_U2 = (colIdx2 + 1.0f) / numCols;
	float top_V2    = (numRows - rowIdx2)        / numRows;
	float bottom_V2 = (numRows - rowIdx2 - 1.0f) / numRows;

	std::cout << "Left U: " << left_U << std::endl;
	std::cout << "Right U: " << right_U << std::endl;
	std::cout << "Top V: " << top_V << std::endl;
	std::cout << "Bottom V: " << bottom_V << std::endl;

	m_vertices = {
		// Front face
		-1.0f, -1.0f,  1.0f,  left_U, bottom_V,
		1.0f, -1.0f,  1.0f,  right_U, bottom_V,
		1.0f,  1.0f,  1.0f,  right_U, top_V,
		-1.0f,  1.0f,  1.0f,  left_U, top_V,

		// Back face
		-1.0f, -1.0f, -1.0f,  left_U, bottom_V,
		-1.0f,  1.0f, -1.0f,  right_U, bottom_V,
		1.0f,  1.0f, -1.0f,  right_U, top_V,
		1.0f, -1.0f, -1.0f,  left_U, top_V,

		// Top face
		-1.0f,  1.0f, -1.0f,  left_U2, bottom_V2,
		-1.0f,  1.0f,  1.0f,  right_U2, bottom_V2,
		1.0f,  1.0f,  1.0f,  right_U2, top_V2,
		1.0f,  1.0f, -1.0f,  left_U2, top_V2,

		// Bottom face
		-1.0f, -1.0f, -1.0f,  left_U, bottom_V,
		1.0f, -1.0f, -1.0f,  right_U, bottom_V,
		1.0f, -1.0f,  1.0f,  right_U, top_V,
		-1.0f, -1.0f,  1.0f,  left_U, top_V,

		// Right face
		1.0f, -1.0f, -1.0f,  left_U, bottom_V,
		1.0f,  1.0f, -1.0f,  right_U, bottom_V,
		1.0f,  1.0f,  1.0f,  right_U, top_V,
		1.0f, -1.0f,  1.0f,  left_U, top_V,

		// Left face
		-1.0f, -1.0f, -1.0f,  left_U, bottom_V,
		-1.0f, -1.0f,  1.0f,  right_U, bottom_V,
		-1.0f,  1.0f,  1.0f,  right_U, top_V,
		-1.0f,  1.0f, -1.0f,  left_U, top_V
	};

	m_indices = {
		0, 1, 2, 0, 2, 3,    // front
		4, 5, 6, 4, 6, 7,    // back
		8, 9, 10, 8, 10, 11, // top
		12, 13, 14, 12, 14, 15, // bottom
		16, 17, 18, 16, 18, 19, // right
		20, 21, 22, 20, 22, 23  // left
	};

	// Create a buffer and set the stride of information
	// NOTE: We are leveraging our data structure in order to very cleanly
	//       get information into and out of our data structure.
	m_vertexBufferLayout.CreateTextureBufferLayout(m_vertices.size(),m_indices.size(),m_vertices.data(),m_indices.data());

	// Load our actual texture
	// We are using the input parameter as our texture to load
	m_texture.LoadTexture(fileName.c_str());

	// Setup shaders
	std::string vertexShader = m_shader.LoadShader("./shaders/vert.glsl");
	std::string fragmentShader = m_shader.LoadShader("./shaders/frag.glsl");
	// Actually create our shader
	m_shader.CreateShader(vertexShader,fragmentShader);
}

void Object::Update(unsigned int screenWidth, unsigned int screenHeight) {
	m_texture.Unbind();
	// Make sure we are updating the correct 'buffers'
	m_vertexBufferLayout.Bind();
	m_texture.Bind();
	m_shader.Bind();
	// For our object, we apply the texture in the following way
	// Note that we set the value to 0, because we have bound
	// our texture to slot 0.
	m_shader.SetUniformMatrix1i("u_Texture",0);

		// Here we apply the 'view' matrix which creates perspective.
	// The first argument is 'field of view'
	// Then perspective
	// Then the near and far clipping plane.
	// Note I cannot see anything closer than 0.1f units from the screen.
	// TODO: In the future this type of operation would be abstracted away
	//       in a camera class.
//        modelTransformMatrix = glm::translate(glm::mat4(),glm::vec3(0.0f,0.0f,-1.0f));
	m_projectionMatrix = glm::perspective(45.0f,(float)screenWidth/(float)screenHeight,0.1f,10.0f);
	// Set the uniforms in our current shader
	m_shader.SetUniformMatrix4fv("modelTransformMatrix",m_transform.GetTransformMatrix());
	m_shader.SetUniformMatrix4fv("projectionMatrix", &m_projectionMatrix[0][0]);
}

void Object::Render() {
	// Select this objects buffer to render
	m_vertexBufferLayout.Bind();
	// Select this objects texture to render
	m_texture.Bind();
	// Select this objects shader to render
	m_shader.Bind();
	//Render data
	glDrawElements(GL_TRIANGLES,
				m_indices.size(),  // The number of indices, not triangles.
				GL_UNSIGNED_INT,   // Make sure the data type matches
				nullptr);          // Offset pointer to the data. nullptr
									// because we are currently bound:
}

// Returns the actual transform stored in our object
// which can then be modified
Transform& Object::GetTransform() {
    return m_transform;
}
