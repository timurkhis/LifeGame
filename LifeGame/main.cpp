//
//  main.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 10.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "Window.hpp"

int main(int argc, char **argv) {
    Window::Instance().MainLoop(argc, argv, "LifeGame", 800, 600);
    return 0;
}

