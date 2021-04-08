#ifndef GUI_H
#define GUI_H

typedef void (* displayFunction)( void );
typedef void (* keyFunction)( unsigned char, int, int );

#include "extra.h"

class GUI {
    private:
        int id;
        const char *wTitle; //window title
        int wWidth, wHeight; //window dimensions
        displayFunction display;
        keyFunction key;

    public:
        GUI( int width, int height, displayFunction dFunction = glutGUI::defaultDisplay, keyFunction kFunction = glutGUI::defaultKey, const char *title = "GLUT" );
        void GLUTInit();
        void GLInit();
        ~GUI();

        void setTitle(const char *title);
        void setDimensions(int width, int height);
        void setDisplay( displayFunction dFunction );
        void setKey( keyFunction kFunction );

        static int processHits(GLint hits, GLuint buffer[]);
        static void pickingInit(GLint cursorX, GLint cursorY, int w, int h, GLuint* selectBuf, int BUFSIZE);
        static int pickingClosestName(GLuint* selectBuf, int BUFSIZE);
        //-------------------picking------------------

        //-------------------viewPorts------------------
        static void glScissoredViewport(int x, int y, int width, int height);
        //-------------------viewPorts------------------
        static void displayInit();
        static void mouseButtonInit(int button, int state, int x, int y);

        static void displayEnd();
        static void keyInit(unsigned char key, int x, int y);
        static void setLight(int id, float posx, float posy, float posz, bool onOffKeyDefault = false, bool attenuated = true, bool low = false, bool hidden = false, bool pontual = true, bool spot = false, bool onOffUserControl = true);
        static void setColor(float r, float g, float b, float a = 1.0, bool specular = false);
        static void drawSphere(float x, float y, float z, float radius);
        static void drawQuad(float width = 5.0, float height = 5.0, float discrWidth = 0.03, float discrHeight = 0.03, float texWidth = 5.0, float texHeight = 5.0, bool inverted = false);
        static void drawOriQuad(float angle = 0.0, float ex = 1.0, float ey = 0.0, float ez = 0.0, float width = 5.0, float height = 5.0, float discrWidth = 0.03, float discrHeight = 0.03, float texWidth = 5.0, float texHeight = 5.0, bool inverted = false);
        static void drawQuadBox(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax, float discrWidth = 0.03, float discrHeight = 0.03, bool inverted = false);
        static void drawBox(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax, bool inverted = false);
        static void drawFloor(float width = 5.0, float height = 5.0, float discrWidth = 0.03, float discrHeight = 0.03, float texWidth = 5.0, float texHeight = 5.0);
        static void drawOrigin(float tamanho = 0.1);
};

#endif
