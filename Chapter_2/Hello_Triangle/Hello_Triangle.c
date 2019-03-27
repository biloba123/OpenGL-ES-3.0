// The MIT License (MIT)
//
// Copyright (c) 2013 Dan Ginsburg, Budirijanto Purnomo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
// Hello_Triangle.c
//
//    This is a simple example that draws a single triangle with
//    a minimal vertex/fragment shader.  The purpose of this
//    example is to demonstrate the basic concepts of
//    OpenGL ES 3.0 rendering.
#include "esUtil.h"

typedef struct
{
   // Handle to a program object
   GLuint programObject;

} UserData;

///
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader ( GLenum type, const char *shaderSrc )
{
    GLint shader;
    shader = glCreateShader(type);
    
    if (!shader) {
        return 0;
    }
    
    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);
    
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    
    if (!compiled) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        
        if (length > 1) {
            char log[length];
            glGetShaderInfoLog(shader, length, NULL, log);
            esLogMessage("Error compile shader:\n%s\n", log);
        }
        
        glDeleteShader(shader);
        
        return 0;
    }
    
    return shader;
}

///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
   char vShaderStr[] =
      "#version 300 es                          \n"
      "layout(location = 0) in vec4 a_position; \n"
      "layout(location = 1) in vec4 a_color;    \n"
      "out vec4 v_color;                        \n"
      "void main()                              \n"
      "{                                        \n"
      "   gl_Position = vec4(a_position.x * 0.5f, a_position.yzw);             \n"
      "   v_color = a_color;                    \n"
      "}                                        \n";

   char fShaderStr[] =
      "#version 300 es                              \n"
      "precision mediump float;                     \n"
      "in vec4 v_color;                             \n"
      "out vec4 o_fragColor;                        \n"
      "void main()                                  \n"
      "{                                            \n"
      "   o_fragColor = v_color;                    \n"
      "}                                            \n";

    GLuint programObject = glCreateProgram();
    
    if (!programObject) {
        return 0;
    }
    
    glAttachShader(programObject, LoadShader(GL_VERTEX_SHADER, vShaderStr));
    glAttachShader(programObject, LoadShader(GL_FRAGMENT_SHADER, fShaderStr));
    
    glLinkProgram(programObject);
    
    GLint linked;
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    
    if (!linked) {
        GLint length = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
        
        if (length > 1) {
            char log[length];
            glGetProgramInfoLog(programObject, length, NULL, log);
            esLogMessage("Error link program:\n%s\n", log);
        }
        
        glDeleteProgram(programObject);
        return 0;
    }
    
    {
//        GLint uniformNum, maxUniformLen;
//
//        glGetProgramiv(programObject, GL_ACTIVE_UNIFORMS, &uniformNum);
//        glGetProgramiv(programObject, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLen);
//
//        GLint size;
//        GLenum type;
//        char *name = malloc(sizeof(char) * maxUniformLen);
//        GLint location;
//
//        glGetActiveUniform(programObject, 0, maxUniformLen, NULL, &size, &type, name);
//        location = glGetUniformLocation(programObject, name);
//        free(name);
//
//        GLenum error;
//        error = glGetError();
//
//        if (type == GL_FLOAT) {
//            glUniform1f(location, 0.5);
//
//            error = glGetError();
//            if (error == GL_NO_ERROR) {
//                esLogMessage("");
//            }
//        }
    }
    
   // Store the program object
   userData->programObject = programObject;

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
   return TRUE;
}

///
// Draw a triangle using the shader pair created in Init()
//
void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
   GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
                            -0.5f, -0.5f, 0.0f,
                            0.5f, -0.5f, 0.0f
                         };

   // Set the viewport
   glViewport ( 0, 0, esContext->width, esContext->height );

   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );

   // Use the program object
   glUseProgram ( userData->programObject );

   // Load the vertex data
   glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
   glEnableVertexAttribArray ( 0 );
    
    glVertexAttrib4f(1, 0.0f, 1.0f, 0.0f, 1.0f);

   glDrawArrays ( GL_TRIANGLES, 0, 3 );
}

void Shutdown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   glDeleteProgram ( userData->programObject );
}

int esMain ( ESContext *esContext )
{
   esContext->userData = malloc ( sizeof ( UserData ) );

   esCreateWindow ( esContext, "Hello Triangle", 320, 240, ES_WINDOW_RGB );

   if ( !Init ( esContext ) )
   {
      return GL_FALSE;
   }

   esRegisterShutdownFunc ( esContext, Shutdown );
   esRegisterDrawFunc ( esContext, Draw );

   return GL_TRUE;
}
