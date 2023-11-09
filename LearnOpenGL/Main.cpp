#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "Shader.h"
#include "stb_image.h"
#include "cmath"
#include <iostream>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <mesh.h>

#include "camera.h"
#include "Model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void Render(GLFWwindow* window, Shader shader, Model model);
void CalculateTime();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
float DotProduct(vec3 vectorA, vec3 vectorB);
mat4 GetPerspectiveProjectionMatrix(float FOV, float aspectRatio, float near, float far);
void PrintMat4(mat4 matrix);
unsigned int loadCubemap(vector<string> faces);


using namespace glm;
using namespace std;



// setting
const unsigned int SCR_WIDTH = 1270;
const unsigned int SCR_HEIGHT = 720;

unsigned int shaderProgram;
unsigned int lightVAO;

Shader ourShader;
Model ourModel;
Shader lightShader;
Model ourLightModel;

Shader skyboxShader;

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

	//stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	ourShader.Init("vert_container.glsl", "frag_container_refraction.glsl");
	lightShader.Init("vert_light.glsl", "frag_light.glsl");
	std::cout << "load shader model" << std::endl;
	ourModel.Init("../LearnOpenGL/resourses/models/backpack/backpack.obj");
	ourLightModel.Init("../LearnOpenGL/resourses/models/suzanne/suzanne.fbx");
	skyboxShader.Init("vert_skybox.glsl", "frag_skybox.glsl");
	std::cout << "init model" << std::endl;
	Render(window, ourShader, ourModel);

	glfwTerminate();
	return 0;
}


void Render(GLFWwindow* window, Shader shader, Model model) {


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

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	vector<string>  faces
	{
		"right.jpg"
		, "left.jpg"
		, "top.jpg"
		, "bottom.jpg"
		, "front.jpg"
		, "back.jpg"
	};

	unsigned int cubemapTexture = loadCubemap(faces);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	ourShader.use();
	ourShader.setInt("skybox", 0);


	while (!glfwWindowShouldClose(window))
	{
		// received input
		CalculateTime();
		processInput(window);

		//rendering commands here
		glClearColor(.1, .1, .1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		shader.setVec3("dirLight.direction", vec3(-0.2f, -1.0f, -.3f));
		shader.setVec3("dirLight.ambient", vec3(.05f));
		shader.setVec3("dirLight.diffuse", vec3(.4f));
		shader.setVec3("dirLight.specular", vec3(.5f));

		// point light 1
		shader.setVec3("pointLights[0].position", pointLightPositions[0]);
		shader.setVec3("pointLights[0].ambient", pointLightColor[0]);
		shader.setVec3("pointLights[0].diffuse", vec3(1.0f, 0.0f, 0.0f));
		shader.setVec3("pointLights[0].specular", vec3(1.0f, 1.0f, 1.0f));
		shader.setFloat("pointLights[0].constant", 1.0f);
		shader.setFloat("pointLights[0].linear", 0.09f);
		shader.setFloat("pointLights[0].quadratic", 0.032f);
		// point light 2
		shader.setVec3("pointLights[1].position", pointLightPositions[1]);
		shader.setVec3("pointLights[1].ambient", pointLightColor[1]);
		shader.setVec3("pointLights[1].diffuse", vec3(0.0f, 0.0f, 1.0f));
		shader.setVec3("pointLights[1].specular", vec3(1.0f, 1.0f, 1.0f));
		shader.setFloat("pointLights[1].constant", 1.0f);
		shader.setFloat("pointLights[1].linear", 0.09f);
		shader.setFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		shader.setVec3("pointLights[2].position", pointLightPositions[2]);
		shader.setVec3("pointLights[2].ambient", pointLightColor[2]);
		shader.setVec3("pointLights[2].diffuse", vec3(0.0f, 1.0f, 0.0f));
		shader.setVec3("pointLights[2].specular", vec3(1.0f, 1.0f, 1.0f));
		shader.setFloat("pointLights[2].constant", 1.0f);
		shader.setFloat("pointLights[2].linear", 0.09f);
		shader.setFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		shader.setVec3("pointLights[3].position", pointLightPositions[3]);
		shader.setVec3("pointLights[3].ambient", pointLightColor[3]);
		shader.setVec3("pointLights[3].diffuse", vec3(1.0f));
		shader.setVec3("pointLights[3].specular", vec3(1.0f, 1.0f, 1.0f));
		shader.setFloat("pointLights[3].constant", 1.0f);
		shader.setFloat("pointLights[3].linear", 0.09f);
		shader.setFloat("pointLights[3].quadratic", 0.032f);


		shader.setVec3("spotLight.position", camera.Position);
		shader.setVec3("spotLight.direction", camera.Front);
		shader.setVec3("spotLight.ambient", vec3(0.0f));
		shader.setVec3("spotLight.diffuse", vec3(1.0f));
		shader.setVec3("spotLight.specular", vec3(1.0f));
		shader.setFloat("spotLight.constant", 1.0f);
		shader.setFloat("spotLight.linear", 0.09f);
		shader.setFloat("spotLight.quadratic", 0.032f);
		shader.setFloat("spotLight.cutOff", cos(radians(12.5f)));
		shader.setFloat("spotLight.outerCutOff", cos(radians(15.0f)));


		shader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
		shader.setVec3("viewPos", camera.Position);

		shader.setVec3("material.ambient", vec3(1.0f, 0.5f, 0.31f));
		shader.setFloat("material.shininess", 32.0f);


		mat4 projection = mat4(1.0f);
		mat4 view = mat4(1.0f);
		mat4 model = glm::mat4(1.0f);

		/*projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		PrintMat4(projection);*/
		projection = GetPerspectiveProjectionMatrix(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		//PrintMat4(projection);
		view = camera.GetViewMatrix();


		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setMat4("model", model);


		ourModel.Draw(shader);

		lightShader.use();
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);

		glBindVertexArray(lightVAO);

		for (unsigned int i = 0; i < 4; i++)
		{
			float dotProd = DotProduct(pointLightPositions[i] - camera.Position, camera.Front);
			lightShader.setVec3("objectColor", pointLightColor[i] * dotProd);
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = scale(model, vec3(.3f));
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightShader.setMat4("model", model);
			ourLightModel.Draw(lightShader);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		//std::cout << "Dot: " << dotProd << std::endl;


	// check and call events, and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();


	}
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

float DotProduct(vec3 vectorA, vec3 vectorB)
{
	vec3 vecA = normalize(vectorA);
	vec3 vecB = normalize(vectorB);

	float returnVal = vecA.x * vecB.x + vecA.y * vecB.y + vecA.z * vecB.z;
	return returnVal;
}

mat4 GetPerspectiveProjectionMatrix(float FOV, float aspectRatio, float near, float far)
{
	mat4 projectionMatrix;
	float f = 1 / (tan(FOV / 2.0f));
	float zRange = far - near;
	float A = -(far + near) / zRange;
	float B = -(2 * far * near) / zRange;
	projectionMatrix[0][0] = f / aspectRatio;
	projectionMatrix[1][1] = f;
	projectionMatrix[2][2] = A;
	projectionMatrix[2][3] = -1.0f;
	projectionMatrix[3][2] = B;
	projectionMatrix[3][3] = 0.0f;

	return projectionMatrix;
}

void PrintMat4(mat4 matrix)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::cout << " " << matrix[i][j];
		}
		cout << endl;
	}
}

unsigned int loadCubemap(vector<string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannel;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannel, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "cubemap Tex failed to load at path" << faces[i] << endl;
		}
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

