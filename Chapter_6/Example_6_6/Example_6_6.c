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
// Example_6_6.c
//
//    This example demonstrates drawing a primitive with
//    a separate VBO per attribute
//
#include "esUtil.h"

typedef struct
{
   // Handle to a program object
   GLuint programObject;

   // VertexBufferObject Ids
   GLuint vboIds[3];

} UserData;


#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1


int Init ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
   const char vShaderStr[] =
      "#version 300 es                            \n"
      "layout(location = 0) in vec4 a_position;   \n"
      "layout(location = 1) in vec4 a_color;      \n"
      "out vec4 v_color;                          \n"
      "void main()                                \n"
      "{                                          \n"
      "    v_color = a_color;                     \n"
      "    gl_Position = a_position;              \n"
      "}";


   const char fShaderStr[] =
      "#version 300 es            \n"
      "precision mediump float;   \n"
      "in vec4 v_color;           \n"
      "out vec4 o_fragColor;      \n"
      "void main()                \n"
      "{                          \n"
      "    o_fragColor = v_color; \n"
      "}" ;

   GLuint programObject;

   // Create the program object
   programObject = esLoadProgram ( vShaderStr, fShaderStr );

   if ( programObject == 0 )
   {
      return GL_FALSE;
   }

   // Store the program object
   userData->programObject = programObject;
   userData->vboIds[0] = 0;
   userData->vboIds[1] = 0;
   userData->vboIds[2] = 0;


   glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
   return GL_TRUE;
}

//客户端定点数组
void DrawPrimitiveWithoutVBOs(GLfloat *vertices, GLint vtxStride, GLint numIndices, GLushort *indices) {
    //清除缓冲区对象绑定
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDX);
    
    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, vtxStride, vertices);
    glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, vtxStride, vertices + VERTEX_POS_SIZE);
    
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indices);
    
    glDisableVertexAttribArray(VERTEX_POS_INDX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDX);
}

//顶点缓冲区对象（结构数组）
void DrawPrimitiveWithVBOsOfAOS(ESContext *esContext, GLint numVertices, GLfloat *vtxBuf, GLint vtxStride, GLint numIndices, GLushort *indices) {
    UserData *userData = esContext->userData;
    
    if (userData->vboIds[0] == 0 && userData->vboIds[1] == 0) {
        glGenBuffers(2, userData->vboIds);
        glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
        glBufferData(GL_ARRAY_BUFFER, numVertices * vtxStride, vtxBuf, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLushort), indices, GL_STATIC_DRAW);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
    
    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDX);
    
    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, vtxStride, 0);
    glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, vtxStride, (const void *)(VERTEX_POS_SIZE * sizeof(float)));
    
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
    
    glBindBuffer ( GL_ARRAY_BUFFER, 0 );
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

//顶点缓冲区对象（数组结构）
void DrawPrimitiveWithVBOsOfSOA ( ESContext *esContext,
                             GLint numVertices, GLfloat **vtxBuf,
                             GLint *vtxStrides, GLint numIndices,
                             GLushort *indices )
{
   UserData *userData = esContext->userData;

   // vboIds[0] - used to store vertex position
   // vboIds[1] - used to store vertex color
   // vboIds[2] - used to store element indices
   if ( userData->vboIds[0] == 0 && userData->vboIds[1] == 0 &&
         userData->vboIds[2] == 0 )
   {
      // Only allocate on the first draw
      glGenBuffers ( 3, userData->vboIds );

      glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[0] );
      glBufferData ( GL_ARRAY_BUFFER, vtxStrides[0] * numVertices,
                     vtxBuf[0], GL_STATIC_DRAW );
      glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[1] );
      glBufferData ( GL_ARRAY_BUFFER, vtxStrides[1] * numVertices,
                     vtxBuf[1], GL_STATIC_DRAW );
      glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[2] );
      glBufferData ( GL_ELEMENT_ARRAY_BUFFER,
                     sizeof ( GLushort ) * numIndices,
                     indices, GL_STATIC_DRAW );
   }


   glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[0] );
   glEnableVertexAttribArray ( VERTEX_POS_INDX );
   glVertexAttribPointer ( VERTEX_POS_INDX, VERTEX_POS_SIZE,
                           GL_FLOAT, GL_FALSE, vtxStrides[0], 0 );

   glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[1] );
   glEnableVertexAttribArray ( VERTEX_COLOR_INDX );
   glVertexAttribPointer ( VERTEX_COLOR_INDX,
                           VERTEX_COLOR_SIZE,
                           GL_FLOAT, GL_FALSE, vtxStrides[1], 0 );

   glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[2] );

   glDrawElements ( GL_TRIANGLES, numIndices,
                    GL_UNSIGNED_SHORT, 0 );

   glDisableVertexAttribArray ( VERTEX_POS_INDX );
   glDisableVertexAttribArray ( VERTEX_COLOR_INDX );

   glBindBuffer ( GL_ARRAY_BUFFER, 0 );
   glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   // 3 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
   GLfloat vertexPos[3 * VERTEX_POS_SIZE] =
   {
      0.0f,  0.5f, 0.0f,        // v0
      -0.5f, -0.5f, 0.0f,        // v1
      0.5f, -0.5f, 0.0f         // v2
   };
   GLfloat color[3 * VERTEX_COLOR_SIZE] =
   {
      1.0f, 0.0f, 0.0f, 1.0f,   // c0
      0.0f, 1.0f, 0.0f, 1.0f,   // c1
      0.0f, 0.0f, 1.0f, 1.0f    // c2
   };
    
    GLfloat vertices[3 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] = {
        0.0f,  0.5f, 0.0f,        // v0
        1.0f, 0.0f, 0.0f, 1.0f,   // c0
        -0.5f, -0.5f, 0.0f,        // v1
        0.0f, 1.0f, 0.0f, 1.0f,   // c1
        0.5f, -0.5f, 0.0f,         // v2
        0.0f, 0.0f, 1.0f, 1.0f    // c2
    };
    
   GLint vtxStrides[2] =
   {
      VERTEX_POS_SIZE * sizeof ( GLfloat ),
      VERTEX_COLOR_SIZE * sizeof ( GLfloat )
   };

   // Index buffer data
   GLushort indices[3] = { 0, 1, 2 };
   GLfloat *vtxBuf[2] = { vertexPos, color };

   glViewport ( 0, 0, esContext->width, esContext->height );
   glClear ( GL_COLOR_BUFFER_BIT );
   glUseProgram ( userData->programObject );

//   DrawPrimitiveWithVBOsOfSOA ( esContext, 3, vtxBuf,
//                           vtxStrides, 3, indices );
    
//    DrawPrimitiveWithoutVBOs(vertices, vtxStrides[0] + vtxStrides[1], 3, indices);
    
    DrawPrimitiveWithVBOsOfAOS(esContext, 3, vertices, vtxStrides[0] + vtxStrides[1], 3, indices);
}

void Shutdown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   glDeleteProgram ( userData->programObject );
   glDeleteBuffers ( 3, userData->vboIds );
}

int esMain ( ESContext *esContext )
{
   esContext->userData = malloc ( sizeof ( UserData ) );

   esCreateWindow ( esContext, "Example 6-6", 320, 240, ES_WINDOW_RGB );

   if ( !Init ( esContext ) )
   {
      return GL_FALSE;
   }

   esRegisterShutdownFunc ( esContext, Shutdown );
   esRegisterDrawFunc ( esContext, Draw );

   return GL_TRUE;
}
