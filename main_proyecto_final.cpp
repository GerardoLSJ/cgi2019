/*---------------------------------------------------------*/
/* ----------------   Pr�ctica 11 --------------------------*/
/*-----------------    2019-2   ---------------------------*/
/*----------- Alumno: Karen Abril Robles Uribe -------------*/
//#define STB_IMAGE_IMPLEMENTATION
#include <glew.h>
#include <glfw3.h>
#include <stb_image.h>

#include "camera.h"
#include "Model.h"

// Other Libs
#include "SOIL2/SOIL2.h"

void resize(GLFWwindow* window, int width, int height);
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// settings
// Window size
int SCR_WIDTH = 3800;
int SCR_HEIGHT = 7600;

GLFWmonitor *monitors;
GLuint VBO, VAO, EBO;

//Camera
Camera camera(glm::vec3(0.0f, 0.0f, 13.0f));
double	lastX = 0.0f,
		lastY = 0.0f;
bool firstMouse = true;

//Timing
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 3.0f, 0.0f);
glm::vec3 lightDirection(0.0f, 0.0f, -3.0f);

void myData(void);
void display(Shader, Model, Model);
void getResolution(void);
void animate(void);
void LoadTextures(void);
unsigned int generateTextures(char*, bool);

//For Keyboard
float	movX = 0.0f,
		movY = 0.0f,
		movZ = -5.0f,
		rotX = 0.0f;

//Texture
unsigned int	t_smile,
				t_toalla,
				t_unam,
				t_white,
				t_panda,
				t_cubo,
				t_caja,
				t_caja_brillo,
				t_cara_brillo,
				t_central,
				t_roca,
				t_groot_brillo;

//Keyframes
float	posX = 0.0f,
		posY = 0.0f,
		posZ = 0.0f,
		rotRodIzq = 0.0f,
		giroMonito = 0.0f,
		movBrazoDer = 0.0f,
		movBrazoIzq = 0.0f;	// Variables para dibujar

#define MAX_FRAMES 9
int i_max_steps = 190;		// Cantidad de cuadros claves intermedios que est� generando
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float incX;		//Variable para IncrementoX
	float incY;		//Variable para IncrementoY
	float incZ;		//Variable para IncrementoZ
	float rotRodIzq;
	float rotInc;
	float giroMonito;
	float giroMonitoInc;
	float movBrazoDer;
	float movBrazoDerInc;
	float movBrazoIzq;
	float movBrazoIzqInc;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;
	KeyFrame[FrameIndex].movBrazoDer = movBrazoDer;
	KeyFrame[FrameIndex].movBrazoIzq = movBrazoIzq;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;

	movBrazoDer = KeyFrame[0].movBrazoDer;
	movBrazoIzq = KeyFrame[0].movBrazoIzq;	// Reemplazo por cuadro clave '0'
}

void interpolation(void)
{
	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	KeyFrame[playIndex].giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;
	KeyFrame[playIndex].movBrazoDerInc = (KeyFrame[playIndex + 1].movBrazoDer - KeyFrame[playIndex].movBrazoDer) / i_max_steps;
	KeyFrame[playIndex].movBrazoIzqInc = (KeyFrame[playIndex + 1].movBrazoIzq - KeyFrame[playIndex].movBrazoIzq) / i_max_steps;
}


unsigned int generateTextures(const char* filename, bool alfa)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}

void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;

	lastX = SCR_WIDTH / 2.0f;
	lastY = SCR_HEIGHT / 2.0f;

}

void LoadTextures()
{
	t_smile = generateTextures("Texturas/awesomeface.png", 1);
	t_toalla = generateTextures("Texturas/toalla.tga", 0);
	t_unam = generateTextures("Texturas/escudo_unam.jpg", 0);
	t_white = generateTextures("Texturas/white.jpg", 0);
	t_cubo = generateTextures("Texturas/Cube03.png", 1);
	t_caja = generateTextures("Texturas/caja.png", 1);
	t_caja_brillo = generateTextures("Texturas/caja_specular.png", 1);
	t_cara_brillo = generateTextures("Texturas/SpecularMap.png", 0);
	t_central = generateTextures("Texturas/Central.png", 1);
	t_roca = generateTextures("Texturas/caja.png", 1);

	//t_groot_brillo = generateTextures("Texturas/SpecularMap_groot_2.png", 1);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, t_smile);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, t_toalla);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, t_unam);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, t_white);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, t_cubo);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, t_caja);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, t_caja_brillo);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, t_cara_brillo);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, t_central);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, t_roca);
	//ActiveTexture(GL_TEXTURE10);
	//BindTexture(GL_TEXTURE_2D, t_groot_brillo);
	
}

void myData()
{	
	float vertices[] = {

		// Cubo con textura
		// positions			// normals				// texture coords
		 0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,		1.00f,  0.3333f,	// Tras
		-0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,		0.75f,  0.3333f,
		-0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,		0.75f,  0.6666f,
		-0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,		0.75f,  0.6666f,
		 0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,		1.00f,  0.6666f,
		 0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,		1.00f,  0.3333f,

		-0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,		0.25f,  0.3333f,	// Fron
		 0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,		0.50f,  0.3333f,
		 0.5f,  0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,		0.50f,  0.6666f,
		-0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,		0.25f,  0.3333f,
		-0.5f,  0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,		0.25f,  0.6666f,
		 0.5f,  0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,		0.50f,  0.6666f,

		-0.5f,  0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,		0.50f,  0.6666f,	// Izq
		-0.5f,  0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,		0.75f,  0.6666f,
		-0.5f, -0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,		0.75f,  0.3333f,
		-0.5f, -0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,		0.75f,  0.3333f,
		-0.5f, -0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,		0.50f,  0.3333f,
		-0.5f,  0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,		0.50f,  0.6666f,

		 0.5f,  0.5f,  0.5f,	 1.0f,  0.0f,  0.0f,		0.25f,  0.6666f,	// Der
		 0.5f, -0.5f,  0.5f,	 1.0f,  0.0f,  0.0f,		0.25f,  0.3333f,
		 0.5f, -0.5f, -0.5f,	 1.0f,  0.0f,  0.0f,		0.00f,  0.3333f,
		 0.5f, -0.5f, -0.5f,	 1.0f,  0.0f,  0.0f,		0.00f,  0.3333f,
		 0.5f,  0.5f, -0.5f,	 1.0f,  0.0f,  0.0f,		0.00f,  0.6666f,
		 0.5f,  0.5f,  0.5f,	 1.0f,  0.0f,  0.0f,		0.25f,  0.6666f,

		-0.5f, -0.5f,  0.5f,	 0.0f, -1.0f,  0.0f,		0.25f,  0.3333f,
		-0.5f, -0.5f, -0.5f,	 0.0f, -1.0f,  0.0f,		0.25f,  0.0000f,	// Inf
		 0.5f, -0.5f, -0.5f,	 0.0f, -1.0f,  0.0f,		0.50f,  0.0000f,
		 0.5f, -0.5f, -0.5f,	 0.0f, -1.0f,  0.0f,		0.50f,  0.0000f,
		 0.5f, -0.5f,  0.5f,	 0.0f, -1.0f,  0.0f,		0.50f,  0.3333f,
		-0.5f, -0.5f,  0.5f,	 0.0f, -1.0f,  0.0f,		0.25f,  0.3333f,

		-0.5f,  0.5f,  0.5f,	 0.0f,  1.0f,  0.0f,		0.25f,  0.6666f,	// Sup
		 0.5f,  0.5f,  0.5f,	 0.0f,  1.0f,  0.0f,		0.50f,  0.6666f,
		 0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,  0.0f,		0.50f,  1.0000f,
		-0.5f,  0.5f,  0.5f,	 0.0f,  1.0f,  0.0f,		0.50f,  0.6666f,
		-0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,  0.0f,		0.25f,  1.0000f,
		 0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,  0.0f,		0.50f,  1.0000f,

		/*
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	*/

	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

}

void animate(void)
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += KeyFrame[playIndex].incX;
			posY += KeyFrame[playIndex].incY;
			posZ += KeyFrame[playIndex].incZ;

			rotRodIzq += KeyFrame[playIndex].rotInc;
			giroMonito += KeyFrame[playIndex].giroMonitoInc;
			movBrazoDer += KeyFrame[playIndex].movBrazoDerInc;
			movBrazoIzq += KeyFrame[playIndex].movBrazoIzqInc;

			i_curr_steps++;
		}

	}
	
}

void display(Shader shader, Model botaDer, Model piernaDer, Model piernaIzq, Model torso,
	Model brazoDer, Model brazoIzq, Model cabeza, Model piso, Model pc)
{
	//Shader projectionShader("shaders/shader_light.vs", "shaders/shader_light.fs");
	//Shader projectionShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs");
	//Shader lightingShader("shaders/shader_texture_light_pos.vs", "shaders/shader_texture_light_pos.fs"); //Positional
	Shader lightingShader("shaders/shader_texture_light_dir.vs", "shaders/shader_texture_light_dir.fs"); //Directional
	//Shader lightingShader("shaders/shader_texture_light_spot.vs", "shaders/shader_texture_light_spot.fs"); //Spotlight
	Shader lampShader("shaders/shader_lamp.vs", "shaders/shader_lamp.fs");

	//To Use Lighting
	lightingShader.use();

	//If the light is Directional, we send the direction of the light
	//lightingShader.setVec3("light.direction", lightDirection);	

	//If the light is Positional, we send the position of the light
	//lightingShader.setVec3("light.position", lightPosition);
	//lightingShader.setVec3("light.position", camera.Position);

	//If the light is Spotlight, we put the light in the camera
	lightingShader.setVec3("light.position", camera.Position);
	lightingShader.setVec3("light.direction", camera.Front);
	lightingShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));

	lightingShader.setVec3("viewPos", camera.Position);

	// light properties
	lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
	lightingShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
	lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
	//For Positional and Spotlight
	lightingShader.setFloat("light.constant", 1.0f);
	lightingShader.setFloat("light.linear", 0.09f);
	lightingShader.setFloat("light.quadratic", 0.032f);

	// material properties
	lightingShader.setFloat("material_shininess", 32.0f);

	shader.use();

	// create transformations and Projection
	//glm::mat4 tmp = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 view = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projection = glm::mat4(1.0f);	//This matrix is for Projection

	glm::mat4 tmp = glm::mat4(1.0f);		// Matriz auxiliar para las escalas
	glm::mat4 tmp2 = glm::mat4(1.0f);		// Matriz auxiliar para las escaleras
	glm::mat4 floor = glm::mat4(1.0f);		// Matriz auxiliar para las escalas

	//Use "projection" to include Camera
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	view = camera.GetViewMatrix();

	// pass them to the shaders
	shader.setMat4("model", model);
	shader.setMat4("view", view);
	// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	shader.setMat4("projection", projection);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -1.0f));
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	shader.setMat4("model", model);
	piso.Draw(shader);

	glBindVertexArray(VAO);
	//Colocar c�digo aqu�
	lightingShader.setVec3("ambientColor", 0.0f, 0.0f, 0.0f);
	lightingShader.setVec3("diffuseColor", 1.0f, 1.0f, 1.0f);
	lightingShader.setVec3("specularColor", 1.0f, 0.0f, 0.0f);

	//lightingShader.setInt("material_specular", t_groot_brillo);		// Aqu� mandamos la textura de brillo (especular)

	//lightingShader.setInt("material_diffuse", t_unam);
	//glDrawArrays(GL_QUADS, 0, 24);

	/*---------------------------------------------------------*/
	/* ----------------   PB Edificio Q -----------------------*/
	/*---------------------------------------------------------*/

	// ---------- Construcci�n Piso ---------- //
	// Centro
	floor = tmp = model = glm::translate(glm::mat4(1.0f), glm::vec3(0.000f, 0.100f, 0.000f));
	model = glm::scale(model, glm::vec3(12.90f, 0.20f, 11.36f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Ala izquierda
	model = tmp;
	tmp = model = glm::translate(floor, glm::vec3(-12.150f, 0.000f, 6.890f));
	model = glm::scale(model, glm::vec3(11.40f, 0.20f, 35.16f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Ala derecha
	model = tmp;
	tmp = model = glm::translate(floor, glm::vec3(12.150f, 0.000f, -7.540f));
	model = glm::scale(model, glm::vec3(11.40f, 0.20f, 45.70f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	// ----- Construcci�n Muros Externos ----- //
	// Pared No. 1
	tmp = model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.550f, 1.700f, 14.985f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 18.99f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 2
	model = tmp;
	floor = tmp = model = glm::translate(model, glm::vec3(-5.600f, 0.000f, 9.395f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 3
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-5.600f, 0.000f, -17.480f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 35.16f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 4
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(5.600f, 0.000f, -17.480f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	// Pared No. 5
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(5.600f, 0.000f, 2.505f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 5.21f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 6
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(6.550f, 0.000f, 2.505f));
	model = glm::scale(model, glm::vec3(12.9f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 7
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(6.550f, 0.000f, -12.355f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 24.91f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 8
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(5.600f, 0.000f, -12.355f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 9
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(5.600f, 0.000f, 22.750f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 45.70f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 10
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-5.600f, 0.000f, 22.750f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 11
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-5.600f, 0.000f, -4.815f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 9.83f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 12
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-6.550f, 0.000f, -4.815f));
	model = glm::scale(model, glm::vec3(12.90f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	// ----- Construcci�n Muros Internos ----- //

	// *** Ala izquierda ***
	// Muro 1
	tmp = model = glm::translate(floor, glm::vec3(0.000f, 0.000f, -5.010f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 2
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -24.940f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 3
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-1.900f, 0.000f, 12.470f));
	model = glm::scale(model, glm::vec3(7.20f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 4
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(3.500f, 0.000f, 6.220f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 12.24f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 5
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -12.440f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 12.24f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// *** Centro ***
	// Muro 1
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(4.000f, 0.000f, -3.650f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 2
	model = tmp;
	floor = tmp = model = glm::translate(model, glm::vec3(3.550f, 0.000f, 0.000f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 3
	model = floor;
	tmp = model = glm::translate(model, glm::vec3(-1.775f, 0.000f, 2.400f));
	model = glm::scale(model, glm::vec3(3.35f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 4
	model = floor;
	floor = tmp = model = glm::translate(model, glm::vec3(6.000f, 0.000f, 0.000f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 5
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(3.550f, 0.000f, 0.000f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 6
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-1.775f, 0.000f, 2.400f));
	model = glm::scale(model, glm::vec3(3.35f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 7
	model = floor;
	tmp2 = tmp = model = glm::translate(model, glm::vec3(-2.800f, 0.000f, 0.000f));
	model = glm::translate(tmp2, glm::vec3(0.000f, 0.000f, 1.000f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 3.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// *** Ala derecha ***
	// Muro 1
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(11.950f, 0.000f, 11.490f));
	//tmp = model = glm::translate(model, glm::vec3(6.000f, 0.000f, 11.990f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 2
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -30.800f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 3
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(1.900f, 0.000f, 10.200f));
	model = glm::scale(model, glm::vec3(7.20f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 4
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, 10.200f));
	model = glm::scale(model, glm::vec3(7.20f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 5
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-3.500f, 0.000f, 5.200f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 10.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 6
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -10.400f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 10.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 7
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -10.200f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 10.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// ---------- Construcci�n Techo ---------- //
	// Centro
	floor = tmp = model = glm::translate(glm::mat4(1.0f), glm::vec3(0.000f, 3.300f, 0.000f));
	
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, 2.600f));
	model = glm::scale(model, glm::vec3(12.90f, 0.20f, 6.16f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -8.180f));
	model = glm::scale(model, glm::vec3(12.90f, 0.20f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-4.675f, 0.000f, 2.600f));
	model = glm::scale(model, glm::vec3(3.55f, 0.20f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(9.350f, 0.000f, 0.000f));
	model = glm::scale(model, glm::vec3(3.55f, 0.20f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Ala izquierda
	model = floor;
	tmp = model = glm::translate(floor, glm::vec3(-12.150f, 0.000f, 6.890f));
	model = glm::scale(model, glm::vec3(11.40f, 0.20f, 35.16f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Ala derecha
	model = tmp;
	tmp = model = glm::translate(floor, glm::vec3(12.150f, 0.000f, -7.540f));
	model = glm::scale(model, glm::vec3(11.40f, 0.20f, 45.70f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	/*---------------------------------------------------------*/
	/* ----------------   P1 Edificio Q -----------------------*/
	/*---------------------------------------------------------*/

	// ----- Construcci�n Muros Externos ----- //
	// Pared No. 1
	tmp = model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.550f, 1.700f, 14.985f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 18.99f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 2
	model = tmp;
	floor = tmp = model = glm::translate(model, glm::vec3(-5.600f, 0.000f, 9.395f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 3
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-5.600f, 0.000f, -17.480f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 35.16f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 4
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(5.600f, 0.000f, -17.480f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 5
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(5.600f, 0.000f, 2.505f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 5.21f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 6
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(6.550f, 0.000f, 2.505f));
	model = glm::scale(model, glm::vec3(12.9f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 7
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(6.550f, 0.000f, -12.355f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 24.91f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 8
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(5.600f, 0.000f, -12.355f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 9
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(5.600f, 0.000f, 22.750f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 45.70f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 10
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-5.600f, 0.000f, 22.750f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 11
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-5.600f, 0.000f, -4.815f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 9.83f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 12
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-6.550f, 0.000f, -4.815f));
	model = glm::scale(model, glm::vec3(12.90f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	// ----- Construcci�n Muros Internos ----- //

	// *** Ala izquierda ***
	// Muro 1
	tmp = model = glm::translate(floor, glm::vec3(0.000f, 0.000f, -5.010f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 2
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -24.940f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 3
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-1.900f, 0.000f, 12.470f));
	model = glm::scale(model, glm::vec3(7.20f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 4
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(3.500f, 0.000f, 6.220f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 12.24f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 5
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -12.440f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 12.24f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// *** Centro ***
	// Muro 1
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(4.000f, 0.000f, -3.650f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 2
	model = tmp;
	floor = tmp = model = glm::translate(model, glm::vec3(3.550f, 0.000f, 0.000f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 3
	model = floor;
	tmp = model = glm::translate(model, glm::vec3(-1.775f, 0.000f, 2.400f));
	model = glm::scale(model, glm::vec3(3.35f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 4
	model = floor;
	floor = tmp = model = glm::translate(model, glm::vec3(6.000f, 0.000f, 0.000f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 5
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(3.550f, 0.000f, 0.000f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 6
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-1.775f, 0.000f, 2.400f));
	model = glm::scale(model, glm::vec3(3.35f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 7
	model = floor;
	tmp2 = tmp = model = glm::translate(model, glm::vec3(-2.800f, 0.000f, 0.000f));
	model = glm::translate(tmp2, glm::vec3(0.000f, 0.000f, 1.000f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 3.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// *** Ala derecha ***
	// Muro 1
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(11.950f, 0.000f, 11.490f));
	//tmp = model = glm::translate(model, glm::vec3(6.000f, 0.000f, 11.990f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 2
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -30.800f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 3
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(1.900f, 0.000f, 10.200f));
	model = glm::scale(model, glm::vec3(7.20f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 4
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, 10.200f));
	model = glm::scale(model, glm::vec3(7.20f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 5
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-3.500f, 0.000f, 5.200f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 10.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 6
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -10.400f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 10.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 7
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -10.200f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 10.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// ---------- Construcci�n Techo ---------- //
	// Centro
	floor = tmp = model = glm::translate(glm::mat4(1.0f), glm::vec3(0.000f, 3.300f, 0.000f));

	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, 2.600f));
	model = glm::scale(model, glm::vec3(12.90f, 0.20f, 6.16f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -8.180f));
	model = glm::scale(model, glm::vec3(12.90f, 0.20f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-4.675f, 0.000f, 2.600f));
	model = glm::scale(model, glm::vec3(3.55f, 0.20f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(9.350f, 0.000f, 0.000f));
	model = glm::scale(model, glm::vec3(3.55f, 0.20f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Ala izquierda
	model = floor;
	tmp = model = glm::translate(floor, glm::vec3(-12.150f, 0.000f, 6.890f));
	model = glm::scale(model, glm::vec3(11.40f, 0.20f, 35.16f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Ala derecha
	model = tmp;
	tmp = model = glm::translate(floor, glm::vec3(12.150f, 0.000f, -7.540f));
	model = glm::scale(model, glm::vec3(11.40f, 0.20f, 45.70f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	/*---------------------------------------------------------*/
	/* ----------------   P1 Edificio Q -----------------------*/
	/*---------------------------------------------------------*/

	// ----- Construcci�n Muros Externos ----- //
	// Pared No. 1
	tmp = model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.550f, 1.700f, 14.985f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 18.99f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 2
	model = tmp;
	floor = tmp = model = glm::translate(model, glm::vec3(-5.600f, 0.000f, 9.395f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 3
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-5.600f, 0.000f, -17.480f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 35.16f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 4
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(5.600f, 0.000f, -17.480f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 5
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(5.600f, 0.000f, 2.505f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 5.21f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 6
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(6.550f, 0.000f, 2.505f));
	model = glm::scale(model, glm::vec3(12.9f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 7
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(6.550f, 0.000f, -12.355f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 24.91f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 8
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(5.600f, 0.000f, -12.355f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 9
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(5.600f, 0.000f, 22.750f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 45.70f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 10
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-5.600f, 0.000f, 22.750f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 11
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-5.600f, 0.000f, -4.815f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 9.83f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pared No. 12
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-6.550f, 0.000f, -4.815f));
	model = glm::scale(model, glm::vec3(12.90f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_roca);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	// ----- Construcci�n Muros Internos ----- //

	// *** Ala izquierda ***
	// Muro 1
	tmp = model = glm::translate(floor, glm::vec3(0.000f, 0.000f, -5.010f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 2
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -24.940f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 3
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-1.900f, 0.000f, 12.470f));
	model = glm::scale(model, glm::vec3(7.20f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 4
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(3.500f, 0.000f, 6.220f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 12.24f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 5
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -12.440f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 12.24f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// *** Centro ***
	// Muro 1
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(4.000f, 0.000f, -3.650f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 2
	model = tmp;
	floor = tmp = model = glm::translate(model, glm::vec3(3.550f, 0.000f, 0.000f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 3
	model = floor;
	tmp = model = glm::translate(model, glm::vec3(-1.775f, 0.000f, 2.400f));
	model = glm::scale(model, glm::vec3(3.35f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 4
	model = floor;
	floor = tmp = model = glm::translate(model, glm::vec3(6.000f, 0.000f, 0.000f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 5
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(3.550f, 0.000f, 0.000f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 5.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 6
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-1.775f, 0.000f, 2.400f));
	model = glm::scale(model, glm::vec3(3.35f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 7
	model = floor;
	tmp2 = tmp = model = glm::translate(model, glm::vec3(-2.800f, 0.000f, 0.000f));
	model = glm::translate(tmp2, glm::vec3(0.000f, 0.000f, 1.000f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 3.00f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// *** Ala derecha ***
	// Muro 1
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(11.950f, 0.000f, 11.490f));
	//tmp = model = glm::translate(model, glm::vec3(6.000f, 0.000f, 11.990f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 2
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -30.800f));
	model = glm::scale(model, glm::vec3(11.00f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 3
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(1.900f, 0.000f, 10.200f));
	model = glm::scale(model, glm::vec3(7.20f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 4
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, 10.200f));
	model = glm::scale(model, glm::vec3(7.20f, 3.00f, 0.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 5
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(-3.500f, 0.000f, 5.200f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 10.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 6
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -10.400f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 10.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Muro 7
	model = tmp;
	tmp = model = glm::translate(model, glm::vec3(0.000f, 0.000f, -10.200f));
	model = glm::scale(model, glm::vec3(0.20f, 3.00f, 10.20f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_toalla);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// ---------- Construcci�n Piso ---------- //
	// Centro
	floor = tmp = model = glm::translate(glm::mat4(1.0f), glm::vec3(0.000f, 0.100f, 0.000f));
	model = glm::scale(model, glm::vec3(12.90f, 0.20f, 11.36f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Ala izquierda
	model = tmp;
	tmp = model = glm::translate(floor, glm::vec3(-12.150f, 0.000f, 6.890f));
	model = glm::scale(model, glm::vec3(11.40f, 0.20f, 35.16f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Ala derecha
	model = tmp;
	tmp = model = glm::translate(floor, glm::vec3(12.150f, 0.000f, -7.540f));
	model = glm::scale(model, glm::vec3(11.40f, 0.20f, 45.70f));
	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_caja);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	
	model = glm::translate( glm::mat4(1.0f) , glm::vec3(0.0f,0.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	shader.setMat4("model", model);
	pc.Draw(shader);	// PC


	// ***********************************************************************************************

	//Light
	lampShader.use();
	lampShader.setMat4("projection", projection);
	lampShader.setMat4("view", view);
	model = glm::mat4(1.0f);
	model = glm::translate(model, lightPosition);
	model = glm::scale(model, glm::vec3(0.25f));
	lampShader.setMat4("model", model);

	//glBindVertexArray(lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);	//Light
	
	glBindVertexArray(0);
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Practica 10", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
	glfwSetWindowPos(window, 0, 30);
    glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, my_input);
    glfwSetFramebufferSizeCallback(window, resize);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//To Enable capture of our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glewInit();


	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	glEnable(GL_DEPTH_TEST);
	
	Shader modelShader("Shaders/modelLoading.vs", "Shaders/modelLoading.fs");
	// Load models
	Model botaDer = ((char *)"Models/Personaje/bota.obj");
	Model piernaDer = ((char *)"Models/Personaje/piernader.obj");
	Model piernaIzq = ((char *)"Models/Personaje/piernader.obj");
	Model torso = ((char *)"Models/Personaje/torso.obj");
	Model brazoDer = ((char *)"Models/Personaje/brazoder.obj");
	Model brazoIzq = ((char *)"Models/Personaje/brazoizq.obj");
	Model cabeza = ((char *)"Models/Personaje/cabeza.obj");
	Model pisoModel = ((char *)"Models/piso/piso.obj");
	Model pc = ((char *)"Models/g.obj");

	//Inicializaci�n de KeyFrames

	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incY = 0;
		KeyFrame[i].incZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].rotInc = 0;
	}

    
	glm::mat4 projection = glm::mat4(1.0f);	//This matrix is for Projection
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	// render loop
    // While the windows is not closed
    while (!glfwWindowShouldClose(window))
    {
		// per-frame time logic
		// --------------------
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
        // input
        // -----
        //my_input(window);
		animate();

        // render
        // Backgound color
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//display(modelShader, ourModel, llantasModel);
		display(modelShader, botaDer, piernaDer, 
				piernaIzq, torso, brazoDer, brazoIzq,
				cabeza, pisoModel, pc);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		posY--;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		posY++;
	// Cuerpo
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
		// Para mover brazo izquierdo
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		movBrazoIzq -=2.0f;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		movBrazoIzq +=2.0f;
		// Para mover brazo derecho
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		movBrazoDer -= 2.0f;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		movBrazoDer += 2.0f;

	
	
	//To play KeyFrame animation 
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}
	}

	//To Save a KeyFrame
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void resize(GLFWwindow* window, int width, int height)
{
    // Set the Viewport to the size of the created window
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}