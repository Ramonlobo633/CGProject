#ifndef BOARD_H
#define BOARD_H
#include "model3ds.h"

#include <pessoal/objeto.h>

class Board : public Objeto
{
public:
    Model3DS* model;
public:
    Board();
    void desenha();
};

#endif // PERSONAGEM_H
