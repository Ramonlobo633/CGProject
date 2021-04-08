#ifndef CADEIRA_H
#define CADEIRA_H


#include <pessoal/objeto.h>
#include "model3ds.h"
class Cadeira : public Objeto
{
public:
    Model3DS* model;
public:
    Cadeira();
    void desenha();
};

#endif // CADEIRA_H
