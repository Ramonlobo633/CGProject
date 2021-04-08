#ifndef MESA_H
#define MESA_H

#include <pessoal/objeto.h>
#include "model3ds.h"
class Mesa : public Objeto
{
public:
    Model3DS* model;
public:
    Mesa();
    void desenha();
};

#endif // MESA_H
