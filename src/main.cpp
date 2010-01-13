/*
* Copyright (c) 2010 David Roberts <d@vidr.cc>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "level.h"

#include <cstdlib>
#include <ctime>
#include <cstdio>

#include <NEAT/neat.h>
#include <GL/glut.h>

#define DEBUG 1

static int mainWindow;
static Level *level;
static b2Vec2 viewCenter(0.0, 0.0);
static double viewZoom = 1.0;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    level->step();
    glutSwapBuffers();
}

void resize(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double ratio = (double) width / height;
    b2Vec2 extents(ratio * 75.0, 75.0);
    extents *= viewZoom;
    b2Vec2 lower = viewCenter - extents;
    b2Vec2 upper = viewCenter + extents;
    gluOrtho2D(lower.x, upper.x, lower.y, upper.y);
}

void timer(int) {
    glutSetWindow(mainWindow);
    glutPostRedisplay();
    glutTimerFunc(FRAME_PERIOD, timer, 0);
}

int main(int argc, char **argv) {
    if(argc <= 1) {
        printf("Must specify a level file to load, e.g.:\n");
        printf("\t%s data/peak.lvl\n", argv[0]);
        printf("\t%s data/climb.lvl\n", argv[0]);
        return 1;
    }
    
    srand(time(NULL));
    NEAT::load_neat_params("data/params.ne", DEBUG);
    level = new Level(argv[1]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(640, 480);
    mainWindow = glutCreateWindow("rtNEATbox");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutTimerFunc(FRAME_PERIOD, timer, 0);
    glutMainLoop();
    
    return 0;
}
