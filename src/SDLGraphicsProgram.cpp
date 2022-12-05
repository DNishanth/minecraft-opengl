#include "SDLGraphicsProgram.hpp"
#include "Camera.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

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
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else{
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
		if( m_window == NULL ){
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		//Create an OpenGL Graphics Context
		m_openGLContext = SDL_GL_CreateContext( m_window );
		if( m_openGLContext == NULL){
			errorStream << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		// Initialize GLAD Library
		if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
			errorStream << "Failed to iniitalize GLAD\n";
			success = false;
		}

		//Initialize OpenGL
		if(!InitGL()){
			errorStream << "Unable to initialize OpenGL!\n";
			success = false;
		}
  	}

    // If initialization did not work, then print out a list of errors in the constructor.
    if(!success){
        errorStream << "SDLGraphicsProgram::SDLGraphicsProgram - Failed to initialize!\n";
        std::string errors=errorStream.str();
        SDL_Log("%s\n",errors.c_str());
    }else{
        SDL_Log("SDLGraphicsProgram::SDLGraphicsProgram - No SDL, GLAD, or OpenGL, errors detected during initialization\n\n");
    }

	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN); // Uncomment to enable extra debug support!
	GetOpenGLVersionInfo();


    // Setup our objects
    m_object = new Object();
    m_object2 = new Object();
    m_object3 = new Object();
    m_object4 = new Object();
    m_object5 = new Object();
    m_object6 = new Object();
    m_object7 = new Object();
    m_object->MakeTexturedQuad("");
    m_object2->MakeTexturedQuad("");
    m_object3->MakeTexturedQuad("");
    m_object4->MakeTexturedQuad("");
    m_object5->MakeTexturedQuad("");
    m_object6->MakeTexturedQuad("");
    m_object7->MakeTexturedQuad("");
}


// Proper shutdown of SDL and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram(){
    // Reclaim all of our objects
    delete m_object;

    //Destroy window
	SDL_DestroyWindow( m_window );
	// Point m_window to NULL to ensure it points to nothing.
	m_window = NULL;
	//Quit SDL subsystems
	SDL_Quit();
}


// Initialize OpenGL
// Setup any of our shaders here.
bool SDLGraphicsProgram::InitGL(){
	//Success flag
	bool success = true;

    // Setup our OpenGL State machine
    // TODO: Read this
    // The below command is new!
    // What we are doing, is telling opengl to create a depth(or Z-buffer) 
    // for us that is stored every frame.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
	return success;
}


// Update OpenGL
void SDLGraphicsProgram::Update(){

    static float rot = 0;
    rot+=0.1;
    if(rot>360){rot=0;}


    m_object->GetTransform().LoadIdentity();
    m_object->GetTransform().Translate(0.0f,0.0f,-8.0f);
    // m_object->GetTransform().Rotate(rot,0.0f,1.0f,0.0f);
    // m_object->GetTransform().Scale(2.0f,2.0f,2.0f);
    m_object->Update(m_screenWidth,m_screenHeight);

    m_object2->GetTransform().LoadIdentity();
    m_object2->GetTransform().Translate(0.0f,0.0f,-8.0f);
    m_object2->Update(m_screenWidth,m_screenHeight);
    m_object3->GetTransform().LoadIdentity();
    m_object3->GetTransform().Translate(-2.0f,0.0f,-8.0f);
    m_object3->Update(m_screenWidth,m_screenHeight);
    m_object4->GetTransform().LoadIdentity();
    m_object4->GetTransform().Translate(4.0f,0.0f,-8.0f);
    m_object4->Update(m_screenWidth,m_screenHeight);
    m_object5->GetTransform().LoadIdentity();
    m_object5->GetTransform().Translate(2.0f,0.0f,-8.0f);
    m_object5->Update(m_screenWidth,m_screenHeight);
    m_object6->GetTransform().LoadIdentity();
    m_object6->GetTransform().Translate(6.0f,0.0f,-8.0f);
    m_object6->Update(m_screenWidth,m_screenHeight);
    m_object7->GetTransform().LoadIdentity();
    m_object7->GetTransform().Translate(0.0f,2.0f,-8.0f);
    m_object7->Update(m_screenWidth,m_screenHeight);
}



// Render
// The render function gets called once per loop
void SDLGraphicsProgram::Render(){
    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, m_screenWidth, m_screenHeight);
    glClearColor( 0.2f, 0.2f, 0.2f, 1.f );
    // TODO: Read this
    // Clear color buffer and Depth Buffer
    // Remember that the 'depth buffer' is our
    // z-buffer that figures out how far away items are every frame
    // and we have to do this every frame!
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Render our object
    m_object->Render();
    m_object2->Render();
    m_object3->Render();
    m_object4->Render();
    m_object5->Render();
    m_object6->Render();
    m_object7->Render();
    // Note: This line is equivalent to:
    // (*m_object).Render(); // But the easier style
    // is to use the '->' which dereferences and then
    // selects the field/function with the '.' operator.

    SDL_Delay(50);
}


//Loops forever!
void SDLGraphicsProgram::Loop(){
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
    while(!quit){
     	 //Handle events on queue
		while(SDL_PollEvent( &e ) != 0) {
        	// User posts an event to quit
	        // An example is hitting the "x" in the corner of the window.
    	    if(e.type == SDL_QUIT) {
        		quit = true;
	        }
            if(e.type==SDL_MOUSEMOTION) {
                int mouseX = e.motion.x;
                int mouseY = e.motion.y;
                Camera::Instance().MouseLook(mouseX, mouseY);
            }
			if (e.type == SDL_KEYDOWN) {
				switch(e.key.keysym.sym) {
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


// Get Pointer to Window
SDL_Window* SDLGraphicsProgram::GetSDLWindow(){
  return m_window;
}

// Helper Function to get OpenGL Version Information
void SDLGraphicsProgram::GetOpenGLVersionInfo(){
	SDL_Log("(Note: If you have two GPU's, make sure the correct one is selected)");
	SDL_Log("Vendor: %s",(const char*)glGetString(GL_VENDOR));
	SDL_Log("Renderer: %s",(const char*)glGetString(GL_RENDERER));
	SDL_Log("Version: %s",(const char*)glGetString(GL_VERSION));
	SDL_Log("Shading language: %s",(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
}
