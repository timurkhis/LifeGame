//
//  Window.cpp
//  LifeGame
//
//  Created by Максим Бакиров on 15.12.16.
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
#include <cmath>
#include "Window.hpp"

const float DegToRad = M_PI / 180.0f;
const float RadToDef = 180.0f / M_PI;

Window::Window() :
    cellSizeRatio(0.05f),
    cellSize(0.0f),
    unitAngleStep(10.0f),
    unitRadiusRatio(0.3f) {}
Window::~Window() {}

Window &Window::Instance() {
    static Window window;
    return window;
}

void Window::MainLoop(int &argc, char **argv, const char *label, int width, int heigth) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(width, heigth);
    glutCreateWindow(label);
    glutReshapeFunc(Window::Reshape);
    glutDisplayFunc(Window::Display);
    glutIdleFunc(Window::IdleFunc);
    glutMouseFunc(Window::MouseFunc);
    glutKeyboardFunc(Window::KeyboardFunc);
    Instance().RecalculateSize();
    glutMainLoop();
}

void Window::Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    Instance().DrawGrid();
    Instance().DrawPoints();
    glutSwapBuffers();
}

void Window::Reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Instance().RecalculateSize();
}

void Window::IdleFunc() {
    for (const auto &handler : Instance().idleHandlers) {
        handler();
    }
}

void Window::MouseFunc(int button, int state, int x, int y) {
    for (const auto &handler : Instance().mouseHandlers) {
        handler(button, state, x, y);
    }
}

void Window::KeyboardFunc(unsigned char key, int x, int y) {
    for (const auto &handler : Instance().keyboardHandlers) {
        handler(key, x, y);
    }
}

void Window::DrawGrid() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    for (float x = cellSize; x < windowSize.x; x += cellSize) {
        glVertex2f(x, 0.f);
        glVertex2f(x, windowSize.y);
    }
    for (float y = cellSize; y < windowSize.y; y += cellSize) {
        glVertex2f(0.f, y);
        glVertex2f(windowSize.x, y);
    }
    glEnd();
}

void Window::DrawPoints() {
    for (const auto &point : units) {
        DrawPoint(point);
    }
}

void Window::DrawPoint(Vector pos) {
    const float radius = 0.5f * cellSize;
    const float unitRadius = unitRadiusRatio * cellSize;
    const float x = pos.x * cellSize + radius;
    const float y = pos.y * cellSize + radius;
    
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 1.0f, 0.0f);
    for (float i = 0.f; i < 360.f; i += unitAngleStep) {
        const float radians = i * DegToRad;
        const float pointX = x + cosf(radians) * unitRadius;
        const float pointY = y + sinf(radians) * unitRadius;
        glVertex2f(pointX, pointY);
    }
    glEnd();
}

void Window::RecalculateSize() {
    windowSize = Vector(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    const float sideRatio = windowSize.x > windowSize.y ? windowSize.y : windowSize.x;
    cellSize = sideRatio * cellSizeRatio;
}

void Window::AddIdleHandler(IdleHandler handler) {
    idleHandlers.push_back(handler);
}

void Window::AddMouseHandler(MouseHandler handler) {
    mouseHandlers.push_back(handler);
}

void Window::AddKeyboardHandler(KeyboardHandler handler) {
    keyboardHandlers.push_back(handler);
}
