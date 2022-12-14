#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "SDLGraphicsProgram.hpp"
#include "Camera.hpp"
#include "SelectionFrameBuffer.hpp"
#include "Image.hpp"


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
    SDL_SetRelativeMouseMode(SDL_bool::SDL_TRUE);

    builder.InitializeBlockData("texture_atlas_original.png");
    crosshair.MakeTexturedQuad(m_screenWidth, m_screenHeight);
    InitWorld();
    activeBlock = Brick;

    selectionBuffer.Create(m_screenWidth, m_screenHeight);
}


// Proper shutdown of SDL and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram() {
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
    glEnable(GL_CULL_FACE);
	return success;
}

// Initialize world terrain
void SDLGraphicsProgram::InitWorld() {
    Image heightMap("terrain_height.ppm");
    heightMap.LoadPPM(true);
    int height = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int z = 0; z < DEPTH; z++) {
            height = ((float) heightMap.GetPixelR(x, z) / 255.0f) * HEIGHT;
            if (height < HEIGHT) {
                blocksArray.getBlock(x, height, z).isVisible = true;
                // Set block at heightmap value to snow or grass based on elevation
                if (height > 36) {
                    blocksArray.getBlock(x, height, z).blockType = Snow;
                }
                else {
                    blocksArray.getBlock(x, height, z).blockType = Grass;
                }
                // Set column of blocks below heightmap value to dirt
                for (int y = 0; y < height; y++) {
                    blocksArray.getBlock(x, y, z).isVisible = true;
                    blocksArray.getBlock(x, y, z).blockType = Dirt;
                }
            }
        }
    }

    // Hide all surrounded blocks
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int z = 0; z < DEPTH; z++) {
                BlockData& currBlock = blocksArray.getBlock(x, y, z);
                if (blocksArray.isSurrounded(x, y, z)) {
                    currBlock.isVisible = false;
                }
            }
        }
    }
}


// Update OpenGL
void SDLGraphicsProgram::Update() {}



// Render
// The render function gets called once per loop
void SDLGraphicsProgram::Render() {
    // Set background to sky color
    glViewport(0, 0, m_screenWidth, m_screenHeight);
    glClearColor(135.0f/255.0f, 206.0f/255.0f, 235.0f/255.0f, 1.f);
    // Clear color buffer and Depth Buffer
    // Remember that the 'depth buffer' is our
    // z-buffer that figures out how far away items are every frame
    // and we have to do this every frame!
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    crosshair.Render(); // Render crosshair
    builder.Render(blocksArray); // Render blocks
}


//Loops forever!
void SDLGraphicsProgram::Loop() {
    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
	bool showWireframe = false;
    float cameraSpeed = 1.0f;
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
                int mouseX = e.motion.x;
                int mouseY = e.motion.y;
                // std::cout << "Mouse X: " << mouseX << " Y: " << mouseY << std::endl;
                Camera::Instance().MouseLook(mouseX, mouseY);
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = e.button.x;
                int mouseY = e.motion.y;
                    // std::cout << "Mouse X: " << mouseX << " Y: " << mouseY << std::endl;
                if (e.button.button == SDL_BUTTON_LEFT) {
                    MakeSelection(m_screenWidth/2, m_screenHeight/2, SDL_BUTTON_LEFT);
                    // MakeSelection(mouseX, mouseY, SDL_BUTTON_LEFT);
                }
                else if (e.button.button == SDL_BUTTON_RIGHT) {
                    MakeSelection(m_screenWidth/2, m_screenHeight/2, SDL_BUTTON_RIGHT);
                    // MakeSelection(mouseX, mouseY, SDL_BUTTON_RIGHT);
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
                        Camera::Instance().MoveLeft(cameraSpeed, blocksArray);
                        break;
                    case SDLK_d:
                        Camera::Instance().MoveRight(cameraSpeed, blocksArray);
                        break;
                    case SDLK_w:
                        Camera::Instance().MoveForward(cameraSpeed, blocksArray);
                        break;
                    case SDLK_s:
                        Camera::Instance().MoveBackward(cameraSpeed, blocksArray);
                        break;
                    case SDLK_SPACE:
                        Camera::Instance().MoveUp(cameraSpeed, blocksArray);
                        break;
                    case SDLK_LCTRL:
                        Camera::Instance().MoveDown(cameraSpeed, blocksArray);
                        break;
                    case SDLK_p:
                        std::cout << "Position: "
                        << Camera::Instance().GetEyeXPosition() << " "
                        << Camera::Instance().GetEyeYPosition() << " "
                        << Camera::Instance().GetEyeZPosition() << " "
                        << std::endl;
                        {
                            int x = Camera::Instance().GetEyeXPosition();
                            int y = Camera::Instance().GetEyeYPosition();
                            int z = Camera::Instance().GetEyeZPosition();
                            if (blocksArray.isValidBlock(x, y, z) && blocksArray.getBlock(x, y, z).isVisible) {
                                std::cout << "Inside block" << std::endl;
                            }
                        }
                        break;
                    case SDLK_c:
                        Camera::Instance().ToggleCollision();
                        break;
                    case SDLK_l:
                        builder.ToggleLighting();
                        break;
                    case SDLK_r:
                        builder.ReloadShaders();
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
	    Render();
      	//Update screen of our specified window
      	SDL_GL_SwapWindow(GetSDLWindow());
    }

    //Disable text input
    SDL_StopTextInput();
}

// Get selected block from selection frame buffer
// Handle block destroy or placement
void SDLGraphicsProgram::MakeSelection(int mouseX, int mouseY, int clickType) {
    // Render blocks in selection framebuffer
    selectionBuffer.Render(blocksArray, m_screenWidth, m_screenHeight);

    // Read pixel color from framebuffer and convert back to block index
    int selectedBlockIndex = selectionBuffer.ReadPixel(mouseX, m_screenHeight - mouseY - 1) - 1;
    selectionBuffer.Unbind();
    if (selectedBlockIndex == -1) {
        return;
    }
    int face = selectedBlockIndex % 6; // 0 - 5 face of block
    int blockID = selectedBlockIndex - face; //  block starting index
    selectedBlockIndex = blockID / 6; // x y z conversion

    int z = selectedBlockIndex % DEPTH;
    int y = (selectedBlockIndex % (DEPTH * HEIGHT)) / DEPTH;
    int x = selectedBlockIndex / (DEPTH * HEIGHT);
    // std::cout << "Selected index: " << selectedBlockIndex << std::endl;
    // std::cout << "Block ID: " << blockID << std::endl;
    // std::cout << "Face: " << face << std::endl;
    if (clickType == SDL_BUTTON_LEFT) {
        // std::cout << "Selected index: " << selectedBlockIndex << std::endl;
        blocksArray.getBlock(x, y, z).isVisible = false;
        blocksArray.getBlock(x, y, z).blockType = Empty;
        blocksArray.revealSurroundingBlocks(x, y, z);
    }
    // debug face selection
    if (clickType == SDL_BUTTON_RIGHT) {
        if (face == 0) {
            // std::cout << "Front" << std::endl;
            z += 1;
        }
        else if (face == 1) {
            // std::cout << "Back" << std::endl;
            z -= 1;
        }
        else if (face == 2) {
            // std::cout << "Top" << std::endl;
            y += 1;
        }
        else if (face == 3) {
            // std::cout << "Bot" << std::endl;
            y -= 1;
        }
        else if (face == 4) {
            // std::cout << "Right" << std::endl;
            x += 1;
        }
        else if (face == 5) {
            // std::cout << "Left" << std::endl;
            x -= 1;
        }
        if (blocksArray.isValidBlock(x, y, z)) {
            // std::cout << "Placing on Block X: " << x << " Y: " << y << " Z: " << z << std::endl;
            // std::cout << "Block type is: " << blocksArray.getBlock(x, y, z).blockType << std::endl;
            blocksArray.getBlock(x, y, z).blockType = activeBlock;
            blocksArray.getBlock(x, y, z).isVisible = true;
            blocksArray.hideSurroundingBlocks(x, y, z);
        }
        else {
            // std::cout << "Out of bounds block" << std::endl;
        }
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
