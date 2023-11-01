#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "Shader.h"
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void Render(GLFWwindow* window);
void BindTexture(unsigned int& texture, const char* imagePath);
void CalculateTime();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


void InitShader();

using namespace glm;



// setting
const unsigned int SCR_WIDTH = 1270;
const unsigned int SCR_HEIGHT = 720;

unsigned int shaderProgram;
unsigned int lightVAO;

unsigned int VAO;
unsigned int VBO;

unsigned int diffuseMap;
unsigned int specularMap;
unsigned int spotlightMap;

Shader ourShader;
Shader lightShader;

vec3 lightPos = vec3(1.2f, 1.0f, 2.0f);

Camera camera(vec3(0.0f, .0f, 3.0f));

bool firstMouse = true;
float Yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float Pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

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

	// tell GLFW to capture our mouse

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Faild to initialize GLAD" << std::endl;
		return -1;
	}

	InitShader();

	Render(window);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}

void InitShader() {
	ourShader.Init("vert_container.glsl", "frag_container.glsl");
	lightShader.Init("vert_light.glsl", "frag_light.glsl");

	float vertices2[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);


	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

	BindTexture(diffuseMap, "container2.png");
	ourShader.use();
	ourShader.setInt("material.diffuse", 0);

	BindTexture(specularMap, "specular.png");
	ourShader.use();
	ourShader.setInt("material.specular", 1);

	glEnable(GL_DEPTH_TEST);
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
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to Load Texture: " << imagePath << std::endl;
	}

	stbi_image_free(data);
}

void Render(GLFWwindow* window) {

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	int const cubeCount = 20;
	vec3* cubePositions = new vec3[cubeCount];
	for (size_t i = 0; i < cubeCount; i++)
	{
		cubePositions[i] = vec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5);
	}

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};
	
	vec3 pointLightColor[]{
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(1.0f),
	};


	while (!glfwWindowShouldClose(window))
	{
		// received input
		CalculateTime();
		processInput(window);

		//rendering commands here
		glClearColor(.1, .1, .1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();
		ourShader.setVec3("dirLight.direction", vec3(-0.2f, -1.0f, -.3f));
		ourShader.setVec3("dirLight.ambient", vec3(.05f));
		ourShader.setVec3("dirLight.diffuse", vec3(.4f));
		ourShader.setVec3("dirLight.specular", vec3(.5f));
		
		//for (int i = 0; i < 4; i++)
		//{
		//	std::string pointLightName = std::string("pointLights[") + std::string(i + "]");
		//	ourShader.setVec3(pointLightName + ".position", pointLightPositions[i]);
		//	ourShader.setVec3(pointLightName + ".ambient", vec3(0.05f, 0.05f, 0.05f));
		//	ourShader.setVec3(pointLightName + ".diffuse", vec3(0.8f, 0.8f, 0.8f));
		//	ourShader.setVec3(pointLightName + ".specular", vec3(1.0f, 1.0f, 1.0f));
		//	ourShader.setFloat(pointLightName + ".constant", 1.0f);
		//	ourShader.setFloat(pointLightName + ".linear", 0.09f);
		//	ourShader.setFloat(pointLightName + ".quadratic", 0.032f);

		//	std::cout << "construct light " << i << std::endl;
		//}

		// point light 1
		ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		ourShader.setVec3("pointLights[0].ambient",	pointLightColor[0]);
		ourShader.setVec3("pointLights[0].diffuse", vec3(1.0f, 0.0f, 0.0f));
		ourShader.setVec3("pointLights[0].specular", vec3(1.0f, 1.0f, 1.0f));
		ourShader.setFloat("pointLights[0].constant", 1.0f);
		ourShader.setFloat("pointLights[0].linear", 0.09f);
		ourShader.setFloat("pointLights[0].quadratic", 0.032f);
		// point light 2
		ourShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		ourShader.setVec3("pointLights[1].ambient", pointLightColor[1]);
		ourShader.setVec3("pointLights[1].diffuse", vec3(0.0f, 0.0f, 1.0f));
		ourShader.setVec3("pointLights[1].specular", vec3(1.0f, 1.0f, 1.0f));
		ourShader.setFloat("pointLights[1].constant", 1.0f);
		ourShader.setFloat("pointLights[1].linear", 0.09f);
		ourShader.setFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		ourShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		ourShader.setVec3("pointLights[2].ambient", pointLightColor[2]);
		ourShader.setVec3("pointLights[2].diffuse", vec3(0.0f, 1.0f, 0.0f));
		ourShader.setVec3("pointLights[2].specular", vec3(1.0f, 1.0f, 1.0f));
		ourShader.setFloat("pointLights[2].constant", 1.0f);
		ourShader.setFloat("pointLights[2].linear", 0.09f);
		ourShader.setFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		ourShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		ourShader.setVec3("pointLights[3].ambient", pointLightColor[3]);
		ourShader.setVec3("pointLights[3].diffuse", vec3(1.0f));
		ourShader.setVec3("pointLights[3].specular", vec3(1.0f, 1.0f, 1.0f));
		ourShader.setFloat("pointLights[3].constant", 1.0f);
		ourShader.setFloat("pointLights[3].linear", 0.09f);
		ourShader.setFloat("pointLights[3].quadratic", 0.032f);
			

		ourShader.setVec3("spotLight.position", camera.Position);
		ourShader.setVec3("spotLight.direction", camera.Front);
		ourShader.setVec3("spotLight.ambient", vec3(0.0f));
		ourShader.setVec3("spotLight.diffuse", vec3(1.0f));
		ourShader.setVec3("spotLight.specular", vec3(1.0f));
		ourShader.setFloat("spotLight.constant", 1.0f);
		ourShader.setFloat("spotLight.linear", 0.09f);
		ourShader.setFloat("spotLight.quadratic", 0.032f);
		ourShader.setFloat("spotLight.cutOff", cos(radians(12.5f)));
		ourShader.setFloat("spotLight.outerCutOff", cos(radians(15.0f)));


		ourShader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
		ourShader.setVec3("viewPos", camera.Position);

		ourShader.setVec3("material.ambient", vec3(1.0f, 0.5f, 0.31f));
		ourShader.setFloat("material.shininess", 32.0f);


		mat4 projection = mat4(1.0f);
		mat4 view = mat4(1.0f);
		mat4 model = glm::mat4(1.0f);

		projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		view = camera.GetViewMatrix();


		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("model", model);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		
		glBindVertexArray(VAO);

		for (unsigned int i = 0; i < cubeCount; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			ourShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		lightShader.use();
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);

		glBindVertexArray(lightVAO);

		for (unsigned int i = 0; i < 4; i++)
		{
			lightShader.setVec3("objectColor", pointLightColor[i]);
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = scale(model, vec3(.1f));
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		// check and call events, and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();


	}
	delete[] cubePositions;
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

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

}

float lastFrameTime = 0.0f;
void CalculateTime()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrameTime;
	lastFrameTime = currentFrame;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProccessMouseScroll(static_cast<float>(yoffset));
}



