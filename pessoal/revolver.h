#ifndef REVOLVER_H
#define REVOLVER_H

#include "model3ds.h"

#include <pessoal/objeto.h>

class Revolver : public Objeto
{
public:
    Model3DS* model;
public:
    Revolver();
    void desenha();
};

#endif // Revolver
