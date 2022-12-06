#if defined(LINUX) || defined(MINGW)
    #include <SDL2/SDL.h>
#else // This works for Mac
    #include <SDL.h>
#endif


#include "Texture.hpp"

#include <stdio.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <glad/glad.h>
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Default Constructor
Texture::Texture() {

}


// Default Destructor
Texture::~Texture(){
	// Delete our texture from the GPU
	glDeleteTextures(1,&m_textureID);
	// Delete our pixel data
	// Note: We could actually do this sooner
	// in our rendering process.
	if(m_pixelData!=NULL){
		delete[] m_pixelData;
	}
}

void Texture::LoadTexture(const std::string filepath){
	// Set member variable
    m_filepath = filepath;
	// Load our actual image data
	// This method loads .ppm files of pixel data
	// LoadPPM(true);
    glEnable(GL_TEXTURE_2D);
	// Generate a buffer for our texture
    glGenTextures(1,&m_textureID);
    // Similar to our vertex buffers, we now 'select'
    // a texture we want to bind to.
    // Note the type of data is 'GL_TEXTURE_2D'
    glBindTexture(GL_TEXTURE_2D, m_textureID);
	// Now we are going to setup some information about
	// our textures.
	// There are four parameters that must be set.
	// GL_TEXTURE_MIN_FILTER - How texture filters (linearly, etc.)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// Wrap mode describes what to do if we go outside the boundaries of
	// texture.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// At this point, we are now ready to load and send some data to OpenGL.
    int width, height, nrChannels;
    unsigned char *data = stbi_load("texture_atlas.png", &width, &height, &nrChannels, 0);
    if (!data) {
        std::cout << "Texture load failed" << std::endl;
        exit(1);
    }
    glTexImage2D(GL_TEXTURE_2D,
                        0 ,
                    GL_RGBA,
                    width,
                    height,
                        0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    data); // Here is the raw pixel data

    // We are done with our texture data so we can unbind.
	// We are done with our texture data so we can unbind.
	glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

/*  ===============================================
Desc: Sets a pixel in our array a specific color
Precondition:
Postcondition:
=============================================== */
void Texture::SetPixel(int x, int y, int r, int g, int b){
  if(x > m_width || y > m_height){
    return;
  }
  else{
    /*std::cout << "modifying pixel at "
              << x << "," << y << "from (" <<
              (int)color[x*y] << "," << (int)color[x*y+1] << "," <<
(int)color[x*y+2] << ")";*/

    m_pixelData[(x*3)+m_height*(y*3)] = r;
    m_pixelData[(x*3)+m_height*(y*3)+1] = g;
    m_pixelData[(x*3)+m_height*(y*3)+2] = b;

/*    std::cout << " to (" << (int)color[x*y] << "," << (int)color[x*y+1] << ","
<< (int)color[x*y+2] << ")" << std::endl;*/

  }
}

/*  ===============================================
Desc:
Precondition:
Postcondition:
=============================================== */
void Texture::PrintPixels(){
    for(int x = 0; x <  m_width*m_height*3; ++x){
        std::cout << " " << (int)m_pixelData[x];
    }
    std::cout << "\n";

}
// slot tells us which slot we want to bind to.
// We can have multiple slots. By default, we
// will set our slot to 0 if it is not specified.
void Texture::Bind(unsigned int slot) const{
	// Using OpenGL 'state' machine we set the active texture
	// slot that we want to occupy. Again, there could
	// be multiple at once.
	// At the time of writing, OpenGL supports 8-32 depending
	// on your hardware.
    glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0+slot);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::Unbind(){
	glBindTexture(GL_TEXTURE_2D, 0);
}


