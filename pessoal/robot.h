#ifndef ROBOT_H
#define ROBOT_H

#include "model3ds.h"

#include <pessoal/objeto.h>

class Robot : public Objeto
{
public:
    Model3DS* model;
public:
    Robot();
    void desenha();
};

#endif // Robot
