#include <iostream>

#include "BlockData.hpp"
#include "Camera.hpp"
#include "SelectionFrameBuffer.hpp"

SelectionFrameBuffer::SelectionFrameBuffer() {}

SelectionFrameBuffer::~SelectionFrameBuffer() {
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteTextures(1, &m_colorBuffer_ID);
    glDeleteRenderbuffers(1, &m_rbo);
}

void SelectionFrameBuffer::Create(int width, int height) {

    // Setup shaders
    std::string vertexShader = m_shader.LoadShader("./shaders/selection_vert.glsl");
	std::string fragmentShader = m_shader.LoadShader("./shaders/selection_frag.glsl");

    // Actually create our shader
	m_shader.CreateShader(vertexShader, fragmentShader);

    // Generate a framebuffer and select it
    glGenFramebuffers(1, &m_fbo);
    Bind();

    // Create a color attachment texture
    glGenTextures(1, &m_colorBuffer_ID);
    glBindTexture(GL_TEXTURE_2D, m_colorBuffer_ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // TODO: try above with GL_RGB32UI instead
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // TODO: test nearest
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // TODO: test if these lines are actually needed
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer_ID, 0);

    // Create our render buffer object for depth
    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "SelectionFrameBuffer is incomplete: " << status << std::endl;
        exit(1);
    }

    // Deselect buffers
    Unbind();
}

// Render visible blocks into framebuffer
void SelectionFrameBuffer::Render(BlocksArray& blocksArray, int width, int height) {
    Bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glm::mat4 m_projectionMatrix = glm::perspective(45.0f, (float) width/(float) height, 0.1f, 100.0f);
    int blockIndex = 1; // Background is index 0
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int z = 0; z < DEPTH; z++) {
                BlockData block = blocksArray.getBlock(x, y, z);
                if (block.isVisible) {
                    // Assign every visible block face a unique color
                    m_shader.SetUniformMatrix4fv("model", block.m_transform.GetTransformMatrix());
                    m_shader.SetUniformMatrix4fv("view", &Camera::Instance().GetWorldToViewmatrix()[0][0]);
                    m_shader.SetUniformMatrix4fv("projection", &m_projectionMatrix[0][0]);
                    for (int i = 0; i <= 30; i += 6) {
                        int r = ((blockIndex + (i/6)) & 0x000000FF) >>  0; // Convert block index to 3 digits from 0-255
                        int g = ((blockIndex + (i/6)) & 0x0000FF00) >>  8; // http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-an-opengl-hack/
                        int b = ((blockIndex + (i/6)) & 0x00FF0000) >> 16;
                        m_shader.SetUniform4f("blockColor", r/255.0f, g/255.0f, b/255.0f, 1.0f);
                        glDrawElements(GL_TRIANGLES,
                            6,   // The number of indices, not triangles.
                            GL_UNSIGNED_INT,    // Make sure the data type matches
                            (void*)(i * sizeof(GLuint)));           // Offset pointer to the data. nullptr
                                            // because we are currently bound:
                    }
                }
                blockIndex+=6; // Increment block index by 6 to account for blocks not rendered
            }
        }
    }
}

int SelectionFrameBuffer::ReadPixel(int x, int y) {
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    unsigned char pixelData[4];
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    glReadBuffer(GL_NONE);
    int blockIndex =
        pixelData[0] +
        pixelData[1] * 256 +
        pixelData[2] * 256 * 256; // Convert color back to block index
    return blockIndex;
}

void SelectionFrameBuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    m_shader.Bind();
}

void SelectionFrameBuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_shader.Unbind();
}