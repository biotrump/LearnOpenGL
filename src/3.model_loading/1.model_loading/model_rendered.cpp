// Std. Includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/freeglut.h>
// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL.h>
#include <learnopengl/filesystem.h>

// Properties
GLuint screenWidth = 800, screenHeight = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//#define VIEWPORT_WIDTH              1280
//#define VIEWPORT_HEIGHT             800

void glut_init(void)
{
	char *myargv[1];
	int myargc = 1;
	myargv[0] = "intel realsense";
	glutInit(&myargc, myargv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
}

void renderBitmapString(float x, float y, void *font, const char *string) {
	const char *c;
	glRasterPos2f(x, y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void drawFps(int m_fps=30)
{
#if 0
	std::string line = "FPS = ";
	line.append(std::to_string(m_fps));
	//glPushAttrib(GL_CURRENT_BIT);
	//glDisable(GL_LIGHTING);
	glColor3f(1.0, 0.f, 0.f);
	renderBitmapString(200, 200, GLUT_BITMAP_8_BY_13, line.c_str());
	//glEnable(GL_LIGHTING);
	glDisable(GL_LIGHTING);
	std::string line = "--- PAUSE ---";
	glColor3f(1.f, 0.f, 0.f);
	renderBitmapString(0.1f, 0.5f, GLUT_BITMAP_TIMES_ROMAN_24, line.c_str());
	glEnable(GL_LIGHTING);
#endif

	glColor3f(1.f, 1.f, 1.f);
	renderBitmapString(0.9f, 0.49f, GLUT_BITMAP_8_BY_13, "Instructions:");
	renderBitmapString(0.9f, 0.46f, GLUT_BITMAP_8_BY_13, "-------------");
	renderBitmapString(0.9f, 0.43f, GLUT_BITMAP_8_BY_13, "Full Hands Mode: Via Command Line - Run \"FF_Hands3DViewer.exe -full\"");
	renderBitmapString(0.9f, 0.40f, GLUT_BITMAP_8_BY_13, "Left Mouse Button - Hold to rotate view");
	renderBitmapString(0.9f, 0.37f, GLUT_BITMAP_8_BY_13, "Right Mouse Button - Click to reset view");
	renderBitmapString(0.9f, 0.34f, GLUT_BITMAP_8_BY_13, "Mouse Wheel Up/Down - Zoom in/out");
	renderBitmapString(0.9f, 0.31f, GLUT_BITMAP_8_BY_13, "F1 - Pause/Play");
	renderBitmapString(0.9f, 0.28f, GLUT_BITMAP_8_BY_13, "F3 - <Cursor Only> Shorten trail");
	renderBitmapString(0.9f, 0.25f, GLUT_BITMAP_8_BY_13, "F4 - <Cursor Only> Extend trail");

	//glRasterPos2i(100, 120);
	//glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	//glutBitmapString(GLUT_BITMAP_HELVETICA_18, "text to render");
}

// The MAIN function, from here we start our application and run our Game loop
int main(int argc, char **argv)
{
    // Init GLFW
    glfwInit();
	//glfw has no "drawing text function", so init glut for drawing text
	glut_init();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, screenWidth, screenHeight);

    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Setup and compile our shaders
    Shader shader("shader.vs", "shader.frag");

    // Load models
    //Model ourModel(FileSystem::getPath("resources/objects/nanosuit/nanosuit.obj").c_str());
	//Model ourModel(FileSystem::getPath("resources/objects/cyborg/cyborg.obj").c_str());
	//Model ourModel(FileSystem::getPath("resources/objects/jeep/jeep1.ms3d").c_str());
	Model ourModel(FileSystem::getPath("resources/objects/harp/harp.3ds").c_str());
	//Model ourModel(FileSystem::getPath("resources/objects/monkey/monkey.obj").c_str());
    // Draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Game loop
    while(!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        Do_Movement();

        // Clear the colorbuffer
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();   // <-- Don't forget this one!
        // Transformation matrices
        glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Draw the loaded model
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        ourModel.Draw(shader);       

		drawFps();
        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

#pragma region "User input"

// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;	
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; 
    
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}	

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

#pragma endregion