#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<SOIL/SOIL.h>
void resizeCallback(GLFWwindow* window, int width, int height);
void createProgram();
void drawtriangle();
void init();
GLuint VBO[2];
GLuint VAO[2];
GLuint EBO[1];
GLuint programs[1];
int main(int argc, char** argv) {

	if (!glfwInit()) {
		std::cout << "Could not initialize glfw" << std::endl;
		exit(1);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(500, 500, "GLFW", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (!gladLoadGL()) {
		std::cout << "Could not initialize GLAD" << std::endl;
		exit(EXIT_FAILURE);
	}
	glfwSetFramebufferSizeCallback(window, resizeCallback);
	init();
	while (!glfwWindowShouldClose(window)) {

		glClearColor(0.5, 0.3, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		drawtriangle();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;

}

void resizeCallback(GLFWwindow* window, int width, int height)
{
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
}

void createProgram()
{
	const char* vertexSource = "#version 330 core\n"
		"in vec2 aPos;\n"
		"in vec2 atextCoord;\n"
		 "out vec2 textCoord;\n"
		"void main(){\n"
		"gl_Position=vec4(aPos,1.0,1.0);\n"
		"textCoord=atextCoord;\n"
		"\n}\0";
	const char* fragmentSource = "#version 330 core\n"
		"out vec4 fragColor;\n"
		"in vec2 textCoord;\n"
		"uniform sampler2D textKitten;\n"
		"uniform sampler2D textPuppy;\n"
		"void main()\n{"
		 "vec4 texture1=texture(textKitten,textCoord);\n"
		 "vec4 texture2=texture(textPuppy,textCoord);\n"
		  "fragColor=mix(texture1,texture2,0.3);"
		"\n}\0";

	//create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	//check for errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "Error compiling vertex shader : " << infoLog << std::endl;
	}
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	//check for errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cout << "Error compiling fragment shader: " << infoLog << std::endl;
	}

	//create program
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	programs[0] = program;

	//check for errors
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "Error linking program: " << infoLog << std::endl;
	}
}

void drawtriangle()
{
	glUseProgram(programs[0]);
	glBindVertexArray(VAO[0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
}

void init()
{
	float vertices[] = {
		-0.5,-0.5,0.0,1.0,//bottom left
		-0.5,0.5,0.0,0.0,//top left
		0.5,-0.5,1.0,1.0,//bottom right
		0.5,0.5,1.0,0.0//top right
	};
	
	unsigned int indices[]{
		0,1,2,
		1,2,3
	};

	glGenBuffers(2, VBO);

	//ebo
	 glGenBuffers(1, &EBO[0]);

	//vao
	glGenVertexArrays(2, VAO);
	glBindVertexArray(VAO[0]);

	//vbo
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);




	//ebo
	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	  //program
	  createProgram();

	  //tex
	  GLuint text[2];
	  glGenTextures(2, text);

	  int width, height, channels;
	  unsigned char* image;
	  glActiveTexture(GL_TEXTURE0);
	  glBindTexture(GL_TEXTURE_2D, text[0]);

	  //texture parameters
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	
	  
	  image = SOIL_load_image("sample.png", &width, &height, &channels, SOIL_LOAD_RGB);
	  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	  if (image) {
		  glGenerateMipmap(GL_TEXTURE_2D);
	  }
	  else {
		  std::cout << "Error loading file" << std::endl;
	  }
	  SOIL_free_image_data(image);
	  glUniform1i(glGetUniformLocation(programs[0], "textKitten"), 0);
	  
	  glActiveTexture(GL_TEXTURE1);
	  glBindTexture(GL_TEXTURE_2D, text[1]);

	  //texture parameters
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT),
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	  image = SOIL_load_image("sample2.png", &width, &height, &channels, SOIL_LOAD_RGB);
	  
	  if (image) {
		  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		  glGenerateMipmap(GL_TEXTURE_2D);
		 
	  }
	  else {
		  std::cout << "error loading image2" << std::endl;
	  }
	  SOIL_free_image_data(image);
	  

	//get location of attribute
	
	glUseProgram(programs[0]);
	glUniform1i(glGetUniformLocation(programs[0], "textPuppy"), 1);
	GLint aPos = glGetAttribLocation(programs[0], "aPos");
	GLuint aText = glGetAttribLocation(programs[0], "atextCoord");
	//configure vertex data
	glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, 4* sizeof(float), (void*)(0));
	glEnableVertexAttribArray(aPos);
	glVertexAttribPointer(aText, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(aText);

	

}
