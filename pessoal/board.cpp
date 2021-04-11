#include "board.h"


Board::Board()
{
     model = new Model3DS("../3ds/board/board.3DS");
}

void Board::desenha()
{
    glPushMatrix();
        Objeto::desenha();

        //GUI::setColor(0.7,0.7,0.7);

        if (selecionado) {
            //glEnable(GL_CULL_FACE);
            GUI::setColor(0.1,0.1,0.1);
            //GUI::drawBox(-1,-1,-1, 1,1,1, true);

        }



        glScalef(1.1,1.1,1.2); //ajuste final da escala, podendo ser não-uniforme, independente para cada eixo
        glRotatef(-105,1,1,0); //alinhar o objeto 3ds com os eixos, deixando ele para cima de acordo com o eixo Y
        glTranslatef(2.9,1,0); //trazer objeto 3ds para origem

        glScalef(0.001,0.001,0.001); //apenas para conseguir enxergar o modelo 3ds
        model->draw(!selecionado); //se estiver selecionado, tem que desenhar o modelo 3ds
                                   //não colorido internamente para que a cor de destaque
                                   //da seleção tenha efeito
    glPopMatrix();
}

