#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "Shader.h"
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void Render(GLFWwindow* window, Shader shader);
void BindTexture(unsigned int& texture, const char* imagePath);
void CalculateTime();


Shader InitShader();

using namespace glm;



// setting
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

unsigned int shaderProgram;
unsigned int VAO;
unsigned int VBO;

unsigned int texture1;
unsigned int texture2;


vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(.0f, .0f, -1.0f);
vec3 cameraUp = vec3(.0f, .0f, .0f);

float deltaTime = 0.0f;


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// init window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Faild to initialize GLAD" << std::endl;
		return -1;
	}

	Shader shader = InitShader();

	Render(window, shader);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}

Shader InitShader() {

	Shader ourShader("vertex.glsl", "fragment.glsl");

	float vertices2[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	BindTexture(texture1, "image.jpg");
	BindTexture(texture2, "wall.jpg");

	ourShader.use(); // don't forget to activate the shader before setting uniforms!  
	ourShader.setInt("texture1", 0); // or with shader class
	ourShader.setInt("texture2", 1); // or with shader class

	glEnable(GL_DEPTH_TEST);

	return ourShader;
}

void BindTexture(unsigned int& texture, const char* imagePath)
{
	// texture attribute
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to Load Texture: " << imagePath << std::endl;
	}

	stbi_image_free(data);
}

void Render(GLFWwindow* window, Shader shader) {

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		// received input
		CalculateTime();
		processInput(window);

		//rendering commands here
		glClearColor(.5, .5, .5, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		mat4 projection = mat4(1.0f);
		mat4 view = mat4(1.0f);

	
		vec3 cameraTarget = vec3(.0f, .0f, .0f);
		vec3 cameraDirection = normalize(cameraPos - cameraTarget);

		vec3 up = vec3(.0f, 1.0f, .0f);
		vec3 cameraRight = normalize(cross(up, cameraDirection));

		cameraUp = cross(cameraDirection, cameraRight);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		//view = translate(view, viewPos);

		//view = rotate(view, radians(viewVel), vec3(0.0f, 1.0f, 0.0f));

		projection = perspective(radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));

		unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));


		// world space positions of our cubes
		vec3 cubePositions[] = {
			vec3(0.0f,  0.0f,  0.0f),
			vec3(2.0f,  5.0f, -15.0f),
			vec3(-1.5f, -2.2f, -2.5f),
			vec3(-3.8f, -2.0f, -12.3f),
			vec3(2.4f, -0.4f, -3.5f),
			vec3(-1.7f,  3.0f, -7.5f),
			vec3(1.3f, -2.0f, -2.5f),
			vec3(1.5f,  2.0f, -2.5f),
			vec3(1.5f,  0.2f, -1.5f),
			vec3(-1.3f,  1.0f, -1.5f)
		};


		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			mat4 model = mat4(1.0f);
			model = translate(model, cubePositions[i]);
			float angle = 20.0f * i * glfwGetTime();
			model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));

			unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		////glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// check and call events, and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

float moveSpeed = .1;

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	const float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

}

float lastFrameTime = 0.0f;
void CalculateTime() 
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrameTime;
	lastFrameTime = currentFrame;
}

