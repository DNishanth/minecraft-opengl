#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "SDLGraphicsProgram.hpp"
#include "Camera.hpp"
#include "SelectionFrameBuffer.hpp"


// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h):m_screenWidth(w),m_screenHeight(h){
	// Initialization flag
	bool success = true;
	// String to hold any errors that occur.
	std::stringstream errorStream;
	// The window we'll be rendering to
	m_window = NULL;
	// Render flag

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else {
		//Use OpenGL 3.3 core
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
		// We want to request a double buffer for smooth updating.
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		//Create window
		m_window = SDL_CreateWindow( "MC",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                m_screenWidth,
                                m_screenHeight,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

		// Check if Window did not create.
		if (m_window == NULL ) {
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		//Create an OpenGL Graphics Context
		m_openGLContext = SDL_GL_CreateContext(m_window);
		if (m_openGLContext == NULL) {
			errorStream << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		// Initialize GLAD Library
		if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
			errorStream << "Failed to iniitalize GLAD\n";
			success = false;
		}

		//Initialize OpenGL
		if (!InitGL()) {
			errorStream << "Unable to initialize OpenGL!\n";
			success = false;
		}
  	}

    // If initialization did not work, then print out a list of errors in the constructor.
    if (!success) {
        errorStream << "SDLGraphicsProgram::SDLGraphicsProgram - Failed to initialize!\n";
        std::string errors=errorStream.str();
        SDL_Log("%s\n",errors.c_str());
    } else {
        SDL_Log("SDLGraphicsProgram::SDLGraphicsProgram - No SDL, GLAD, or OpenGL, errors detected during initialization\n\n");
    }

	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN); // Uncomment to enable extra debug support!
	GetOpenGLVersionInfo();

    builder.MakeTexturedQuad("");
    InitWorld();
    activeBlock = Brick;

    selectionBuffer.Create(m_screenWidth, m_screenHeight);
}


// Proper shutdown of SDL and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram() {
    // Reclaim all of our objects
    // delete m_object;

    //Destroy window
	SDL_DestroyWindow( m_window );
	// Point m_window to NULL to ensure it points to nothing.
	m_window = NULL;
	//Quit SDL subsystems
	SDL_Quit();
}


// Initialize OpenGL
// Setup any of our shaders here.
bool SDLGraphicsProgram::InitGL() {
	//Success flag
	bool success = true;

    // Setup our OpenGL State machine
    // The below command is new!
    // What we are doing, is telling opengl to create a depth(or Z-buffer)
    // for us that is stored every frame.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
	return success;
}


void SDLGraphicsProgram::InitWorld() {
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            for (int z = 0; z < 16; z++) {
                blocksArray.getBlock(x, y, z).isVisible = false;
                blocksArray.getBlock(x, y, z).blockType = Dirt;
                blocksArray.getBlock(x, y, z).m_transform.Translate(x,y,z);
            }
        }
    }
    blocksArray.getBlock(0, 0, 0).isVisible = true;
}


// Update OpenGL
void SDLGraphicsProgram::Update() {

    // static float rot = 0;
    // rot+=0.1;
    // if(rot>360){rot=0;}
    // for (int x = 0; x < 16; x++) {
    //     for (int y = 0; y < 16; y++) {
    //         for (int z = 0; z < 16; z++) {
    //             if (blocks[x][y][z].isVisible) {
    //                 blocks[x][y][z].Translate(x,y,z);
    //             }
    //         }
    //     }
    // }
}



// Render
// The render function gets called once per loop
void SDLGraphicsProgram::Render() {
    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, m_screenWidth, m_screenHeight);
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    // Clear color buffer and Depth Buffer
    // Remember that the 'depth buffer' is our
    // z-buffer that figures out how far away items are every frame
    // and we have to do this every frame!
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Render blocks
    builder.Render(blocksArray);

    SDL_Delay(50);
}


//Loops forever!
void SDLGraphicsProgram::Loop() {
    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
	bool showWireframe = false;
    float cameraSpeed = 0.5f;
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();
    // While application is running
    while (!quit) {
     	 //Handle events on queue
		while (SDL_PollEvent( &e ) != 0) {
        	// User posts an event to quit
	        // An example is hitting the "x" in the corner of the window.
    	    if (e.type == SDL_QUIT) {
        		quit = true;
	        }
            if (e.type == SDL_MOUSEMOTION) {
                int mouseX = e.motion.x; // TODO: move out of loop
                int mouseY = e.motion.y;
                Camera::Instance().MouseLook(mouseX, mouseY);
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = e.button.x;
                int mouseY = e.motion.y;
                if (e.button.button == SDL_BUTTON_LEFT) {
                    GetSelection(mouseX, mouseY, SDL_BUTTON_LEFT);
                    std::cout << "Mouse X: " << mouseX << " Y: " << mouseY << std::endl;
                }
                else if (e.button.button == SDL_BUTTON_RIGHT) {
                    GetSelection(mouseX, mouseY, SDL_BUTTON_RIGHT);
                    std::cout << "Mouse X: " << mouseX << " Y: " << mouseY << std::endl;
                }
            }
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_q:
						quit = true;
						break;
					case SDLK_i:
						if (showWireframe) {
							glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Render filled in
							showWireframe = false;
						}
						else {
							glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Render wireframe model
							showWireframe = true;
						}
						break;
                    case SDLK_a:
                        Camera::Instance().MoveLeft(cameraSpeed);
                        break;
                    case SDLK_d:
                        Camera::Instance().MoveRight(cameraSpeed);
                        break;
                    case SDLK_w:
                        Camera::Instance().MoveForward(cameraSpeed);
                        break;
                    case SDLK_s:
                        Camera::Instance().MoveBackward(cameraSpeed);
                        break;
                    case SDLK_SPACE:
                        Camera::Instance().MoveUp(cameraSpeed);
                        break;
                    case SDLK_LALT:
                        Camera::Instance().MoveDown(cameraSpeed);
                        break;
                    case SDLK_p:
                        std::cout << "Position: "
                        << Camera::Instance().GetEyeXPosition() << " "
                        << Camera::Instance().GetEyeYPosition() << " "
                        << Camera::Instance().GetEyeZPosition() << " "
                        << std::endl;
                        break;
                    case SDLK_1:
                        activeBlock = Dirt;
                        break;
                    case SDLK_2:
                        activeBlock = Grass;
                        break;
                    case SDLK_3:
                        activeBlock = Plank;
                        break;
                    case SDLK_4:
                        activeBlock = Brick;
                        break;
                    case SDLK_5:
                        activeBlock = Cobblestone;
                        break;
                    case SDLK_6:
                        activeBlock = Sandstone;
                        break;
                    case SDLK_7:
                        activeBlock = Mossystone;
                        break;
                    case SDLK_8:
                        activeBlock = LightBlueWool;
                        break;
                    case SDLK_9:
                        activeBlock = OrangeWool;
                        break;
				}
			}
      	} // End SDL_PollEvent loop.

		// Update our scene
		Update();
		// Render using OpenGL
	    Render(); 	// TODO: potentially move this depending on your logic
					// for how you handle drawing a triangle or rectangle.
      	//Update screen of our specified window
      	SDL_GL_SwapWindow(GetSDLWindow());
    }

    //Disable text input
    SDL_StopTextInput();
}


void SDLGraphicsProgram::GetSelection(int mouseX, int mouseY, int clickType) {
    // TODO: Does this fit better in selection buffer class?
    // TODO: USE API TRACE TO SHOW FRAME BUFFER
    // TODO: distance check for selection
    // TODO: Better design is to have block builder take in shader to update
    selectionBuffer.Bind();
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glm::mat4 m_projectionMatrix = glm::perspective(45.0f, (float) m_screenWidth/(float)m_screenHeight, 0.1f, 100.0f);
    int blockIndex = 0;
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            for (int z = 0; z < 16; z++) {
                BlockData block = blocksArray.getBlock(x, y, z);
                if (block.isVisible) {
                    std::cout << "Should only be once" << std::endl;
                    // Update(block, 1280, 720);
                    // TODO: do this in blockbuilder pass in shader used
                    selectionBuffer.m_shader.SetUniformMatrix4fv("model", block.m_transform.GetTransformMatrix());
                    selectionBuffer.m_shader.SetUniformMatrix4fv("view", &Camera::Instance().GetWorldToViewmatrix()[0][0]);
                    selectionBuffer.m_shader.SetUniformMatrix4fv("projection", &m_projectionMatrix[0][0]);
                    for (int i = 0; i <= 30; i += 6) {
                        int r = ((blockIndex + (i/6)) & 0x000000FF) >>  0; // Convert block index to 3 digits from 0-255
                        int g = ((blockIndex + (i/6)) & 0x0000FF00) >>  8; // http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-an-opengl-hack/
                        int b = ((blockIndex + (i/6)) & 0x00FF0000) >> 16;
                        selectionBuffer.m_shader.SetUniform4f("blockColor", r/255.0f, g/255.0f, b/255.0f, 1.0f);
                        // blockIndex++;
                        glDrawElements(GL_TRIANGLES,
                            6,   // The number of indices, not triangles.
                            GL_UNSIGNED_INT,    // Make sure the data type matches
                            (void*)(i * sizeof(GLuint)));           // Offset pointer to the data. nullptr
                                            // because we are currently bound:
                    }

                    // glDrawElements(GL_TRIANGLES,
                    //     builder.m_indices.size(),   // The number of indices, not triangles.
                    //     GL_UNSIGNED_INT,    // Make sure the data type matches
                    //     nullptr);           // Offset pointer to the data. nullptr
                    //                         // because we are currently bound:
                }
                blockIndex+=6;
            }
        }
    }
    // TODO: change to center screen
    // TODO: Make sure background color can't be selected
    int selectedBlockIndex = selectionBuffer.ReadPixel(mouseX, m_screenHeight - mouseY - 1);
    selectionBuffer.Unbind();
    int face = selectedBlockIndex % 6;
    int blockID = selectedBlockIndex - face;
    selectedBlockIndex = blockID / 6;
    std::cout << "\nSelected index: " << selectedBlockIndex << std::endl;
    std::cout << "Block ID: " << blockID << std::endl;
    std::cout << "Face: " << face << std::endl;
    if (selectedBlockIndex > 4096* 8) {
        return;
    }
    int z = selectedBlockIndex % 16;
    int y = (selectedBlockIndex / 16) % 16;
    int x = selectedBlockIndex / 256;
    std::cout << "Block X: " << x << " Y: " << y << " Z: " << z << std::endl;
    if (clickType == SDL_BUTTON_LEFT) {
        std::cout << "Handling left click" << std::endl;
        blocksArray.getBlock(x, y, z).isVisible = false;
    }
    // debug face selection
    if (clickType == SDL_BUTTON_RIGHT) {
        std::cout << "Handling right click" << std::endl;
        if (face == 0) {
            std::cout << "Front" << std::endl;
            z += 1;
        }
        else if (face == 1) {
            std::cout << "Back" << std::endl;
            z -= 1;
        }
        else if (face == 2) {
            std::cout << "Top" << std::endl;
            y += 1;
        }
        else if (face == 3) {
            std::cout << "Bot" << std::endl;
            y -= 1;
        }
        else if (face == 4) {
            std::cout << "Right" << std::endl;
            x += 1;
        }
        else if (face == 5) {
            std::cout << "Left" << std::endl;
            x -= 1;
        }
        blocksArray.getBlock(x, y, z).blockType = activeBlock;
        blocksArray.getBlock(x, y, z).isVisible = true;
    }
}


// Get Pointer to Window
SDL_Window* SDLGraphicsProgram::GetSDLWindow() {
  return m_window;
}

// Helper Function to get OpenGL Version Information
void SDLGraphicsProgram::GetOpenGLVersionInfo() {
	SDL_Log("(Note: If you have two GPU's, make sure the correct one is selected)");
	SDL_Log("Vendor: %s",(const char*)glGetString(GL_VENDOR));
	SDL_Log("Renderer: %s",(const char*)glGetString(GL_RENDERER));
	SDL_Log("Version: %s",(const char*)glGetString(GL_VERSION));
	SDL_Log("Shading language: %s",(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
}
