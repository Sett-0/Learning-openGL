#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <random>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int LOG_SIZE = 512;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main() {\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

// Generating random double in range
std::random_device rd; // obtain a random number from hardware
std::mt19937 gen(rd()); // seed the generator
std::uniform_real_distribution<> distr(0, 1); // define the range

int main() {
	glfwInit();

	// Using openGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Using core profile for openGL
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// For MacOS
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Learn OpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD!" << std::endl;
		return -2;
	}

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// CREATING SHADERS
	// Vertex Shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Checking if shader compilation went fine
	int success;
	char infoLog[LOG_SIZE];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, LOG_SIZE, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment Shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Checking if shader compilation went fine
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, LOG_SIZE, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Linking the created shaders into one program (aka pipline ?)
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Checking if linking shaders into program went fine
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, LOG_SIZE, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Deleting the shader objects since we don't need them anymore
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// DRAWING AN OBJECT
	// Triangle vertices in Screen Normalized Coordinates (each axis, x, y and z, lies in the range (-1, 1))
	// Rectangle
	float vertices[] = {
		 0.5f,  0.5f, 0.0f, // top right
		-0.5f,  0.5f, 0.0f, // top left
		-0.5f, -0.5f, 0.0f, // bottom left
		 0.5f, -0.5f, 0.0f, // bottom right
	};

	unsigned int indices[] = { // note that we start from 0!
		0, 1, 2,
		2, 3, 0,
	};

	// Allocating memory on GPU
	// Bind the Vertex Array Object first and set vertex buffer(s), and then convert vertex atribute(s)
	unsigned int vertexBufferObject, vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glGenBuffers(1, &vertexBufferObject);

	// Creating Element Buffer Object
	unsigned int ElementBufferObject;
	glGenBuffers(1, &ElementBufferObject);

	// Telling openGL how to interpret the vertex data
	// Bind Vertex Array Object
	glBindVertexArray(vertexArrayObject);
	// Copy our vertices array in vertex buffer for openGL to use 
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind the Element Buffer Object
	// Copy our indices array in element buffer for openGL to use 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Set the vertex attributes pointers
	// args: pointer index of current VAO, n in vecn, data type of each component in the array, normalize (bool), stride, first data element offset pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
 
	// note that this is allowed, the call to glVertexAttribPointer registered VBO 
	// as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, 
	// but this rarely happens. Modifying other VAOs requires a call to glBindVertexArray anyways 
	// so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);
	
	// ---> TLDR <---
	// In VBO (vertex buffer object) we can store vertices data (position, color, texture coordinates),
	// in EBO (element buffer object) we store indexes which represet line drawing order,
	// in VAO (vertex array object) we specify how to read our data from VBO (again, which numbers are coordinates, which are colors etc).
	// VBO, EBO and VAO are stored on the GPU!
	// We also use binding functions to specify which BAO, EBO or VAO we are using right now.
	// For different VBO data we need to use separate VAOs (for ex., we bind one VAO for vertex data and a second one for the colors).

	// RENDER LOOP
	// Initial background color
	glClearColor(0.07f, 0.07f, 0.07f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Default mode

	while (!glfwWindowShouldClose(window)) {
		// INPUT
		processInput(window);

		// RENDER
		// Draw our first triangle
		// Activating the program object to render it
		glUseProgram(shaderProgram);
		glBindVertexArray(vertexArrayObject); // seeing as we only have a single VAO there's no need to bind it
											  // every time, but we'll do it to keep things a bit more organized
		// args: drawing mode, starting index, how many
		// glDrawArrays(GL_TRIANGLES, 0, 3);
		//glBindVertexArray(0); // no need to unbind it every time
		
		//args: drawing mode, how many, indices type, offset in EBO
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Optional: de-allocate all resources once they're outlived their purpouse
	glDeleteVertexArrays(1, &vertexArrayObject);
	glDeleteBuffers(1, &vertexBufferObject);
	glDeleteProgram(shaderProgram);

	// GLFW: terminate, clearing all previously allocated GLFW resources
	glfwTerminate();
	return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		glClearColor((float)distr(gen), (float)distr(gen), (float)distr(gen), 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		glClear(GL_COLOR_BUFFER_BIT);
	}
}