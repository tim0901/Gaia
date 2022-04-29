
#include "OpenGLViewport.h"

#include "GLshader.h"


#ifndef __APPLE__

#pragma warning( push, 0 )
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // !STB_IMAGE_IMPLEMENTATION
#include "../Dependencies/stb_image.h"
#pragma warning( pop )

#else

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // !STB_IMAGE_IMPLEMENTATION
#include "../Dependencies/stb_image.h"
#pragma clang diagnostic pop

#endif
//OpenGL Defines
void error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool WGLExtensionSupported(const char *extension_name);

//GLFW error callback
void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
    std::cout << "test" << std::endl;
    std::cout << description << std::endl;

}

void OpenGLViewport::InitialiseViewport(){
    
    
    //initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Deals with parameter required for compilation on OS X 
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	//Scales window size to image size, but not larger than monitor's resolution
    int xDim = 0;
    int yDim = 0;
    
#ifndef __APPLE__ // These don't work on MacOS, so MacOS assumes a resolution of 1080p when using OpenGL
    xDim = GetSystemMetrics(SM_CXSCREEN);
    yDim = GetSystemMetrics(SM_CYSCREEN);
#else
	xDim = 1920;
	yDim = 1080;
#endif
    
    // While maintaining the correct aspect ratio, scale the image to fit on the screen
	if (image->xDim > xDim)
	{
		yDim = (static_cast<float>(image->yDim) / static_cast<float>(image->xDim)) * static_cast<float>(xDim);
	}
	else if (image->yDim > yDim)
	{
		xDim = (static_cast<float>(image->xDim) / static_cast<float>(image->yDim)) * static_cast<float>(yDim);
	}
	else {
		xDim = image->xDim;
		yDim = image->yDim;
	}

    // Window name
	std::string windowname = "Gaia - " + image->fileName;

	window = glfwCreateWindow(xDim, yDim, windowname.c_str(), NULL, NULL);

    glfwMakeContextCurrent(window);

    
    // Check the window has actually appeared
	if (window == NULL)
	{
        if(!glfwInit()){
            std::cout << "it was me... GLFW!" << std::endl;
            glfwSetErrorCallback(error_callback); //retrieve GLFW errors
        }
        
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
    
    
	int n(NULL);

    // Set window icon
	GLFWimage icons[1];
	icons[0].pixels = stbi_load("icon.png", &icons->width, &icons->height, &n, 4);
	glfwSetWindowIcon(window, 1, icons);
	stbi_image_free(icons[0].pixels);
	
	//Resizes viewport when window is resized
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetWindowAspectRatio(window, image->xDim, image->yDim);
    
	//Initializes GLAD which manages function pointers for OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	//Shaders
	//
	GLshader ourShader("OpenGL/shader.vert", "OpenGL/shader.frag");

	//Tells OpenGL the size of the rendering window, so OpenGL can display correctly
	//First 2 ints: coords of lower left corner. Last 2 ints: width and height of window
	glViewport(0, 0, xDim, yDim);

	//Defines vertices
	float vertices[] = {
		// positions          // colors           // texture coords
		 1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};

	//Defines indices
	unsigned int indices[] = {//note: starts from 0
		0, 1, 3,
		1, 2, 3
	};

	//Stores vertices in GPU memory
	//
	//Vertex array object - stores states of verticies for a single object, allowing easy recall
	//Element buffer object
	//Assigns OpenGL ID
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	//Bind the vertex array first, then bind and set vertex buffers, then configure vertex attributes
	glBindVertexArray(VAO);

	//Binds buffer to type 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Copies data from vertices array to buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Tells OpenGL how to interpret vertex data

	//Position 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//Texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//Assigns vertex attribute 0 to the previously defined VBO
	glEnableVertexAttribArray(2);

#ifndef __APPLE__
	//VSYNC - to stop the window running at 1000fps
	//
	PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT = NULL;
	PFNWGLGETSWAPINTERVALEXTPROC    wglGetSwapIntervalEXT = NULL;

	if (WGLExtensionSupported("WGL_EXT_swap_control"))
	{
		// Extension is supported, init pointers.
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

		// this is another function from WGL_EXT_swap_control extension
		wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");

		//Enables vsync
		wglSwapIntervalEXT(1);
	}
#else
    //Enables vsync
    glfwSwapInterval(1);
#endif

	// We want to run the window using the sRGB colour space
	glEnable(GL_FRAMEBUFFER_SRGB);
    
    glfwMakeContextCurrent(window);
    
	//Texture
	//
	//Texture
	glGenTextures(1, &texture);

	ourShader.use();
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture"), 0);


	glActiveTexture(GL_TEXTURE0);//activates texture unit before binding it
	glBindTexture(GL_TEXTURE_2D, texture);

	//Texture Wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Colour outside borders
	float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//Texture Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, image->xDim, image->yDim, 0, GL_RGB, GL_FLOAT, (GLvoid*)image->data.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	initialised = true;
}

void OpenGLViewport::UpdateView(int argc, const char* argv[]) {

	while (!glfwWindowShouldClose(window)) {
		if (!initialised)
			return;

		//Every shader and rendering call after this will use this program objects

		//Keep window updated and responsive
		ProcessInput();

		//Render Stuff
		//
		//Generate texture
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->xDim, image->yDim, GL_RGB, GL_FLOAT, (GLvoid*)image->data.data());
#ifndef __APPLE__
		glGenerateMipmap(GL_TEXTURE_2D);
#endif // !__APPLE__

		//Resets window every cycle, stops previous iteration's results being seen
		//Decides colour to be used
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//Fills entire colour buffer with colour defined above
		glClear(GL_COLOR_BUFFER_BIT);

		//Binds textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);//activates texture unit before binding it
		glBindTexture(GL_TEXTURE_2D, texture);

		//ourShader.use();
		//Draw elements in buffer
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//GLFW: swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

//When the user resizes the window, this adjusts the viewport respectively
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#ifndef __APPLE__
//Does this PC support the pointers required to use VSYNC?
bool WGLExtensionSupported(const char *extension_name)
{
	// this is pointer to function which returns pointer to string with list of all wgl extensions
	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

	// determine pointer to wglGetExtensionsStringEXT function
	_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");

	if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
	{
		// string was not found
		return false;
	}

	// extension is supported
	return true;
}
#endif

//Reads shader details from files
std::string readFile(const char *filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}
