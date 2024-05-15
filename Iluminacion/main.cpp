#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.h"
#include "Cubo.h"
#include "Camera.h"
#include "Texture.h"

using namespace std;

const unsigned width = 800;
const unsigned height = 600;

unsigned int VBO, VAO, EBO;
unsigned int VBO2, VAO2, EBO2;
unsigned int VBO3, VAO3, EBO3;
unsigned int VAO_L;

unsigned int diffuseMap1, diffuseMap2, diffuseMap3, diffuseMap4;;
unsigned int specularMap1, specularMap2, specularMap3, specularMap4;

mat4 projection, view;

vec3 cameraPos = vec3(0.0f, 0.0f, 0.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastTime = 0.0f;

Camera camera(vec3(0.0f, 0.0f, 22.0f));
float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;
bool gravedadActive = true;

struct AABB {
	vec3 min;
	vec3 max;
};

void initGLFWVersion();
bool gladLoad();
void updateWindow(GLFWwindow* window, Shader ourShader, Shader ourLight, Texture ourTexture);

void framebuffer_size_callback(GLFWwindow* window, int w, int h);
void processInput(GLFWwindow* window, Shader ourShader);
void MovementInput(GLFWwindow* window, Shader ourShader);
void Mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void GeneracionBuffer(
	GLuint& VAO,
	GLuint& VBO,
	GLuint& EBO,
	GLuint& VAO2,
	GLuint& VBO2,
	GLuint& EBO2,
	GLuint& VAO3, 
	GLuint& VBO3, 
	GLuint& EBO3,
	const GLfloat* vertices,
	size_t verticesSize,
	const GLuint* indices,
	size_t indicesSize,
	GLuint& VAO_L,
	const GLfloat* vertices2,
	size_t vertices2Size,
	const GLuint* indices2,
	size_t indices2Size,
	const GLfloat* vertices3,
	size_t vertices3Size,
	const GLuint* indices3,
	size_t indices3Size);
void VertexAttribute(int layout, int data, int total, int start);
void DeleteVertexArrays(GLuint& VA);
void DeleteBuffer(GLuint& VBO, GLuint& EBO);
void TransformCubo(Shader ourShader);
void TransformCuboLight(Shader ourLight, vec3 lightPose);
void TransformCamera(Shader ourShader);
void CameraUniform(Shader shaderName);

void updatePhysics(float deltaTime);
bool AABBIntersect(AABB box1, AABB box2);
AABB GenerateBoindingBox(vec3 position, float w, float h, float d);
bool DetecCollision();
bool DetectTrigger();
bool DetecCollisionF();

int main()
{
	initGLFWVersion();

	//Creacion de ventana
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGl", NULL, NULL);
	if (window == NULL)
	{
		cout << "Falla al cargar ventana" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	if (!gladLoad())
	{
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	glfwSetScrollCallback(window, Scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	glEnable(GL_DEPTH_TEST);

	Shader ourShader("vertexShader.vs", "fragmentShader.fs");
	Shader ourLight("vertexLight.vl", "fragmentLight.fl");
	Texture ourTexture;

	GeneracionBuffer(VAO, VBO, EBO, VAO2, VBO2, EBO2, VAO3, VBO3, EBO3, vertices, sizeof(vertices), indices, sizeof(indices), VAO_L,vertices2, sizeof(vertices2),indices2, sizeof(indices2),vertices3,sizeof(vertices3),indices3, sizeof(indices3));

	diffuseMap1 = ourTexture.loadTextureID("Imagenes/w.png", 2);
	specularMap1 = ourTexture.loadTextureID("Imagenes/caja_especular2.png", 2);

	diffuseMap2 = ourTexture.loadTextureID("Imagenes/madera.jpg", 2);
	specularMap2 = ourTexture.loadTextureID("Imagenes/caja_especular2.png", 2);

	diffuseMap3 = ourTexture.loadTextureID("Imagenes/w6.png", 2);
	specularMap3 = ourTexture.loadTextureID("Imagenes/caja_especular2.png", 2);

	diffuseMap4 = ourTexture.loadTextureID("Imagenes/caja.png", 2);
	specularMap4 = ourTexture.loadTextureID("Imagenes/caja_especular2.png", 2);
	/*
	ourShader.use();
	ourShader.setInt("material.diffuse", 0);
	ourShader.setInt("material.specular", 1);
	*/
	updateWindow(window, ourShader, ourLight, ourTexture);

	DeleteVertexArrays(VAO);
	DeleteVertexArrays(VAO2);
	DeleteVertexArrays(VAO3);
	DeleteVertexArrays(VAO_L);
	DeleteBuffer(VBO, EBO);
	DeleteBuffer(VBO2, EBO2);
	DeleteBuffer(VBO3, EBO3);
	
	glfwTerminate();
	return 0;
}
void initGLFWVersion()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__
}
bool gladLoad()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Falla al cargar glad" << endl;
		return false;
	}
	return true;
}
void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}
void processInput(GLFWwindow* window, Shader ourShader)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	MovementInput(window,ourShader);
}

bool band=false;

void MovementInput(GLFWwindow* window, Shader ourShader)
{
	mat4 modelo = mat4(1.0f);
	vec3 oldPosition = posCube3[0];

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		posCube3[0].y = posCube3[0].y + 0.01;
		modelo = translate(modelo, posCube3[0]);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		posCube3[0].y = posCube3[0].y - 0.01;
		modelo = translate(modelo, posCube3[0]);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		posCube3[0].x = posCube3[0].x - 0.01;
		modelo = translate(modelo, posCube3[0]);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		posCube3[0].x = posCube3[0].x + 0.01;
		modelo = translate(modelo, posCube3[0]);
	}
	if (DetecCollision())
	{
		cuboVel.z = 0;
		posCube3[0].z = posCube2[0].z+1;
	}
	if (DetecCollisionF())
	{
		cuboVel.z = 0;
		posCube3[0] = oldPosition;
		if (posCube3[0].z < 0.1)
		{
			band = true;
		}
		if (band == false)
		{
			posCube3[0].z = posCube[0].z + 1.0;
		}
	}
	if (DetectTrigger())
	{
		band = false;
		posCube3[0] = vec3(0.0,-6.0,6.0);
	}
	ourShader.setMat4("model", modelo);
}

void Mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = xposIn;
	float ypos = yposIn;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	//camera.ProcessMouseMovement(xoffset, yoffset);
	camera.ProcessMouseMovement(0, 0);
}
void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
void updateWindow(GLFWwindow* window, Shader ourShader, Shader ourLight, Texture ourTexture)
{
	while (!glfwWindowShouldClose(window))
	{
		float currenTime = glfwGetTime();
		deltaTime = currenTime - lastTime;
		lastTime = currenTime;

		processInput(window, ourShader);

		if (gravedadActive)
		{
			updatePhysics(deltaTime);
		}

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/*
		ourTexture.ViewTextureMap(0, diffuseMap);
		ourTexture.ViewTextureMap(1, specularMap);
		*/
		ourShader.use();
		
		//ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		ourShader.setVec3("light.position", posCubeLight[0]);
		ourShader.setVec3("viewPos", camera.Position);
		/*
		ourShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
		ourShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		ourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		ourShader.setFloat("material.shininess", 32.0f);
		*/
		ourShader.setVec3("light.ambient", 0.9f, 0.9f, 0.9f);
		ourShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		ourShader.setVec3("light.specular", 0.3f, 0.3f, 0.3f);

		ourShader.setFloat("material.shininess", 64.0f);

		TransformCamera(ourShader);
		TransformCubo(ourShader);

		ourLight.use();
		CameraUniform(ourLight);
		TransformCuboLight(ourLight, posCubeLight[0]);
	
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
void GeneracionBuffer(
	GLuint& VAO,
	GLuint& VBO,
	GLuint& EBO,
	GLuint& VAO2,
	GLuint& VBO2,
	GLuint& EBO2,
	GLuint& VAO3, 
	GLuint& VBO3, 
	GLuint& EBO3,
	const GLfloat* vertices,
	size_t verticesSize,
	const GLuint* indices,
	size_t indicesSize,
	GLuint& VAO_L,
	const GLfloat* vertices2,
	size_t vertices2Size,
	const GLuint* indices2,
	size_t indices2Size,
	const GLfloat* vertices3,
	size_t vertices3Size,
	const GLuint* indices3,
	size_t indices3Size)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

	VertexAttribute(0, 3, 8, 0);
	VertexAttribute(1, 3, 8, 3);
	VertexAttribute(2, 2, 8, 6);
	//Objeto 2
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, vertices2Size, vertices2, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2Size, indices2, GL_STATIC_DRAW);

	VertexAttribute(0, 3, 8, 0);
	VertexAttribute(1, 3, 8, 3);
	VertexAttribute(2, 2, 8, 6);
	//Objeto 3
	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glGenBuffers(1, &EBO3);

	glBindVertexArray(VAO3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, vertices3Size, vertices3, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices3Size, indices3, GL_STATIC_DRAW);

	VertexAttribute(0, 3, 8, 0);
	VertexAttribute(1, 3, 8, 3);
	VertexAttribute(2, 2, 8, 6);

	//light
	glGenVertexArrays(1, &VAO_L);
	glBindVertexArray(VAO_L);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	VertexAttribute(0, 3, 6, 0);
	VertexAttribute(1, 3, 6, 3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void VertexAttribute(int layout, int data, int total, int start)
{
	glVertexAttribPointer(layout, data, GL_FLOAT, GL_FALSE, total * sizeof(float), (void*)(start * sizeof(float)));
	glEnableVertexAttribArray(layout);
}
void DeleteVertexArrays(GLuint& VA)
{
	glDeleteVertexArrays(1, &VA);
}
void DeleteBuffer(GLuint& VBO, GLuint& EBO)
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}
void TransformCubo(Shader ourShader)
{
	int tam = sizeof(posCube) / sizeof(posCube[0]);
	int tam2 = sizeof(posCube2) / sizeof(posCube2[0]);
	int tam3 = sizeof(posCube3) / sizeof(posCube3[0]);
	glBindVertexArray(VAO);
	for (int i = 0; i < tam; i++)
	{
		mat4 modelo = mat4(1.0f);
		modelo = translate(modelo, posCube[i]);
		//modelo = rotate(modelo, radians(-45.0f), vec3(0.3f, 0.7f, 0.0f));
		ourShader.setMat4("model", modelo);
		ourShader.setInt("material.diffuse", 0);
		ourShader.setInt("material.specular", 1);
		if (i == 123 || i == 124) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap3);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specularMap3);
		}
		else {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap1);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specularMap1);
		}
		//ourShader.setVec3("objectColor", 0.5f, 0.5f, 0.5f);

		/*ourShader.setVec3("material.ambient", 0.5f, 0.5f, 0.5f);
		ourShader.setVec3("material.diffuse", 0.3f, 0.3f, 0.31f);
		ourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		ourShader.setFloat("material.shininess", 32.0f);*/

		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(VAO2);
	
	for (int j = 0; j < tam2; j++)
	{
		mat4 modelo2 = mat4(1.0f);
		modelo2 = translate(modelo2, posCube2[j]);
		ourShader.setMat4("model", modelo2);

		ourShader.setInt("material.diffuse", 0);
		ourShader.setInt("material.specular", 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap2);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap2);

		//ourShader.setVec3("objectColor", 0.5f, 0.28f, 0.16f);

		/*ourShader.setVec3("material.ambient", 0.8f, 0.5f, 0.2f);
		ourShader.setVec3("material.diffuse", 0.6f, 0.3f, 0.1f);
		ourShader.setVec3("material.specular", 0.8f, 0.8f, 0.8f);
		ourShader.setFloat("material.shininess", 32.0f);*/

		glDrawElements(GL_TRIANGLES, sizeof(indices2) / sizeof(indices2[0]), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(VAO3);

	for (int j = 0; j < tam3; j++)
	{
		mat4 modelo3 = mat4(1.0f);
		modelo3 = translate(modelo3, posCube3[j]);
		ourShader.setMat4("model", modelo3);

		ourShader.setInt("material.diffuse", 0);
		ourShader.setInt("material.specular", 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap4);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap4);

		//ourShader.setVec3("objectColor", 0.5f, 0.28f, 0.16f);

		/*ourShader.setVec3("material.ambient", 0.8f, 0.5f, 0.2f);
		ourShader.setVec3("material.diffuse", 0.6f, 0.3f, 0.1f);
		ourShader.setVec3("material.specular", 0.8f, 0.8f, 0.8f);
		ourShader.setFloat("material.shininess", 32.0f);*/

		glDrawElements(GL_TRIANGLES, sizeof(indices3) / sizeof(indices3[0]), GL_UNSIGNED_INT, 0);
	}
}
void TransformCuboLight(Shader ourLight, vec3 lightPose)
{
	int tam = sizeof(posCubeLight) / sizeof(posCubeLight[0]);
	glBindVertexArray(VAO_L);
	for (int i = 0; i < tam; i++)
	{
		mat4 modelo = mat4(1.0f);
		modelo = translate(modelo, lightPose);
		modelo = scale(modelo, vec3(0.2f));
		ourLight.setMat4("model", modelo);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
		glDrawElements(GL_TRIANGLES, sizeof(indices2) / sizeof(indices2[0]), GL_UNSIGNED_INT, 0);
		glDrawElements(GL_TRIANGLES, sizeof(indices3) / sizeof(indices3[0]), GL_UNSIGNED_INT, 0);
	}
}
void TransformCamera(Shader ourShader)
{
	projection = perspective(radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
	view = camera.GetViewMatrix();

	CameraUniform(ourShader);
}
void CameraUniform(Shader shaderName)
{
	shaderName.setMat4("projection", projection);
	shaderName.setMat4("view", view);
}

void updatePhysics(float deltaTime)
{
	cuboVel.z += gravedad * deltaTime;
	posCube3[0].z += cuboVel.z * deltaTime;

	/*if (posCube[0].y < 0.0f)
	{
		posCube[0].y = 0.0f;
		cuboVel.y = 0;
	}*/
}

bool AABBIntersect(AABB box1, AABB box2)
{
	bool col;
	bool xOverlap = box1.min.x <= box2.max.x && box1.max.x >= box2.min.x;
	bool yOverlap = box1.min.y <= box2.max.y && box1.max.y >= box2.min.y;
	bool zOverlap = box1.min.z <= box2.max.z && box1.max.z >= box2.min.z;

	col = xOverlap && yOverlap && zOverlap;
	return col;
}

AABB GenerateBoindingBox(vec3 position, float w, float h, float d)
{
	AABB box;
	box.min = vec3((position.x - w) / 2.0f, (position.y - h) / 2.0f, position.z);
	box.max = vec3((position.x + w) / 2.0f, (position.y + h) / 2.0f, position.z + d);
	return box;
}

bool DetecCollision()
{
	bool col = false;
	AABB cubeBox2 = GenerateBoindingBox(posCube2[0], 8.0f, 8.0f, 1.0f);
	AABB cubeBox3 = GenerateBoindingBox(posCube3[0], 0.4f, 0.4f, 0.4f);
	AABB cubeBoxT = GenerateBoindingBox(posTriggerF[0], 1.0f, 1.0f, 1.0f);

	if (AABBIntersect(cubeBox3, cubeBox2)) {
		col = true;
	}
	
	return col;
}

bool DetecCollisionF() {
	bool col = false;
	AABB cubeBox3 = GenerateBoindingBox(posCube3[0], 0.4f, 0.4f, 0.4f);

	int tam = sizeof(posCube) / sizeof(posCube[0]);

	for (int i = 0; i < tam; ++i) {
		AABB cubeBox = GenerateBoindingBox(posCube[i], 0.5f, 0.5f, 0.5f);
		if (AABBIntersect(cubeBox3, cubeBox)) {
			col = true;
			break;
		}
	}

	return col;
}

bool DetectTrigger() {
	bool col = false;
	AABB cubeBox3 = GenerateBoindingBox(posCube3[0], 0.4f, 0.4f, 0.4f);
	AABB cubeBoxT = GenerateBoindingBox(posTriggerF[0], 0.5f, 0.55f, 0.55f);

	if (AABBIntersect(cubeBox3, cubeBoxT))
	{
		col = true;
	}
	return col;
}