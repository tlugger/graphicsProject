/*
 *  Set projection
 */
#include "CSCIx229.h"

void Project(int fpv,double fov,double asp,double dim)
{
        //  Tell OpenGL we want to manipulate the projection matrix
        glMatrixMode(GL_PROJECTION);
        //  Undo previous transformations
        glLoadIdentity();
        //  Perspective transformation
        if (fpv == 1)
                gluPerspective(fov,asp,.01,4*dim);
        else
                glOrtho(-asp*dim,asp*dim,-dim,+dim,-dim,+dim);
        //  Switch to manipulating the model matrix
        glMatrixMode(GL_MODELVIEW);
        //  Undo previous transformations
        glLoadIdentity();
}
