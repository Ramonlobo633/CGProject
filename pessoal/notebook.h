#ifndef NOTEBOOK_H
#define NOTEBOOK_H

#include <pessoal/objeto.h>
#include "model3ds.h"
class Notebook : public Objeto
{
public:
    Model3DS* model;
public:
    Notebook();
    void desenha();
};

#endif // NOTEBOOK_H
