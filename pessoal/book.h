#ifndef BOOK_H
#define BOOK_H

#include "model3ds.h"

#include <pessoal/objeto.h>

class Book : public Objeto
{
public:
    Model3DS* model;
public:
    Book();
    void desenha();
};

#endif
