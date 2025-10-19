#define _USE_MATH_DEFINES

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <random>
#include "../../Two triangles/shader_s.h"

// Set program to use discrete videocard
typedef unsigned long DWORD;
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 1;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Shader* shaderProgram);
void configureVAOsAndVBOs(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO, float** vertices, unsigned int** indices,
						  const unsigned int* Nv, const unsigned int* Ni);

const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;
int N_ATTRIBUTES;
bool MOUSE_BUTTON_LEFT_PRESSED = false;

// Generating random double in range
std::random_device rd; // obtain a random number from hardware
std::mt19937 gen(rd()); // seed the generator
std::uniform_real_distribution<> distr(0, 1); // define the range

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &N_ATTRIBUTES);
	std::cout << "Maximum number of vertex attributes supported: " << N_ATTRIBUTES << std::endl;

	Shader triangle("shaders/3.3.shader.txt", "shaders/3.3.shader_triangle.txt");
	Shader   circle("shaders/3.3.shader.txt", "shaders/3.3.shader_circle.txt");
	Shader shaderPrograms[] = { circle, triangle };


	// DRAWING AN OBJECT
	float triangleVertices[] = {
		 0.0,  0.0, 0.0, distr(gen), distr(gen), distr(gen),

		 0.5,  1.0, 0.0, distr(gen), distr(gen), distr(gen),
		 1.0,  0.5, 0.0, distr(gen), distr(gen), distr(gen),

		 1.0, -0.5, 0.0, distr(gen), distr(gen), distr(gen),
		 0.5, -1.0, 0.0, distr(gen), distr(gen), distr(gen),

		-0.5, -1.0, 0.0, distr(gen), distr(gen), distr(gen),
		-1.0, -0.5, 0.0, distr(gen), distr(gen), distr(gen),

		-1.0,  0.5, 0.0, distr(gen), distr(gen), distr(gen),
		-0.5,  1.0, 0.0, distr(gen), distr(gen), distr(gen),
	};

	unsigned int triangleIndices[] = {
		0, 1, 2,
		0, 3, 4,
		0, 5, 6,
		0, 7, 8,
	};

	float circleVertices[] = {
		 0.0,            0.0,           0.0, distr(gen), distr(gen), distr(gen),

		 0.0,            1.0,           0.0, distr(gen), distr(gen), distr(gen),
		 cos(M_PI / 4),  sin(M_PI / 4), 0.0, distr(gen), distr(gen), distr(gen),

		 1.0,            0.0,           0.0, distr(gen), distr(gen), distr(gen),
		 cos(M_PI / 4), -sin(M_PI / 4), 0.0, distr(gen), distr(gen), distr(gen),

		 0.0,           -1.0,           0.0, distr(gen), distr(gen), distr(gen),
		-cos(M_PI / 4), -sin(M_PI / 4), 0.0, distr(gen), distr(gen), distr(gen),

		-1.0,            0.0,           0.0, distr(gen), distr(gen), distr(gen),
		-cos(M_PI / 4),  sin(M_PI / 4), 0.0, distr(gen), distr(gen), distr(gen),
	};

	unsigned int circleIndices[] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 7,
		0, 7, 8,
		0, 8, 1,
	};

	float* vertices[] = { circleVertices, triangleVertices };
	unsigned int* indices[] = { circleIndices, triangleIndices };
	const unsigned int Nv[] = { sizeof(circleVertices) / sizeof(float) / 3, sizeof(triangleVertices) / sizeof(float) / 3 };
	const unsigned int Ni[] = { sizeof(circleIndices) / sizeof(unsigned int) / 3, sizeof(triangleIndices) / sizeof(unsigned int) / 3 };
	unsigned int VAO[2], VBO[2], EBO[2];

	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(2, EBO);

	configureVAOsAndVBOs(VAO, VBO, EBO, vertices, indices, Nv, Ni);

	// RENDER LOOP
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Default mode
	
	int colorGradientLocation;
	float r, g, b, time;
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.07f, 0.07f, 0.07f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		processInput(window, shaderPrograms);
		for (int i = 0; i < 2; ++i) {
			shaderPrograms[i].use();
			time = glfwGetTime();
			r =  sin(time) / 4; 
			g =  cos(time) / 4;
			b = -sin(time) / 4;
			shaderPrograms[i].setFloat3("colorGradient", r, g, b);

			glBindVertexArray(VAO[i]);
			glDrawElements(GL_TRIANGLES, 3 * Ni[i], GL_UNSIGNED_INT, 0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(2, EBO);
	shaderPrograms[0].deleteProgram();
	shaderPrograms[1].deleteProgram();

	glfwTerminate();
	return 0;
}

void configureVAOsAndVBOs(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO, float** vertices, unsigned int** indices,
	const unsigned int* Nv, const unsigned int* Ni) {
	for (int i = 0; i < 2; ++i) {
		glBindVertexArray(VAO[i]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
		glBufferData(GL_ARRAY_BUFFER, 3 * Nv[i] * sizeof(float), vertices[i], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * Ni[i] * sizeof(unsigned int), indices[i], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
}


void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, Shader* shaderPrograms) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		MOUSE_BUTTON_LEFT_PRESSED = true;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && MOUSE_BUTTON_LEFT_PRESSED) {
		//int randomColorLocation;
		//glUseProgram(shaderPrograms[0]);
		//randomColorLocation = glGetUniformLocation(shaderPrograms[0], "randomColor");
		//glUniform3fv(randomColorLocation, 1, true, randomColors);

		//glUseProgram(shaderPrograms[1]);
		//randomColorLocation = glGetUniformLocation(shaderPrograms[1], "randomColor");
		//glUniform3fv(randomColorLocation, 1, true, randomColors);

		MOUSE_BUTTON_LEFT_PRESSED = false;
	}
}