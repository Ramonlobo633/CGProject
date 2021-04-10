#include <GL/glut.h>
#include <iostream>
#include <iomanip>

using namespace std;

#include "CameraDistante.h"
#include "Desenha.h"

#include "gui.h"

#include "stanfordbunny.h"
#include "model3ds.h"
#include <vector>

#include <pessoal/objeto.h>
#include <pessoal/personagem.h>
#include <pessoal/cadeira.h>
#include <pessoal/mesa.h>
#include <pessoal/notebook.h>


vector<Objeto*> objetos;
int posSelecionado = -1;


GLuint gBunnyWireList = NULL;
GLuint gBunnySolidList = NULL;




//variaveis globais
int width = 800;
int height = 600;

int slices = 16;
int stacks = 16;

float trans_obj = false;
float trans_luz = false;

float tx = 0.0;
float ty = 0.0;
float tz = 0.0;

float ax = 0.0;
float ay = 0.0;
float az = 0.0;

float delta = 5.0;

float sx = 1.0;
float sy = 1.0;
float sz = 1.0;

//ponto em coords locais, a ser calculado em coords de mundo
float pl[4] = { 0.0, 0.0, 0.0, 1.0 };
//ponto em coords globais, resultado da conversao a partir de pl
float pg[4] = { 0.0, 0.0, 0.0, 1.0 };

bool lbpressed = false;
bool mbpressed = false;
bool rbpressed = false;

bool lookat = false;

float last_x = 0.0;
float last_y = 0.0;

Camera* cam = new CameraDistante();
float savedCamera[9];

Camera* cam2 = new CameraDistante(-3,2,-5, 0,0,0, 0,1,0);
//Camera* cam2 = new CameraDistante(2,1,0, 2,1,-5, -1,0,0);
bool manual_cam = false;
bool change_manual_cam = false;

GLfloat light_position[] = { 1.5f, 1.5f, 1.5f, 1.0f };

//projecao
Vetor3D objProj;
bool drawGround = false;
bool drawGrid = true;
bool drawShadow = false;
GLfloat k = 0.0;
bool drawMesh = false;
bool normProj = false;
enum {PERSP = 0, ORTHO, OBLIQ};
int normProjType = PERSP;
bool drawCubo2x2x2 = true;
bool incluirObjeto = false;

//volume de visualizacao
float s = 0.5; //1.0;
float x = -s;
float X =  s;
float y = -s;
float Y =  s;
float near = 1.5;
float far = 2.5;
//matriz de cisalhamento (projecao obliqua)
    float alfaG = 75; //60; //45; //30 //90
    float phiG = 75; //-75; //-60; //-45; //-30 //-90
bool visVolEdit = false;

bool viewports = false;
bool scissored = false;
bool pickbyhit = false;



int pontoSelecionado = 0;




//-------------------sombra-------------------
//desenha todos os objetos que possuem sombra
void desenhaObjetosComSombra() {

        for (int i = 0; i < objetos.size(); ++i) {

                glPushMatrix();
                    glPushName(i + 1);
                        objetos[i]->desenha();
                    glPopName();
                glPopMatrix();

        }
      //  glPushMatrix();
      //      GUI::drawSphere(1.1,2.1,0.5,0.2);
      //  glPopMatrix();


    if (posSelecionado >= 0 and posSelecionado < objetos.size()) {

        objetos[posSelecionado]->t.x += glutGUI::dtx;
        objetos[posSelecionado]->t.y += glutGUI::dty;
        objetos[posSelecionado]->t.z += glutGUI::dtz;

        objetos[posSelecionado]->a.x += glutGUI::dax;
        objetos[posSelecionado]->a.y += glutGUI::day;
        objetos[posSelecionado]->a.z += glutGUI::daz;

        objetos[posSelecionado]->s.x += glutGUI::dsx;
        objetos[posSelecionado]->s.y += glutGUI::dsy;
        objetos[posSelecionado]->s.z += glutGUI::dsz;

      }

}

//picking

int picking( GLint cursorX, GLint cursorY, int w, int h ) {
    int BUFSIZE = 512;
    GLuint selectBuf[512];

    GUI::pickingInit(cursorX,cursorY,w,h,selectBuf,BUFSIZE);

    //de acordo com a implementacao original da funcao display
    //lembrar de nao inicializar a matriz de projecao, para nao ignorar a gluPickMatrix
    GUI::displayInit();

    //só precisa desenhar o que for selecionavel
    desenhaObjetosComSombra();
    //fim-de acordo com a implementacao original da funcao display
    //retornando o name do objeto (ponto de controle) mais proximo da camera (z minimo! *[matrizes de normalizacao da projecao])
    return GUI::pickingClosestName(selectBuf,BUFSIZE);
}
//-------------------picking------------------


//subrotinas extras
void mult_matriz_vetor(float res[4], float matriz[16], float entr[4]) {
    for (int i = 0; i < 4; i++) {
        res[i] = 0.0;
        for (int j = 0; j < 4; j++) {
            res[i] += matriz[4*j+i] * entr[j]; //matriz^T.entr
        }
    }
}

void mostra_matriz_transform(float matriz[16], bool transposta = true) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (!transposta) {
                cout << setiosflags (ios::fixed) << setprecision(2) << matriz[4*i+j] << "  ";
            } else {
                cout << setiosflags (ios::fixed) << setprecision(2) << matriz[4*j+i] << "  "; //matriz^T
            }
        }
        cout << "\n";
    }
    //cout << "\n";
}

void imprime_coords_locais_globais()
{
    //imprimindo coords locais e coords globais
      //locais
        cout << "Coords locais: " << pl[0] << ", " << pl[1] << ", " << pl[2] << "\n";
      //globais
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
              //composicao de transformacoes
              glTranslated(tx,ty,tz);
              glRotated(az,0,0,1);
              glRotated(ay,0,1,0);
              glRotated(ax,1,0,0);
            float mudanca_sist_coords[16];
            glGetFloatv(GL_MODELVIEW_MATRIX,mudanca_sist_coords);
            cout << "Matriz mudanca sist coords local=>global (T.Rz.Ry.Rx):\n";
            mostra_matriz_transform(mudanca_sist_coords);
            mult_matriz_vetor(pg,mudanca_sist_coords,pl);
            cout << "Coords globais: " << pg[0] << ", " << pg[1] << ", " << pg[2] << "\n\n";
        glPopMatrix();
}

void desenha_camera(float tam) {
    GLUquadricObj *quad = gluNewQuadric();

    glPushMatrix();
      Desenha::drawBox( -tam,-tam,0.0, tam,tam,2*tam );
      glTranslated(0,0,-tam);
      Desenha::gluClosedCylinder(quad, tam, tam/2, tam, slices,stacks);
    glPopMatrix();

    gluDeleteQuadric( quad );
}

void transformacao_camera_2_global(Vetor3D e, Vetor3D c, Vetor3D u, bool mostra_matriz = false)
{
    //z'
    Vetor3D z_ = e - c;
    z_.normaliza();
    //x'
    Vetor3D x_ = u ^ z_;
    x_.normaliza();
    //y'
    Vetor3D y_ = z_ ^ x_;
    //y_.normaliza();

    //matriz de transformacao
        float transform[16] = {
                                x_.x,   y_.x,   z_.x,   e.x,
                                x_.y,   y_.y,   z_.y,   e.y,
                                x_.z,   y_.z,   z_.z,   e.z,
                                0.0,    0.0,    0.0,    1.0
                             };
        glMultTransposeMatrixf( transform );

    if (mostra_matriz) {
        cout << "Matriz mudanca sist coords camera2=>global (R t = x' y' z' e):\n";
        mostra_matriz_transform(transform,false);
        cout << "\n";
    }
}

void transformacao_global_2_camera(Vetor3D e, Vetor3D c, Vetor3D u, bool mostra_matriz = false)
{
    //z'
    Vetor3D z_ = e - c;
    z_.normaliza();
    //x'
    Vetor3D x_ = u ^ z_;
    x_.normaliza();
    //y'
    Vetor3D y_ = z_ ^ x_;
    //y_.normaliza();

    //t'=-R^T.t=R^T.(-e)
    Vetor3D t_;
        t_.x = x_ * ( e * (-1) );
        t_.y = y_ * ( e * (-1) );
        t_.z = z_ * ( e * (-1) );

    //matriz de transformacao
        float transform[16] = {
                                x_.x,   x_.y,   x_.z,   t_.x,
                                y_.x,   y_.y,   y_.z,   t_.y,
                                z_.x,   z_.y,   z_.z,   t_.z,
                                0.0,    0.0,    0.0,    1.0
                             };
        glMultTransposeMatrixf( transform );

    if (mostra_matriz) {
        cout << "Matriz mudanca sist coords global=>camera2 (R^T -R^T.t):\n";
        mostra_matriz_transform(transform,false);
        cout << "\n";
    }
}


/* GLUT callback Handlers */

void resize(int w, int h)
{
    width = w;
    height = h;
}

void displayInit()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float ar = height>0 ? (float) width / (float) height : 1.0;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.,ar,0.1,1000.);


}

void displayEnd()
{
    glutSwapBuffers();
}



//Create a matrix that will project the desired shadow
//plano arbitrario
void shadowMatrix(GLfloat shadowMat[4][4], GLfloat groundplane[4], GLfloat lightpos[4])
{
    enum {X,Y,Z,W};
    GLfloat dot;

    /* Find dot product between light position vector and ground plane normal. */
    dot = groundplane[X] * lightpos[X] +
    groundplane[Y] * lightpos[Y] +
    groundplane[Z] * lightpos[Z] +
    groundplane[W] * lightpos[W];

    shadowMat[0][0] = dot - lightpos[X] * groundplane[X];
    shadowMat[0][1] = 0.f - lightpos[X] * groundplane[Y];
    shadowMat[0][2] = 0.f - lightpos[X] * groundplane[Z];
    shadowMat[0][3] = 0.f - lightpos[X] * groundplane[W];

    shadowMat[1][0] = 0.f - lightpos[Y] * groundplane[X];
    shadowMat[1][1] = dot - lightpos[Y] * groundplane[Y];
    shadowMat[1][2] = 0.f - lightpos[Y] * groundplane[Z];
    shadowMat[1][3] = 0.f - lightpos[Y] * groundplane[W];

    shadowMat[2][0] = 0.f - lightpos[Z] * groundplane[X];
    shadowMat[2][1] = 0.f - lightpos[Z] * groundplane[Y];
    shadowMat[2][2] = dot - lightpos[Z] * groundplane[Z];
    shadowMat[2][3] = 0.f - lightpos[Z] * groundplane[W];

    shadowMat[3][0] = 0.f - lightpos[W] * groundplane[X];
    shadowMat[3][1] = 0.f - lightpos[W] * groundplane[Y];
    shadowMat[3][2] = 0.f - lightpos[W] * groundplane[Z];
    shadowMat[3][3] = dot - lightpos[W] * groundplane[W];
}

//Create a matrix that will project the desired shadow
//plano alinhado aos eixos principais
void shadowMatrixYk(GLfloat shadowMat[4][4], GLfloat lightpos[4], GLfloat k)
{
    enum {X,Y,Z,W};

    shadowMat[0][0] =  k*lightpos[W] - lightpos[Y];
    shadowMat[0][1] =  lightpos[X];
    shadowMat[0][2] =  0.0;
    shadowMat[0][3] = -k*lightpos[X];

    shadowMat[1][0] =  0.0;
    shadowMat[1][1] =  k*lightpos[W];
    shadowMat[1][2] =  0.0;
    shadowMat[1][3] = -k*lightpos[Y];

    shadowMat[2][0] =  0.0;
    shadowMat[2][1] =  lightpos[Z];
    shadowMat[2][2] =  k*lightpos[W] - lightpos[Y];
    shadowMat[2][3] = -k*lightpos[Z];

    shadowMat[3][0] =  0.0;
    shadowMat[3][1] =  lightpos[W];
    shadowMat[3][2] =  0.0;
    shadowMat[3][3] = -lightpos[Y];

    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
            shadowMat[i][j] *= -1;
}
//-------------------sombra-------------------

void volVis(float alfa, float phi)
{
    glColor3d(1.0,0.5,0.0);
        float xfp = x*far/near;
        float Xfp = X*far/near;
        float yfp = y*far/near;
        float Yfp = Y*far/near;

    switch (normProjType) {
    case PERSP:
        glLineWidth(3.0);
        glBegin(GL_LINE_STRIP);
            glVertex3f(x,y,-near);
            glVertex3f(X,y,-near);
            glVertex3f(X,Y,-near);
            glVertex3f(x,Y,-near);
            glVertex3f(x,y,-near);
        glEnd();
        glBegin(GL_LINE_STRIP);
            glVertex3f(xfp,yfp,-far);
            glVertex3f(Xfp,yfp,-far);
            glVertex3f(Xfp,Yfp,-far);
            glVertex3f(xfp,Yfp,-far);
            glVertex3f(xfp,yfp,-far);
        glEnd();
        //
        glBegin(GL_LINES);
            glVertex3f(x ,y ,-near);
            glVertex3f(xfp,yfp,-far );
            glVertex3f(X ,y ,-near);
            glVertex3f(Xfp,yfp,-far );
            glVertex3f(X ,Y ,-near);
            glVertex3f(Xfp,Yfp,-far );
            glVertex3f(x ,Y ,-near);
            glVertex3f(xfp,Yfp,-far );
        glEnd();
        glLineWidth(1.0);
        glBegin(GL_LINES);
            glVertex3f(0  ,0  , 0   );
            glVertex3f(xfp,yfp,-far );
            glVertex3f(0  ,0  , 0   );
            glVertex3f(Xfp,yfp,-far );
            glVertex3f(0  ,0  , 0   );
            glVertex3f(Xfp,Yfp,-far );
            glVertex3f(0  ,0  , 0   );
            glVertex3f(xfp,Yfp,-far );
        glEnd();
        break;
    case ORTHO:
        glLineWidth(3.0);
        glBegin(GL_LINE_STRIP);
            glVertex3f(x,y,-near);
            glVertex3f(X,y,-near);
            glVertex3f(X,Y,-near);
            glVertex3f(x,Y,-near);
            glVertex3f(x,y,-near);
        glEnd();
        glBegin(GL_LINE_STRIP);
            glVertex3f(x,y,-far);
            glVertex3f(X,y,-far);
            glVertex3f(X,Y,-far);
            glVertex3f(x,Y,-far);
            glVertex3f(x,y,-far);
        glEnd();
        //
        glBegin(GL_LINES);
            glVertex3f(x ,y ,-near);
            glVertex3f(x ,y ,-far);
            glVertex3f(X ,y ,-near);
            glVertex3f(X ,y ,-far);
            glVertex3f(X ,Y ,-near);
            glVertex3f(X ,Y ,-far);
            glVertex3f(x ,Y ,-near);
            glVertex3f(x ,Y ,-far);
        glEnd();
        glLineWidth(1.0);
        glBegin(GL_LINES);
            glVertex3f(x ,y , 0);
            glVertex3f(x ,y ,-far);
            glVertex3f(X ,y , 0);
            glVertex3f(X ,y ,-far);
            glVertex3f(X ,Y , 0);
            glVertex3f(X ,Y ,-far);
            glVertex3f(x ,Y , 0);
            glVertex3f(x ,Y ,-far);
        glEnd();
        glBegin(GL_LINE_STRIP);
            glVertex3f(x,y,0);
            glVertex3f(X,y,0);
            glVertex3f(X,Y,0);
            glVertex3f(x,Y,0);
            glVertex3f(x,y,0);
        glEnd();
        break;

    default:
        break;
    }
}
Vetor3D transformedPoint(Vetor3D p, int a)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glLoadIdentity();

        glRotatef(objetos[a]->a.z,0,0,1);  // \ .
        glRotatef(objetos[a]->a.y,0,1,0);  //  | Rz.Ry.Rx . v
        glRotatef(objetos[a]->a.x,1,0,0);

        float matrix[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
    glPopMatrix();

    float pos[4] = {p.x,p.y,p.z, 1.0};
    float res[4];

    glutGUI::multGLMatrixByVector(res,matrix,pos);

    Vetor3D res3D = Vetor3D(res[0],res[1],res[2]);
    return res3D;
}
void displayInner(bool manual_cam)
{

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //transformacao de camera
    //definindo posicao e orientacao da camera
    if(!manual_cam){
      gluLookAt(cam->e.x,cam->e.y,cam->e.z, cam->c.x,cam->c.y,cam->c.z, cam->u.x,cam->u.y,cam->u.z);
    }else{
        Vetor3D eye = Vetor3D(1,2,2);
        Vetor3D up = Vetor3D(0,1,0);
        Vetor3D center = Vetor3D(0,-2,0);
        gluLookAt(eye.x,eye.y,eye.z, center.x,center.y,center.z, up.x, up.y, up.z);
        //caso existam objetos, defina o primeiro como olho juntamente com todas suas transformações
        if (!objetos.empty()) {

            int s = objetos.size();
            if(s == 1){
                //Vetor3D xlocalemCoordsGlobais = transformedPoint(Vetor3D(1,0,0));
                Vetor3D ylocalemCoordsGlobais = transformedPoint(Vetor3D(0,1,0), 0);
                Vetor3D zlocalemCoordsGlobais = transformedPoint(Vetor3D(0,0,1), 0);
                Vetor3D eye = objetos[0]->t;
                Vetor3D center = eye + Vetor3D(0,0,1);


                Vetor3D up = ylocalemCoordsGlobais;
                gluLookAt(eye.x,eye.y,eye.z, center.x,center.y,center.z, up.x, up.y, up.z);
            }else{
                //aponta a camera(que sera definida como o primeiro objeto) para o objeto selecionado
                if(lookat){  
                    if(posSelecionado == 0){
                        Vetor3D ylocalemCoordsGlobais = transformedPoint(Vetor3D(0,1,0), 0);
                        Vetor3D zlocalemCoordsGlobais = transformedPoint(Vetor3D(0,0,1), 0);
                        Vetor3D eye = objetos[0]->t;
                        Vetor3D center = eye + zlocalemCoordsGlobais;


                        Vetor3D up = ylocalemCoordsGlobais;
                        gluLookAt(eye.x,eye.y,eye.z, center.x,center.y,center.z, up.x, up.y, up.z);
                    }else{
                        Vetor3D ylocalemCoordsGlobais = transformedPoint(Vetor3D(0,1,0), 0);
                        Vetor3D eye = objetos[0]->t;
                        Vetor3D center = objetos[posSelecionado]->t;


                        Vetor3D up = ylocalemCoordsGlobais;
                        gluLookAt(eye.x,eye.y,eye.z, center.x,center.y,center.z, up.x, up.y, up.z);
                    }
                }
                Vetor3D ylocalemCoordsGlobais = transformedPoint(Vetor3D(0,1,0), 0);
                Vetor3D zlocalemCoordsGlobais = transformedPoint(Vetor3D(0,0,1), s-1);
                Vetor3D eye = objetos[0]->t;
                Vetor3D center = objetos[s-1]->t + zlocalemCoordsGlobais;


                Vetor3D up = ylocalemCoordsGlobais;
                gluLookAt(eye.x,eye.y,eye.z, center.x,center.y,center.z, up.x, up.y, up.z);
           }

        }



    }

    //sistema global
    glPushMatrix();
        //posicao da luz
            glutGUI::trans_luz = trans_luz;
            GUI::setLight(0,light_position[0],light_position[1],light_position[2],false,false);
            glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        //desenhando eixos do sistema de coordenadas global
            Desenha::drawEixos( 0.5 );
        //chao
            if (drawGround) {
                glPushMatrix();
                    glColor3d(0.6,0.4,0.0);
                    glTranslated(0.0,k-0.001,0.0);
                    GUI::drawFloor(15.0,15.0,0.5,0.5);
                glPopMatrix();
            }
            if (drawGrid) {
                glPushMatrix();
                    glColor3d(0.0,0.0,0.0);
                    Desenha::drawGrid( 10, k, 10, 1 );
                glPopMatrix();
            }
    glPopMatrix();


    //sistema local 1 com sombra
    if (!normProj) {
        glPushMatrix();
            //glColor3d(0.3,0.3,0.3);
            glColor3d(0.0,0.5,0.0);
            desenhaObjetosComSombra();
        glPopMatrix();
        glPushMatrix();

            //matriz de projecao para gerar sombra no plano y=k
                GLfloat sombra[4][4];
                shadowMatrixYk(sombra,light_position,k);

                glMultTransposeMatrixf( (GLfloat*)sombra );
            glDisable(GL_LIGHTING);
            glColor3d(0.0,0.0,0.0);
            if (drawShadow) desenhaObjetosComSombra();
            glEnable(GL_LIGHTING);
        glPopMatrix();
    }
//-------------------sombra-------------------

//----------normalizacao da projecao----------
    //sistema local 1 com distorção de projeção (normalização da projeção)
    if (normProj) {

                float alfa = alfaG*(PI/180); //grau2rad
                float phi = phiG*(PI/180); //grau2rad

        //marcacoes didaticas
            glPushMatrix();
                if (drawCubo2x2x2) {
                    //cubo 2x2x2
                    glColor3d(0.5,0.0,0.0);
                    glLineWidth(2.0);
                        //Desenha::drawSelectedBox(-1,1,-1,1,-1,1);
                        Desenha::drawWireBox(-1,1,-1,1,-1,1);
                    glLineWidth(1.0);
                    //imagem
                    glLineWidth(5.0);
                        glBegin(GL_LINE_STRIP);
                            glVertex3f(-1,-1,1);
                            glVertex3f( 1,-1,1);
                            glVertex3f( 1, 1,1);
                            glVertex3f(-1, 1,1);
                            glVertex3f(-1,-1,1);
                        glEnd();
                    glLineWidth(1.0);
                }
                //camera
                glColor3d(0.3,0.3,0.3);
                desenha_camera(0.1);
                //volume de visualizacao
                volVis(alfa, phi);
            glPopMatrix();
        //objeto original
            glPushMatrix();
                //glColor3d(0.3,0.3,0.3);
                glColor3d(0.0,0.5,0.0);
                glTranslatef(-0.5,-0.5,-0.5-0.5*(far+near));
                desenhaObjetosComSombra();
            glPopMatrix();
        //objeto distorcido pela normalizacao da projecao
            if (drawCubo2x2x2) {
                glPushMatrix();
                    //glColor4d(0.3,0.3,0.3,0.4); //transparente
                    glColor4d(0.0,0.5,0.0,0.4); //transparente
                    //glTranslatef(2.,0,0);
                    //matriz p multiplicar z por -1
                        float zNeg[16] = {
                                            1.0, 0.0, 0.0, 0.0,
                                            0.0, 1.0, 0.0, 0.0,
                                            0.0, 0.0,-1.0, 0.0,
                                            0.0, 0.0, 0.0, 1.0
                                         };
                        glMultTransposeMatrixf( zNeg );
                    //escolhe tipo da projecao
                    switch (normProjType) {
                    case PERSP:
                        //matriz de projecao perspectiva
                            glFrustum(x,X, y,Y, near,far);
                        break;
                    case ORTHO:
                        //matriz de projecao ortografica
                            glOrtho(x,X, y,Y, near,far);
                        break;

                    default:
                        break;
                    }
                    //desenha objeto
                    glTranslatef(-0.5,-0.5,-0.5-0.5*(far+near));
                    desenhaObjetosComSombra();
                glPopMatrix();
            }
    }

}

void display()
{
    displayInit();

        glViewport(0, 0, width, height);
        displayInner(false);
        if(viewports){
            glViewport(0, 3*height/4, width/4, height/4);
            displayInner(true);
        }
    displayEnd();
}

void mouseButton(int button, int state, int x, int y) {
    //capturando picking
    //GUI::mouseButtonInit(button,state,x,y);

    if(pickbyhit == true){
        // if the left button is pressed
        if (button == GLUT_LEFT_BUTTON) {
            // when the button is pressed
            if (state == GLUT_DOWN) {
                //picking
                int pick = picking( x, y, 5, 5 );
                cout << 'p' << endl;
                if (pick == 0) {
                    lbpressed = true;

                }else{
                    pontoSelecionado = pick - 1;
                    if(posSelecionado>=0){
                        objetos[posSelecionado]->selecionado = false;
                    }
                    //cout << pontoSelecionado << endl;
                    posSelecionado = pontoSelecionado;
                    objetos[posSelecionado]->selecionado = true;
                }
            }else{
                lbpressed = false;
            }
        }
        // if the middle button is pressed
        if (button == GLUT_MIDDLE_BUTTON) {
            // when the button is pressed
            if (state == GLUT_DOWN) {
                mbpressed = true;
            } else {// state = GLUT_UP
                mbpressed = false;

            }
        }
        // if the left button is pressed
        if (button == GLUT_RIGHT_BUTTON) {
            // when the button is pressed
            if (state == GLUT_DOWN) {
                rbpressed = true;


            } else {// state = GLUT_UP
                rbpressed = false;
            }
        }

        last_x = x;
        last_y = y;
    }else{
       //demais funções mouse
        // if the left button is pressed
        if (button == GLUT_LEFT_BUTTON) {
            // when the button is pressed
            if (state == GLUT_DOWN) {
                lbpressed = true;
            } else {// state = GLUT_UP
                lbpressed = false;
            }
        }

        // if the middle button is pressed
        if (button == GLUT_MIDDLE_BUTTON) {
            // when the button is pressed
            if (state == GLUT_DOWN) {
                mbpressed = true;
            } else {// state = GLUT_UP
                mbpressed = false;
            }
        }
        // if the left button is pressed
        if (button == GLUT_RIGHT_BUTTON) {
            // when the button is pressed
            if (state == GLUT_DOWN) {
                rbpressed = true;


            } else {// state = GLUT_UP
                rbpressed = false;
            }
        }

        last_x = x;
        last_y = y;
    }
}

void mouseMove(int x_, int y_) {
    float fator = 10.0;
    if (lbpressed && !rbpressed && !mbpressed) {
        if (visVolEdit) {
            x += (x_ - last_x)/100.0;
            y += -(y_ - last_y)/100.0;
        } else
        if (!trans_obj) {
            if (!manual_cam) {
                if (!change_manual_cam) {
                    cam->rotatex(y_,last_y);
                    cam->rotatey(x_,last_x);
                } else {
                    cam2->rotatex(last_y,y_);
                    cam2->rotatey(last_x,x_);
                }
            } else {
                if (!change_manual_cam) {
                    cam->rotatex(last_y,y_);
                    cam->rotatey(last_x,x_);
                } else {
                    cam2->rotatex(y_,last_y);
                    cam2->rotatey(x_,last_x);
                }
            }
        } else {
            objetos[posSelecionado]->a.x += (y_ - last_y)/fator;
            objetos[posSelecionado]->a.y += (x_ - last_x)/fator;
        }
    }
    fator = 100.0;
    if (!lbpressed && rbpressed && !mbpressed) {
        if (visVolEdit) {
            X += (x_ - last_x)/100.0;
            Y += -(y_ - last_y)/100.0;
        } else
        if (!trans_obj && !trans_luz) {
            if (!manual_cam) {
                if (!change_manual_cam) {
                    cam->translatex(x_,last_x);
                    cam->translatey(y_,last_y);
                } else {
                    cam2->translatex(last_x,x_);
                    cam2->translatey(last_y,y_);
                }
            } else {
                if (!change_manual_cam) {
                    cam->translatex(last_x,x_);
                    cam->translatey(last_y,y_);
                } else {
                    cam2->translatex(x_,last_x);
                    cam2->translatey(y_,last_y);
                }
            }
        } else {
            if (trans_obj) {
                objetos[posSelecionado]->t.x += (x_ - last_x)/fator;
                objetos[posSelecionado]->t.y += -(y_ - last_y)/fator;
            }
            if (trans_luz) {
                light_position[0] += (x_ - last_x)/fator;
                light_position[1] += -(y_ - last_y)/fator;
            }
        }
    }
    if (lbpressed && rbpressed && !mbpressed) {
        if (visVolEdit) {
            near += (x_ - last_x)/100.0;
            far += -(y_ - last_y)/100.0;
        } else
        if (!trans_obj && !trans_luz) {
            if (!manual_cam) {
                if (!change_manual_cam) {
                    cam->zoom(y_,last_y);
                } else {
                    cam2->zoom(last_y,y_);
                }
            } else {
                if (!change_manual_cam) {
                    cam->zoom(last_y,y_);
                } else {
                    cam2->zoom(y_,last_y);
                }
            }
        } else {
            if (trans_obj) {

                objetos[posSelecionado]->t.z += (y_ - last_y)/fator;
                fator = 10.0;
                objetos[posSelecionado]->a.z += -(x_ - last_x)/fator;
            }
            if (trans_luz) {
                light_position[2] += (y_ - last_y)/fator;
            }
        }
    }
    fator = 100.0;
    if (!lbpressed && !rbpressed && mbpressed) {
        if (visVolEdit) {
            alfaG += (x_ - last_x)/100.0;
            phiG += -(y_ - last_y)/100.0;
        } else
        if (!trans_obj) {
        } else {
            //sx *= 1+(x_ - last_x)/fator;
            objetos[posSelecionado]->s.x += (x_ - last_x)/fator;
            objetos[posSelecionado]->s.y += -(y_ - last_y)/fator;
        }
    }
    if (lbpressed && !rbpressed && mbpressed) {
        if (!trans_obj) {
        } else {
            objetos[posSelecionado]->s.z += (y_ - last_y)/fator;
        }
    }
    if (!lbpressed && rbpressed && mbpressed) {
        if (!trans_obj) {
        } else {
            pl[0] += (x_ - last_x)/fator;
            pl[1] += -(y_ - last_y)/fator;
            imprime_coords_locais_globais();
        }
    }

    last_x = x_;
    last_y = y_;
}

void key(unsigned char key, int x_, int y_)
{
    switch (key)
    {
        case 27 :
        case 13 :
        case 'q':
            exit(0);
            break;

        case 'F':
            glutFullScreen();
            break;
        case 'f':
            glutReshapeWindow(800,600);
            break;

        case '+':
            slices++;
            stacks++;
            break;

        case '-':
            if (slices>3 && stacks>3)
            {
                slices--;
                stacks--;
            }
            break;

        case 'X':
            //ax+=delta;
            if(trans_obj) ax+=delta;
            else { pl[0] += 0.01; imprime_coords_locais_globais(); };
            break;

        case 'Y':
            //ay+=delta;
            if(trans_obj) ay+=delta;
            else { pl[1] += 0.01; imprime_coords_locais_globais(); };
            break;

        case 'Z':
            //az+=delta;
            if(trans_obj) az+=delta;
            else { pl[2] += 0.01; imprime_coords_locais_globais(); };
            break;

        case 'x':
            //ax-=delta;
            if(trans_obj) ax-=delta;
            else { pl[0] -= 0.01; imprime_coords_locais_globais(); };
            break;

        case 'y':
            //ay-=delta;
            if(trans_obj) ay-=delta;
            else { pl[1] -= 0.01; imprime_coords_locais_globais(); };
            break;

        case 'z':
            //az-=delta;
            if(trans_obj) az-=delta;
            else { pl[2] -= 0.01; imprime_coords_locais_globais(); };
            break;

        case 'i':
            ax=ay=az=0.0;
            tx=ty=tz=0.0;
            sx=sy=sz=1.0;
            break;
        case 'I':
            x=y=-s;
            X=Y=s;
            near=1.5;
            far=2.5;
            alfaG=75;
            phiG=75;
            k=0.0;
            break;

        case 't':
            trans_obj = !trans_obj;
            break;

        case 'l':
            trans_luz = !trans_luz;
            break;
            //para ativar o olhar
       case 'L':
            lookat = !lookat;
        break;

        case 'p':
            light_position[3] = 1 - light_position[3];
            break;

        case 'G':
            drawGround = !drawGround;
            break;
        case 'g':
            drawGrid = !drawGrid;
            break;
        case 's':
            drawShadow = !drawShadow;
            break;
        case 'S':
             scissored = !scissored;
        break;
        case 'K':
            k += 0.1;
            break;
        case 'k':
            k -= 0.1;
            break;
        case 'n':
            normProj = !normProj;
            break;
        case 'N':
            normProjType = (normProjType+1)%3;
            break;
        case 'v':
            viewports = !viewports;
            manual_cam = !manual_cam;
            break;

        case 'c':
            static int posCam = 0;
            posCam++;
            if (cam->estilo == 1) {
                delete cam;
                if (posCam%5==0) cam = new CameraDistante(); //CameraDistante(0,1,5, 0,1,0, 0,1,0);
                if (posCam%5==1) cam = new CameraDistante(5,1,0, 0,1,0, 0,1,0);
                if (posCam%5==2) cam = new CameraDistante(0,1,-5, 0,1,0, 0,1,0);
                if (posCam%5==3) cam = new CameraDistante(-5,1,0, 0,1,0, 0,1,0);
                if (posCam%5==4) cam = new CameraDistante(savedCamera[0],savedCamera[1],savedCamera[2],savedCamera[3],savedCamera[4],savedCamera[5],savedCamera[6],savedCamera[7],savedCamera[8]);
            } else if (cam->estilo == 2) {
                delete cam;
                cam = new CameraDistante();
            }
            objProj = Vetor3D();
            break;

        case 'C':
            delete cam;
            cam = new CameraDistante(0,0,0, 0,0,-1, 0,1,0);
            objProj = Vetor3D(-0.5,-0.5,-2.5);
            break;

        case '0':
            pickbyhit = !pickbyhit;
            cout << pickbyhit << endl;
            break;

    case 8:
        if (posSelecionado >= 0 and posSelecionado < objetos.size()) {
            objetos.pop_back();
        }
        break;
    case 'r':
        if (posSelecionado >= 0 and posSelecionado < objetos.size()) {
            if(objetos[posSelecionado]->selecionado = true){
                objetos.erase(objetos.begin()+posSelecionado);

            }
        }
        break;

    case '.':
        if (posSelecionado >= 0 and posSelecionado < objetos.size()) {
            objetos[posSelecionado]->selecionado = false;
        }
        posSelecionado++;
        posSelecionado = posSelecionado%objetos.size();
        if (posSelecionado >= 0 and posSelecionado < objetos.size()) {
            objetos[posSelecionado]->selecionado = true;
        }
        break;
    case ',':
        if (posSelecionado >= 0 and posSelecionado < objetos.size()) {
            objetos[posSelecionado]->selecionado = false;
        }
        posSelecionado--;
        if (posSelecionado < 0) {
            posSelecionado = objetos.size()-1;
        }
        if (posSelecionado >= 0 and posSelecionado < objetos.size()) {
            objetos[posSelecionado]->selecionado = true;
        }
        break;


    case 'a':
        incluirObjeto = !incluirObjeto;
        break;
    case '3':
        if (incluirObjeto) {
            objetos.push_back( new Personagem() );
        }
        break;
    case '4':
        if (incluirObjeto) {
            objetos.push_back( new Cadeira() );
        }
        break;
    case '5':
        if (incluirObjeto) {
            objetos.push_back( new Mesa() );
        }
        break;
    case '6':
        if (incluirObjeto) {
            objetos.push_back( new Notebook() );
        }
        break;

    default:

        break;
    }

    glutPostRedisplay();
}

void idle(void)
{
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{

    //chamadas de inicializacao da GLUT
    glutGUI::width = width;
    glutGUI::height = height;
    glutGUI::cam = cam;

        glutInit(&argc, argv);
        glutInitWindowSize(width,height);
        glutInitWindowPosition(10,10);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

        glutCreateWindow("CG Project");

        glutReshapeFunc(resize);
        glutDisplayFunc(display);
        glutKeyboardFunc(key);
        glutIdleFunc(idle);

        glutMouseFunc(mouseButton);
        glutMotionFunc(mouseMove);

    //chamadas de inicializacao da OpenGL
        glClearColor(0.8,0.8,0.8,1.0);

        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_CULL_FACE);
        glEnable(GL_NORMALIZE); //mantem a qualidade da iluminacao mesmo quando glScalef eh usada

        glShadeModel(GL_SMOOTH);
        //glShadeModel(GL_FLAT);

        glEnable(GL_DEPTH_TEST);
        //glDepthFunc(GL_LESS);

        glEnable(GL_BLEND); //habilita a transparencia
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //definindo uma luz
            glEnable(GL_LIGHT0);

            const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
            const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
            const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            //const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

            const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
            const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
            const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
            const GLfloat high_shininess[] = { 100.0f };

            glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
            glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
            glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
            //glLightfv(GL_LIGHT0, GL_POSITION, light_position);

            glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
            glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

            //glFrontFace(GL_CCW);
            gBunnySolidList = GenStanfordBunnySolidList ();
            gBunnyWireList = GenStanfordBunnyWireList ();

    //iniciando o loop principal da glut
        glutMainLoop();


    return EXIT_SUCCESS;
}
