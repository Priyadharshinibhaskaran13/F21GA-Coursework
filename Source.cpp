// 3D Graphics and Animation - Main Template
// Visual Studio 2019
// Last Changed 01/10/2019

#pragma comment(linker, "/NODEFAULTLIB:MSVCRT")
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <GL/glew.h>			// Add library to extend OpenGL to newest version
#include <GLFW/glfw3.h>			// Add library to launch a window
#include <GLM/glm.hpp>			// Add helper maths library
#include <GLM/gtx/transform.hpp>

#include <stb_image.h>			// Add library to load images for textures

#include "Mesh.h"				// Simplest mesh holder and OBJ loader - can update more - from https://github.com/BennyQBD/ModernOpenGLTutorial


// MAIN FUNCTIONS
void setupRender();
void startup();
void update(GLfloat currentTime);
void render(GLfloat currentTime);
void endProgram();

// HELPER FUNCTIONS OPENGL
void hintsGLFW();
string readShader(string name);
void checkErrorShader(GLuint shader);
void errorCallbackGLFW(int error, const char* description);
void debugGL();
static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam);

// CALLBACK FUNCTIONS FOR WINDOW
void onResizeCallback(GLFWwindow* window, int w, int h);
void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow* window, double x, double y);
void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);


// VARIABLES
GLFWwindow* window;											// Keep track of the window
int				windowWidth = 640;
int				windowHeight = 480;
bool			running = true;									// Are we still running?
glm::mat4		proj_matrix;									// Projection Matrix
glm::vec3		cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);	// Week 5 lecture
glm::vec3		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float           aspect = (float)windowWidth / (float)windowHeight;
float			fovy = 45.0f;
bool			keyStatus[1024];
GLfloat			deltaTime = 0.0f;
GLfloat			lastTime = 0.0f;
GLuint			program;
GLint			proj_location;
glm::vec3		modelPosition;
glm::vec3		modelRotation;
Mesh cube, obj1,obj2, obj3,obj4, obj5, obj6, obj7, obj8, obj9 , obj10, obj11, obj12, obj13, obj14, obj15, obj16 ;		// Add a cube object
GLuint          texture[8];

int main()
{
	if (!glfwInit()) {							// Checking for GLFW
		cout << "Could not initialise GLFW...";
		return 0;
	}

	glfwSetErrorCallback(errorCallbackGLFW);	// Setup a function to catch and display all GLFW errors.

	hintsGLFW();								// Setup glfw with various hints.		

												// Start a window using GLFW
	string title = "My OpenGL Application";

	// Fullscreen
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	// windowWidth = mode->width; windowHeight = mode->height; //fullscreen
	// window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), glfwGetPrimaryMonitor(), NULL); // fullscreen

	// Window
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
	if (!window) {								// Window or OpenGL context creation failed
		cout << "Could not initialise GLFW...";
		endProgram();
		return 0;
	}

	glfwMakeContextCurrent(window);				// making the OpenGL context current

												// Start GLEW (note: always initialise GLEW after creating your window context.)
	glewExperimental = GL_TRUE;					// hack: catching them all - forcing newest debug callback (glDebugMessageCallback)
	GLenum errGLEW = glewInit();
	if (GLEW_OK != errGLEW) {					// Problems starting GLEW?
		cout << "Could not initialise GLEW...";
		endProgram();
		return 0;
	}

	debugGL();									// Setup callback to catch openGL errors.	

												// Setup all the message loop callbacks.
	glfwSetWindowSizeCallback(window, onResizeCallback);		// Set callback for resize
	glfwSetKeyCallback(window, onKeyCallback);					// Set Callback for keys
	glfwSetMouseButtonCallback(window, onMouseButtonCallback);	// Set callback for mouse click
	glfwSetCursorPosCallback(window, onMouseMoveCallback);		// Set callback for mouse move
	glfwSetScrollCallback(window, onMouseWheelCallback);		// Set callback for mouse wheel.
																//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	// Set mouse cursor. Fullscreen
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Set mouse cursor FPS fullscreen.

	setupRender();								// setup some render variables.
	startup();		// Setup all necessary information for startup (aka. load texture, shaders, models, etc).
	//updateInput(window);
	do {										// run until the window is closed
		GLfloat currentTime = (GLfloat)glfwGetTime();		// retrieve timelapse
		deltaTime = currentTime - lastTime;		// Calculate delta time
		lastTime = currentTime;					// Save for next frame calculations.
		glfwPollEvents();						// poll callbacks
		update(currentTime);					// update (physics, animation, structures, etc)
		render(currentTime);					// call render function.

		glfwSwapBuffers(window);				// swap buffers (avoid flickering and tearing)

		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);	// exit if escape key pressed
		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);

	endProgram();			// Close and clean everything up...

	cout << "\nPress any key to continue...\n";
	cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}

void errorCallbackGLFW(int error, const char* description) {
	cout << "Error GLFW: " << description << "\n";
}

void hintsGLFW() {
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);			// Create context in debug mode - for debug message callback
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void endProgram() {
	glfwMakeContextCurrent(window);		// destroys window handler
	glfwTerminate();	// destroys all windows and releases resources.
}

void setupRender() {
	glfwSwapInterval(1);	// Ony render when synced (V SYNC)

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 2);
	glfwWindowHint(GLFW_STEREO, GL_FALSE);
}

void startup() {
	
	// loading textures to the product
	glGenTextures(10  , texture);
	int iWidth, iHeight, iChannels;

	string name = "white.jpg";
	unsigned char* iData = stbi_load(name.c_str(), &iWidth, &iHeight, &iChannels, 0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, iWidth, iHeight);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, GL_RGB, GL_UNSIGNED_BYTE, iData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	obj3.LoadModel("sphere1.obj");
	obj4.LoadModel("sphere2.obj");
	obj5.LoadModel("sphere3.obj");
	obj6.LoadModel("torus.obj");
	obj7.LoadModel("plantonic.obj");

	free(iData);

	string name1 = "brown.jpg";
	iData = stbi_load(name1.c_str(), &iWidth, &iHeight, &iChannels, 0);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, iWidth, iHeight);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, GL_RGB, GL_UNSIGNED_BYTE, iData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	obj1.LoadModel("cube(bag).obj");
	obj2.LoadModel("Disc(bag).obj");

	free(iData);

	string name2 = "red.jpg";
	iData = stbi_load(name2.c_str(), &iWidth, &iHeight, &iChannels, 0);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, iWidth, iHeight);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, GL_RGB, GL_UNSIGNED_BYTE, iData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	obj8.LoadModel("cone.obj");

	free(iData);

	string name3 = "black.jpg";
	iData = stbi_load(name3.c_str(), &iWidth, &iHeight, &iChannels, 0);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, iWidth, iHeight);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, GL_RGB, GL_UNSIGNED_BYTE, iData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	obj9.LoadModel("cylinder3.obj");
	obj10.LoadModel("cylinder2.obj");
	obj11.LoadModel("eye1.obj");
	obj14.LoadModel("eye2.obj");
	obj12.LoadModel("finger1.obj");
	obj15.LoadModel("finger2.obj");

	free(iData);

	string name4 = "Yellow.jpg";
	iData = stbi_load(name4.c_str(), &iWidth, &iHeight, &iChannels, 0);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, iWidth, iHeight);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, GL_RGB, GL_UNSIGNED_BYTE, iData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	obj13.LoadModel("nose.obj");

	free(iData);
	string name5 = "images.jpg";
	iData = stbi_load(name5.c_str(), &iWidth, &iHeight, &iChannels, 0);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, iWidth, iHeight);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, GL_RGB, GL_UNSIGNED_BYTE, iData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	obj16.LoadModel("background.obj");
	
	//int iWidth1, iHeight1, iChannels1;
	//unsigned char* iData1 = stbi_load(name1.c_str(), &iWidth1, &iHeight1, &iChannels1, 0);
	//glBindTexture(GL_TEXTURE_2D, texture[1]);
	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, iWidth1, iHeight1);
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth1, iHeight1, GL_RGB, GL_UNSIGNED_BYTE, iData1);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glGenerateMipmap(GL_TEXTURE_2D);

	// Load main object model and shaders
	//cube.LoadModel("F21GA_Snowman_OBJFile.obj");

	

	program = glCreateProgram();

	string vs_text = readShader("vs_model.glsl"); const char* vs_source = vs_text.c_str();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);
	checkErrorShader(vs);
	glAttachShader(program, vs);

	string fs_text = readShader("fs_model.glsl"); const char* fs_source = fs_text.c_str();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);
	checkErrorShader(fs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glUseProgram(program);

	// Start from the centre
	modelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	modelRotation = glm::vec3(0.0f, 0.0f, 0.0f);

	// A few optimizations.
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Calculate proj_matrix for the first time.
	aspect = (float)windowWidth / (float)windowHeight;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void update(GLfloat currentTime) {
	if (keyStatus[GLFW_KEY_LEFT])			modelRotation.y += 0.05f;
	if (keyStatus[GLFW_KEY_RIGHT])			modelRotation.y -= 0.05f;
	if (keyStatus[GLFW_KEY_UP])				modelRotation.x += 0.05f;
	if (keyStatus[GLFW_KEY_DOWN])			modelRotation.x -= 0.05f;
	if (keyStatus[GLFW_KEY_KP_ADD])			modelPosition.z += 0.10f;
	if (keyStatus[GLFW_KEY_KP_SUBTRACT])	modelPosition.z -= 0.10f;

	//get the top view of the snowman
	if (keyStatus[GLFW_KEY_P]) {
		modelPosition = glm::vec3(0.0f, 0.0f, 1.0f);
		modelRotation = glm::vec3(-5.0f, -5.0f, -5.0f);

	}

}

void render(GLfloat currentTime) {
	glViewport(0, 0, windowWidth, windowHeight);

	
	// Clear colour buffer
	glm::vec4 backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f); glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);

	// Clear deep buffer
	static const GLfloat one = 1.0f; glClearBufferfv(GL_DEPTH, 0, &one);

	// Enable blend
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Use our shader programs
	glUseProgram(program);

	// Setup camera
	glm::mat4 viewMatrix = glm::lookAt(cameraPosition,					// eye
		cameraPosition + cameraFront,	// centre
		cameraUp);						// up

	// Do some translations, rotations and scaling
	
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), modelPosition);
		modelMatrix = glm::rotate(modelMatrix, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));

		glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);

		GLint tex_location = glGetUniformLocation(program, "tex");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glUniform1i(tex_location, 0);

		obj3.Draw();
		obj4.Draw();
		obj5.Draw();
		obj6.Draw();
		obj7.Draw();


		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glUniform1i(tex_location, 1);
		obj1.Draw();
		obj2.Draw();

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glUniform1i(tex_location, 2);
		obj8.Draw();


		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texture[3]);
		glUniform1i(tex_location, 3);
		obj9.Draw();
		obj10.Draw();
		obj11.Draw();
		obj12.Draw();
		obj14.Draw();
		obj15.Draw();

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, texture[4]);
		glUniform1i(tex_location, 4);
		obj13.Draw();

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, texture[5]);
		glUniform1i(tex_location, 5);
		obj16.Draw();
	
}

void onResizeCallback(GLFWwindow* window, int w, int h) {
	windowWidth = w;
	windowHeight = h;

	aspect = (float)w / (float)h;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) keyStatus[key] = true;
	else if (action == GLFW_RELEASE) keyStatus[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

}

void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

}

void onMouseMoveCallback(GLFWwindow* window, double x, double y) {
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);

}

static void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset) {
	int yoffsetInt = static_cast<int>(yoffset);
}

void updateInput(GLFWwindow* window)  {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		modelPosition.z -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		modelPosition.z += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		modelPosition.x -= 0.01f;
	}
}

void debugGL() {
	//Output some debugging information
	cout << "VENDOR: " << (char*)glGetString(GL_VENDOR) << endl;
	cout << "VERSION: " << (char*)glGetString(GL_VERSION) << endl;
	cout << "RENDERER: " << (char*)glGetString(GL_RENDERER) << endl;

	// Enable Opengl Debug
	//glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam) {

	cout << "---------------------opengl-callback------------" << endl;
	cout << "Message: " << message << endl;
	cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << " --- ";

	cout << "id: " << id << " --- ";
	cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		cout << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		cout << "NOTIFICATION";
	}
	cout << endl;
	cout << "-----------------------------------------" << endl;
}

string readShader(string name) {
	string vs_text;
	std::ifstream vs_file(name);

	string vs_line;
	if (vs_file.is_open()) {

		while (getline(vs_file, vs_line)) {
			vs_text += vs_line;
			vs_text += '\n';
		}
		vs_file.close();
	}
	return vs_text;
}

void  checkErrorShader(GLuint shader) {
	// Get log lenght
	GLint maxLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	// Init a string for it
	std::vector<GLchar> errorLog(maxLength);

	if (maxLength > 1) {
		// Get the log file
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		cout << "--------------Shader compilation error-------------\n";
		cout << errorLog.data();
	}
}
