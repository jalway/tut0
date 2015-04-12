
#include <SFML\Window.hpp>
#include <SFML\System.hpp>
#include <GL\glew.h>
#include <iostream>

sf::Window window;

GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;


// gl_VertexID is a built-in variable, similar to gl_Position.
// There are a variety of these you can find in
// openGL docs; they're worth knowing since you 
// can use them for creative and highly efficient things.
// They give very valuable data without the associated cost
// of using buffers to send it in.
//
// Here we use them to create a set of triangle vertices using
// some simple modulus math.
// 0, 1, 0, 1, 0, 1 .... = i % 2
// 0, 0, 1, 1, 0, 0 .... = (i / 2) % 2
//	Which if used as x and y coordinates, results in the following 
//  triangle strip, given 4 vertices as input:
//	2===3
//	| \	|
//	|  \|
//	0===1

const char* vertexSource =
"#version 150\n"
"void main()"
"{"
"gl_Position = vec4("
"mod( gl_VertexID, 2 ),"
"mod( gl_VertexID / 2, 2 ),"
"0.0,"
"1.0 );"
"}";

// In the fragment shader, we will use a second built-in variable:
// gl_PrimitiveID. This is the index of the primitive being drawn. In the case of 2 triangles,
// this will be 0 and 1; which we now plug into the green color channel.

// Additionally, we will use a third built-in variable:
// gl_FragCoord. This gives x and y pixel coordinates, among other things.
// These are in pixel coordinates relative to the bottom left corner of the window.
// These range from 0 to max width and height (800, and 600 for this demo).
// We're going to do a modulus of 50 pixels on the pixel height value, then multiply by 0.02 to
// give us a 0 to 1 range we will create some blue stripes with.
const char* fragmentSource =
"#version 150\n"
"out vec4 outColor;"
"void main()"
"{"
"float verticalStripes = mod( gl_FragCoord.y, 50.0 ) * 0.02;"
"outColor = vec4( 0.0, gl_PrimitiveID, verticalStripes, 1.0 );"
"}";


void initShaders()
{
	// Compile the vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	// Compile the fragment shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	// Combine shaders into a program, bind, link and use
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glBindFragDataLocation(shaderProgram, 0, "outColor");

	glLinkProgram(shaderProgram);

	glUseProgram(shaderProgram);
}

void init()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 2;

	window.create(sf::VideoMode(800, 600), "Buffer_Free_Rendering", sf::Style::Close, settings);

	glewExperimental = true;
	glewInit();

	//Compile shaders
	initShaders();
}

void draw()
{
	//Clear to a neutral grey
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw two triangles. Triangle strip mode draws count - 2 triangles, sharing the last 2 vertices of a triangle with the next triangle.
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//Swap buffers
	window.display();
}

void pollEvents()
{
	sf::Event windowEvent;
	while (window.pollEvent(windowEvent))
	{
		switch (windowEvent.type)
		{
		case sf::Event::Closed:
			window.close();
			break;
		}
	}
}

void cleanUp()
{
	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

int main()
{
	init();

	while (window.isOpen())
	{
		draw();
		pollEvents();
	}

	cleanUp();

	return 0;
}
