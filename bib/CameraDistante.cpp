#include "CameraDistante.h"
//---------------------------------------------------------------------------
CameraDistante::CameraDistante() : Camera()
{
  c.z=0;

  estilo = CAMDIST;
}
//---------------------------------------------------------------------------
CameraDistante::CameraDistante(Vetor3D e, Vetor3D c, Vetor3D u) :
      Camera(e, c, u)
{
  estilo = CAMDIST;
}
//---------------------------------------------------------------------------
CameraDistante::CameraDistante(GLfloat ex, GLfloat ey, GLfloat ez, GLfloat cx, GLfloat cy, GLfloat cz, GLfloat ux, GLfloat uy, GLfloat uz) :
      Camera(ex, ey, ez, cx, cy, cz, ux, uy, uz)
{
  estilo = CAMDIST;
}
//---------------------------------------------------------------------------
void CameraDistante::zoom(GLfloat win_y, GLfloat last_y){
  //vetor do olho(eye) ao centro(center)
  Vetor3D Vec = c.subtracao(e);
  //normalizando o Vec
  Vec.normaliza();

  //GLfloat moduloVec = Vec.modulo();
  //if ((moduloVec > 1) || (last_y > win_y)){
  //  e = e.soma(Vec.multiplicacao( (win_y - last_y)/2 ));
  //}
  //nova estrategia bem melhor
  Vetor3D eNovo = e.soma(Vec.multiplicacao( (win_y - last_y)/20.0 ));
  if ( c.subtracao(eNovo).prodEscalar(Vec) >= 0.0001 ){
    e = eNovo;
  }
}
//---------------------------------------------------------------------------
void CameraDistante::translatex(GLfloat win_x, GLfloat last_x){
  //vetor do olho(eye) ao centro(center)
  Vetor3D Vec = c.subtracao(e);
  //vetor no sentido positivo da direcao x
  Vetor3D Xpos = Vec.prodVetorial(u);
  Xpos.normaliza();

  e = e.soma( Xpos.multiplicacao( Vec.modulo()*(last_x - win_x)/300.0 ) );
  c = c.soma( Xpos.multiplicacao( Vec.modulo()*(last_x - win_x)/300.0 ) );
}
//---------------------------------------------------------------------------
void CameraDistante::translatey(GLfloat win_y, GLfloat last_y){
  //vetor do olho(eye) ao centro(center)
  Vetor3D Vec = c.subtracao(e);

  e = e.subtracao( u.multiplicacao( Vec.modulo()*(last_y - win_y)/300.0 ) );
  c = c.subtracao( u.multiplicacao( Vec.modulo()*(last_y - win_y)/300.0 ) );
}
//---------------------------------------------------------------------------
void CameraDistante::rotatex(GLfloat win_y, GLfloat last_y){
  //vetor auxiliar aux
  Vetor3D aux = c;//Vetor3D(c.x, c.y, c.z);
  c = c.multiplicacao(2.0).subtracao(e);
  e = aux;//.recebe(aux);

  //vetor do olho(eye) ao centro(center)
  Vetor3D Vec = c.subtracao(e);
  GLfloat moduloVecant = Vec.modulo();
  c = c.soma(u.multiplicacao( ((1.0/30.0)*moduloVecant)*(last_y - win_y)/5.0 ));
  Vec = c.subtracao(e);
  GLfloat moduloVec = Vec.modulo();
    Vec = Vec.multiplicacao( moduloVecant/moduloVec );
  c = e.soma(Vec);
  Vec = c.subtracao(e);

  //vetor no sentido positivo da direcao x
  Vetor3D Xpos = Vec.prodVetorial(u);
  u = Xpos.prodVetorial(Vec);
  u.normaliza();

  aux = e;//.recebe(e);
  e = e.multiplicacao(2.0).subtracao(c);
  c = aux;//.recebe(aux);
}
//---------------------------------------------------------------------------
void CameraDistante::rotatey(GLfloat win_x, GLfloat last_x){
  //vetor auxiliar aux
  Vetor3D aux = c;//Vetor3D(c.x, c.y, c.z);
  c = c.multiplicacao(2).subtracao(e);
  e = aux;//.recebe(aux);

  //vetor do olho(eye) ao centro(center)
  Vetor3D Vec = c.subtracao(e);
  GLfloat moduloVecant = Vec.modulo();
  //vetor no sentido positivo da direcao x
  Vetor3D Xpos = Vec.prodVetorial(u);
  Xpos.normaliza();

  c = c.subtracao(Xpos.multiplicacao( ((1.0/30.0)*moduloVecant)*(last_x - win_x)/5.0 ));

  Vec = c.subtracao(e);
  GLfloat moduloVec = Vec.modulo();
    Vec = Vec.multiplicacao( moduloVecant/moduloVec );
  c = e.soma(Vec);

  //novo-----------------------------------
  //atualizando u
  Vetor3D up;
  if (u.y>=0.0) {
    up = Vetor3D(0.0,1.0,0.0);
  } else {
    up = Vetor3D(0.0,-1.0,0.0);
  }
  Vetor3D XposUp = Vec.prodVetorial(up);
  u = XposUp.prodVetorial(Vec);
  u.normaliza();
  //fim_novo-------------------------------

  aux = e;//.recebe(e);
  e = e.multiplicacao(2.0).subtracao(c);
  c = aux;//.recebe(aux);
}
//---------------------------------------------------------------------------
void CameraDistante::rotatez(GLfloat win_x, GLfloat last_x){
  //vetor do olho(eye) ao centro(center)
  Vetor3D Vec = c.subtracao(e);
  //vetor no sentido positivo da direcao x
  Vetor3D Xpos = Vec.prodVetorial(u);
  Xpos.normaliza();

  //modificando o vetor up
  u = u.soma(Xpos.multiplicacao( (last_x - win_x)/300.0 ));
  u.normaliza();
}
//---------------------------------------------------------------------------
//passando o ponto local a camera (x,y,-1) para as coordenadas do mundo
Vetor3D CameraDistante::getPickedPoint(GLfloat x, GLfloat y){
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
