#ifndef PERSONAGEM_H
#define PERSONAGEM_H
#include "model3ds.h"

#include <pessoal/objeto.h>

class Personagem : public Objeto
{
public:
    Model3DS* model;
public:
    Personagem();
    void desenha();
};

#endif // PERSONAGEM_H
