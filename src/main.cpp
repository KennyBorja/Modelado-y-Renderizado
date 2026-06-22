#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint texTerreno;
GLuint texParedes;
GLuint texTecho;
GLuint texTronco;
GLuint texHojas;

GLuint cargarTextura(const char* ruta) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(ruta, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 4)
            format = GL_RGBA;
        else if (nrChannels == 1)
            format = GL_RED;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    } else {
        stbi_image_free(data);
        return 0;
    }

    return textureID;
}

float camX = 0;
float camY = 0;
float camZ = 25;

bool leftMousePresionado = false;
bool rightMousePresionado = false;
double ultimoMouseX = 0.0;
double ultimoMouseY = 0.0;
float radio = 25.0f;
float anguloX = 0.0f;
float anguloY = 0.0f;

void saveScreenshot(GLFWwindow* window, const char* filename) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
    unsigned char* pixels = new unsigned char[3 * width * height];
    glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);
    
    FILE* f = fopen(filename, "wb");
    if (!f) {
        delete[] pixels;
        return;
    }
    
    unsigned char bmpHeader[54] = {
        'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0,
        40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };
    
    int size = 54 + 3 * width * height;
    bmpHeader[2] = (unsigned char)(size);
    bmpHeader[3] = (unsigned char)(size >> 8);
    bmpHeader[4] = (unsigned char)(size >> 16);
    bmpHeader[5] = (unsigned char)(size >> 24);
    
    bmpHeader[18] = (unsigned char)(width);
    bmpHeader[19] = (unsigned char)(width >> 8);
    bmpHeader[20] = (unsigned char)(width >> 16);
    bmpHeader[21] = (unsigned char)(width >> 24);
    
    bmpHeader[22] = (unsigned char)(height);
    bmpHeader[23] = (unsigned char)(height >> 8);
    bmpHeader[24] = (unsigned char)(height >> 16);
    bmpHeader[25] = (unsigned char)(height >> 24);
    
    fwrite(bmpHeader, 1, 54, f);
    fwrite(pixels, 1, 3 * width * height, f);
    fclose(f);
    
    delete[] pixels;
}

bool luzDireccional = true;

void configurarIluminacion() {
    if (luzDireccional) {
        GLfloat light_pos[] = { 5.0f, 5.0f, 5.0f, 0.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    } else {
        GLfloat light_pos[] = { 5.0f, 5.0f, 5.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    }
}

void configurarEscena() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
  GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
  GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
}

void configurarProyeccion(int width, int height) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (float)width / height, 1, 50);
}

void procesarInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camX -= 0.1f;

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camX += 0.1f;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camY += 0.1f;

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camY -= 0.1f;

  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    camZ -= 0.1f;

  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    camZ += 0.1f;

  static bool pPressedLastFrame = false;
  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
      if (!pPressedLastFrame) {
          saveScreenshot(window, "ejercicio4.bmp");
          pPressedLastFrame = true;
      }
  } else {
      pPressedLastFrame = false;
  }

  static bool lPressedLastFrame = false;
  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
      if (!lPressedLastFrame) {
          luzDireccional = !luzDireccional;
          lPressedLastFrame = true;
      }
  } else {
      lPressedLastFrame = false;
  }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            leftMousePresionado = true;
            glfwGetCursorPos(window, &ultimoMouseX, &ultimoMouseY);
        } else if (action == GLFW_RELEASE) {
            leftMousePresionado = false;
        }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rightMousePresionado = true;
            glfwGetCursorPos(window, &ultimoMouseX, &ultimoMouseY);
        } else if (action == GLFW_RELEASE) {
            rightMousePresionado = false;
        }
    }
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    double deltaX = xpos - ultimoMouseX;
    double deltaY = ypos - ultimoMouseY;

    if (leftMousePresionado) {
        anguloX += (float)deltaX * 0.01f;
        anguloY += (float)deltaY * 0.01f;

        ultimoMouseX = xpos;
        ultimoMouseY = ypos;
    } else if (rightMousePresionado) {
        float rightX = cos(anguloX);
        float rightZ = -sin(anguloX);
        
        float upX = -sin(anguloY) * sin(anguloX);
        float upY = cos(anguloY);
        float upZ = -sin(anguloY) * cos(anguloX);

        float sensibilidad = radio * 0.002f;

        camX -= rightX * deltaX * sensibilidad;
        camZ -= rightZ * deltaX * sensibilidad;
        
        camX += upX * deltaY * sensibilidad;
        camY += upY * deltaY * sensibilidad;
        camZ += upZ * deltaY * sensibilidad;

        ultimoMouseX = xpos;
        ultimoMouseY = ypos;
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    radio -= (float)yoffset;
    if (radio < 1.0f) radio = 1.0f;
}

void configurarCamara() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float cX = camX + radio * cos(anguloY) * sin(anguloX);
    float cY = camY + radio * sin(anguloY);
    float cZ = camZ + radio * cos(anguloY) * cos(anguloX);

    float upY = cos(anguloY) > 0 ? 1.0f : -1.0f;

    gluLookAt(
        cX, cY, cZ,
        camX, camY, camZ,
        0, upY, 0
    );
}

void dibujarTerreno() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texTerreno);

    GLfloat mat_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat mat_shininess = 10.0f;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-20.0f, 0.0f, -20.0f);
    glTexCoord2f(0.0f, 10.0f); glVertex3f(-20.0f, 0.0f,  20.0f);
    glTexCoord2f(10.0f, 10.0f); glVertex3f( 20.0f, 0.0f,  20.0f);
    glTexCoord2f(10.0f, 0.0f); glVertex3f( 20.0f, 0.0f, -20.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void dibujarCasa() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texParedes);

    GLfloat wall_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat wall_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat wall_specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat wall_shininess = 16.0f;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, wall_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, wall_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, wall_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, wall_shininess);
    
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.0f, 0.0f, 2.5f);
    glTexCoord2f(2.0f, 0.0f); glVertex3f( 2.0f, 0.0f, 2.5f);
    glTexCoord2f(2.0f, 1.5f); glVertex3f( 2.0f, 3.0f, 2.5f);
    glTexCoord2f(0.0f, 1.5f); glVertex3f(-2.0f, 3.0f, 2.5f);
    
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(-2.0f, 0.0f, -2.5f);
    glTexCoord2f(2.0f, 1.5f); glVertex3f(-2.0f, 3.0f, -2.5f);
    glTexCoord2f(0.0f, 1.5f); glVertex3f( 2.0f, 3.0f, -2.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 2.0f, 0.0f, -2.5f);
    
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.0f, 0.0f, -2.5f);
    glTexCoord2f(2.5f, 0.0f); glVertex3f(-2.0f, 0.0f,  2.5f);
    glTexCoord2f(2.5f, 1.5f); glVertex3f(-2.0f, 3.0f,  2.5f);
    glTexCoord2f(0.0f, 1.5f); glVertex3f(-2.0f, 3.0f, -2.5f);
    
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(2.5f, 0.0f); glVertex3f( 2.0f, 0.0f, -2.5f);
    glTexCoord2f(2.5f, 1.5f); glVertex3f( 2.0f, 3.0f, -2.5f);
    glTexCoord2f(0.0f, 1.5f); glVertex3f( 2.0f, 3.0f,  2.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 2.0f, 0.0f,  2.5f);
    glEnd();
    
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.0f, 3.0f, 2.5f);
    glTexCoord2f(2.0f, 0.0f); glVertex3f( 2.0f, 3.0f, 2.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.0f, 4.5f, 2.5f);
    
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(2.0f, 0.0f); glVertex3f( 2.0f, 3.0f, -2.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.0f, 3.0f, -2.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.0f, 4.5f, -2.5f);
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, texTecho);
    GLfloat roof_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat roof_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat roof_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat roof_shininess = 30.0f;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, roof_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, roof_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, roof_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, roof_shininess);
    
    glBegin(GL_QUADS);
    glNormal3f(-0.6f, 0.8f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.0f, 3.0f,  2.5f);
    glTexCoord2f(0.0f, 2.0f); glVertex3f( 0.0f, 4.5f,  2.5f);
    glTexCoord2f(2.0f, 2.0f); glVertex3f( 0.0f, 4.5f, -2.5f);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(-2.0f, 3.0f, -2.5f);
    
    glNormal3f(0.6f, 0.8f, 0.0f);
    glTexCoord2f(0.0f, 2.0f); glVertex3f( 0.0f, 4.5f,  2.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 2.0f, 3.0f,  2.5f);
    glTexCoord2f(2.0f, 0.0f); glVertex3f( 2.0f, 3.0f, -2.5f);
    glTexCoord2f(2.0f, 2.0f); glVertex3f( 0.0f, 4.5f, -2.5f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void dibujarArbol() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texTronco);

    GLfloat trunk_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat trunk_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat trunk_specular[] = { 0.01f, 0.01f, 0.01f, 1.0f };
    GLfloat trunk_shininess = 5.0f;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, trunk_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, trunk_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, trunk_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, trunk_shininess);

    GLUquadricObj *quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluQuadricTexture(quad, GL_TRUE);
    
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(quad, 0.4, 0.4, 3.0, 20, 20);
    glPopMatrix();
    
    gluDeleteQuadric(quad);
    
    glBindTexture(GL_TEXTURE_2D, texHojas);
    GLfloat leaves_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat leaves_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat leaves_specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat leaves_shininess = 10.0f;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, leaves_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, leaves_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, leaves_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, leaves_shininess);

    GLUquadricObj *sphereQuad = gluNewQuadric();
    gluQuadricNormals(sphereQuad, GLU_SMOOTH);
    gluQuadricTexture(sphereQuad, GL_TRUE);

    glPushMatrix();
    glTranslatef(0.0f, 3.0f, 0.0f);
    gluSphere(sphereQuad, 1.5, 30, 30);
    glPopMatrix();

    gluDeleteQuadric(sphereQuad);

    glDisable(GL_TEXTURE_2D);
}

void dibujarEscena_punto1() {
    dibujarTerreno();
    
    glPushMatrix();
    dibujarCasa();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-6.0f, 0.0f, -4.0f);
    dibujarArbol();
    glPopMatrix();
}

int main() {
  if (!glfwInit())
    return -1;

  int argc = 1;
  char *argv[1] = {(char *)"app"};
  glutInit(&argc, argv);

  GLFWwindow *window = glfwCreateWindow(1000, 1000, "Laboratorio 9 - Escena 3D", NULL, NULL);
  if (!window)
    return -1;

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    return -1;

  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetCursorPosCallback(window, cursorPositionCallback);
  glfwSetScrollCallback(window, scrollCallback);

  configurarEscena();
  configurarProyeccion(1000, 1000);

  texTerreno = cargarTextura("textures/grass.png");
  texParedes = cargarTextura("textures/brick.png");
  texTecho = cargarTextura("textures/tiles.png");
  texTronco = cargarTextura("textures/wood.png");
  texHojas = cargarTextura("textures/leaves.png");

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    procesarInput(window);
    configurarCamara();
    configurarIluminacion();

    dibujarEscena_punto1();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
