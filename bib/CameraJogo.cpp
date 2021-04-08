#include "CameraJogo.h"
//---------------------------------------------------------------------------
CameraJogo::CameraJogo() : Camera()
{
  c.z=e.z-1;

  estilo = CAMJOGO;
}
//---------------------------------------------------------------------------
CameraJogo::CameraJogo(Vetor3D e, Vetor3D c, Vetor3D u) :
      Camera(e, c, u)
{
  estilo = CAMJOGO;
}
//---------------------------------------------------------------------------
CameraJogo::CameraJogo(GLfloat ex, GLfloat ey, GLfloat ez, GLfloat cx, GLfloat cy, GLfloat cz, GLfloat ux, GLfloat uy, GLfloat uz) :
      Camera(ex, ey, ez, cx, cy, cz, ux, uy, uz)
{
  estilo = CAMJOGO;
}
//---------------------------------------------------------------------------
void CameraJogo::zoom(GLfloat win_y, GLfloat last_y){
  //vetor do olho(eye) ao centro(center)
  Vetor3D Vec = c.subtracao(e);

  e = e.subtracao(Vec.multiplicacao( (win_y - last_y)/20.0 ));
  c = c.subtracao(Vec.multiplicacao( (win_y - last_y)/20.0 ));
}
//---------------------------------------------------------------------------
void CameraJogo::translatex(GLfloat win_x, GLfloat last_x){
  //vetor do olho(eye) ao centro(center)
  Vetor3D Vec = c.subtracao(e);
  //vetor no sentido positivo da direcao x
  Vetor3D Xpos = Vec.prodVetorial(u);

  e = e.subtracao( Xpos.multiplicacao( (last_x - win_x)/30.0 ) );
  c = c.subtracao( Xpos.multiplicacao( (last_x - win_x)/30.0 ) );
}
//---------------------------------------------------------------------------
void CameraJogo::translatey(GLfloat win_y, GLfloat last_y){
  e = e.soma( u.multiplicacao( (last_y - win_y)/30.0 ) );
  c = c.soma( u.multiplicacao( (last_y - win_y)/30.0 ) );
}
//---------------------------------------------------------------------------
void CameraJogo::rotatex(GLfloat win_y, GLfloat last_y){
  c = c.soma( u.multiplicacao( (last_y - win_y)/100.0 ) );

  //Vec normalizado
  Vetor3D N = c.subtracao(e);
  N.normaliza();
  c = e.soma(N);
    
  //vetor no sentido positivo da direcao x
  Vetor3D Xpos = N.prodVetorial(u);
  u = Xpos.prodVetorial(N);
  u.normaliza();
}
//---------------------------------------------------------------------------
void CameraJogo::rotatey(GLfloat win_x, GLfloat last_x){
  //vetor do olho(eye) ao centro(center)
  Vetor3D Vec = c.subtracao(e);
  //vetor no sentido positivo da direcao x
  Vetor3D Xpos = Vec.prodVetorial(u);

  c = c.subtracao( Xpos.multiplicacao( (last_x - win_x)/100.0 ) );

  //Vec normalizado
  Vetor3D N = c.subtracao(e);
  N.normaliza();
  c = e.soma(N);
}
//---------------------------------------------------------------------------
void CameraJogo::rotatez(GLfloat win_x, GLfloat last_x){
  //vetor do olho(eye) ao centro(center)
  Vetor3D Vec = c.subtracao(e);
  //vetor no sentido positivo da direcao x
  Vetor3D Xpos = Vec.prodVetorial(u);
  Xpos.normaliza();

  //modificando o vetor up
  u = u.subtracao( Xpos.multiplicacao( (last_x - win_x)/300.0 ) );
  u.normaliza();
}
//---------------------------------------------------------------------------
//passando o ponto local a camera (x,y,-1) para as coordenadas do mundo
Vetor3D CameraJogo::getPickedPoint(GLfloat x, GLfloat y){
  //calculando a base da camera
  //vetor do centro(center) ao olho(eye) - Zpos
  Vetor3D Vce = e.subtracao(c);
  Vce.normaliza();
  //vetor no sentido positivo da direcao x
  Vetor3D Xpos = u.prodVetorial(Vce);
  Xpos.normaliza();

  //mudanca da base da camera para a base do mundo (canonica)
  float dx = Xpos.x * x + u.x * y + Vce.x * -1;
  float dy = Xpos.y * x + u.y * y + Vce.y * -1;
  float dz = Xpos.z * x + u.z * y + Vce.z * -1;
  
  //translacao em relacao a posicao da camera
  dx += e.x;
  dy += e.y;
  dz += e.z;

  return Vetor3D(dx,dy,dz);
}
//---------------------------------------------------------------------------
