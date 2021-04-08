#include "notebook.h"

Notebook::Notebook()
{
    model = new Model3DS("../3ds/notebook/notebook.3ds");
}

void Notebook::desenha()
{
    glPushMatrix();
        Objeto::desenha();

        GUI::setColor(0.5,0.5,0.5);

        if (selecionado) {
            //glEnable(GL_CULL_FACE);
             GUI::setColor(0.1,0.1,0.1);
            // GUI::drawBox(-1,-1,-1, 1,1,1, true);
        }



        glScalef(1,1,1.2); //ajuste final da escala, podendo ser não-uniforme, independente para cada eixo
        glRotatef(-90,1,0,0); //alinhar o objeto 3ds com os eixos, deixando ele para cima de acordo com o eixo Y
        glTranslatef(0,0,0); //trazer objeto 3ds para origem

        glScalef(0.001,0.001,0.001); //apenas para conseguir enxergar o modelo 3ds
        model->draw(!selecionado); //se estiver selecionado, tem que desenhar o modelo 3ds
                                   //não colorido internamente para que a cor de destaque
                                   //da seleção tenha efeito
    glPopMatrix();
}