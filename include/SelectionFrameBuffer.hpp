#ifndef SELECTION_FRAME_BUFFER_HPP
#define SELECTION_FRAME_BUFFER_HPP

#include <glad/glad.h>

#include "Shader.hpp"

class SelectionFrameBuffer {
    public:
        SelectionFrameBuffer();

        ~SelectionFrameBuffer();

        void Create(int width, int height);

        void Render(BlocksArray& blocksArray, int width, int height);

        int ReadPixel(int x, int y);

        void Bind();

        void Unbind();


    private:
        // Framebuffer ID
        GLuint m_fbo;

        // Texturebuffer ID
        GLuint m_colorBuffer_ID;

        // Renderbuffer ID
        GLuint m_rbo;

        Shader m_shader;
};

#endif