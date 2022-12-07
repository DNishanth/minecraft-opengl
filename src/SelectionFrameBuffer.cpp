#include <iostream>

#include "SelectionFrameBuffer.hpp"

SelectionFrameBuffer::SelectionFrameBuffer() {
    // TODO: why does creating shaders here cause error? undefined shader id?
}

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

    // Create our render buffer object for depth // TODO: try another texture as depth attachment if this don't work
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

int SelectionFrameBuffer::ReadPixel(int x, int y) {
    // TODO: Is flush step needed?
    // TODO: Check RGBA vs RGB INT vs RGB
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    unsigned char pixelData[4];
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    glReadBuffer(GL_NONE);
    int blockIndex =
        pixelData[0] +
        pixelData[1] * 256 +
        pixelData[2] * 256 * 256;
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