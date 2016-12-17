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

#include <cassert>
#include <iostream>
#include <string>
#include <cmath>
#include <stdlib.h>
#include "Window.hpp"

const float DegToRad = M_PI / 180.0f;
const float RadToDef = 180.0f / M_PI;

const int Window::KeyMinus = 45;
const int Window::KeyPlus = 61;
const int Window::KeyEscape = 27;

Window::Window() :
    cameraMoveSensititity(1.f),
    unitAngleStep(10.0f),
    unitRadiusRatio(0.3f),
    cellSizeRatioMin(0.02f),
    cellSizeRatioMax(0.1f),
    cellSizeRatioStep(0.01f),
    cellSizeRatio(0.05f),
    cellSize(0.0f),
    units(nullptr),
    leftButtonPressed(false),
    cameraScrolled(false) {}

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
    glutMouseFunc(Window::MouseFunc);
    glutKeyboardFunc(Window::KeyboardFunc);
    glutMotionFunc(Window::MotionFunc);
    Instance().RecalculateSize();
    glutMainLoop();
}

void Window::Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    Instance().DrawGrid();
    Instance().DrawPoints();
    Instance().DrawNumbers();
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

void Window::MouseFunc(int button, int state, int x, int y) {
    Window::Instance().MouseHandle(button, state, x, y);
}

void Window::KeyboardFunc(unsigned char key, int x, int y) {
    for (const auto &handler : Instance().keyboardHandlers) {
        handler(key);
    }
    Instance().KeyboardHandle(key, x, y);
}

void Window::MotionFunc(int x, int y) {
    Instance().CameraScroll(x, y);
    Instance().Refresh();
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
    if (units == nullptr) return;
    for (const auto &point : *units) {
        Vector pos(point + cellOffset);
        ClampVector(pos);
        DrawPoint(pos);
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

void Window::DrawNumbers() {
    glColor3f(1.0f, 1.0f, 1.0f);
    const float halfSize = cellSize * 0.5f;
    for (int x = 0; x < windowSize.x / cellSize; x++) {
        glRasterPos2f(x * cellSize + halfSize, 0.f);
        const int number = (x - cellOffset.x) % fieldSize.x;
        DrawNumber(number >= 0 ? number : fieldSize.x + number);
    }
    for (int y = 0; y < windowSize.y / cellSize; y++) {
        glRasterPos2f(0.f, y * cellSize + halfSize);
        const int number = (y - cellOffset.y) % fieldSize.y;
        DrawNumber(number >= 0 ? number : fieldSize.y + number);
    }
}

void Window::DrawNumber(int number) {
    const std::string &string = std::to_string(number);
    for (int i = 0; i < string.size(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, string[i]);
    }
}

void Window::MouseHandle(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        if (cameraScrolled) {
            cameraScrolled = false;
        } else {
            const Vector cell(x / cellSize, (windowSize.y - y) / cellSize);
            for (const auto &handler : mouseHandlers) {
                Vector fieldCell(cell - cellOffset);
                ClampVector(fieldCell);
                assert(fieldCell.x >= 0 && fieldCell.y >= 0);
                handler(fieldCell);
            }
        }
    }
    if (button == GLUT_LEFT_BUTTON) {
        if (!leftButtonPressed && state == GLUT_DOWN) {
            leftButtonPressed = true;
            leftButtonPressedPos = Vector(x, windowSize.y - y) - cellOffset * cellSize;
        } else if (leftButtonPressed && state == GLUT_UP) {
            leftButtonPressed = false;
        }
    }
}

void Window::KeyboardHandle(unsigned char key, int x, int y) {
    if (key == KeyEscape) {
        exit(0);
    } else if (key == KeyMinus) {
        cellSizeRatio -= cellSizeRatioStep;
        if (cellSizeRatio < cellSizeRatioMin) cellSizeRatio = cellSizeRatioMin;
    } else if (key == KeyPlus) {
        cellSizeRatio += cellSizeRatioStep;
        if (cellSizeRatio > cellSizeRatioMax) cellSizeRatio = cellSizeRatioMax;
    }
    RecalculateSize();
    Refresh();
}

void Window::CameraScroll(int x, int y) {
    if (!leftButtonPressed) return;
    cameraScrolled = true;
    const float sensitivity = cameraMoveSensititity;
    Vector newOffset = Vector(x, windowSize.y - y) - leftButtonPressedPos;
    newOffset *= sensitivity;
    newOffset /= cellSize;
    newOffset.x %= fieldSize.x;
    newOffset.y %= fieldSize.y;
    cellOffset = newOffset;
}

void Window::ClampVector(Vector &vec) {
    vec.x %= fieldSize.x;
    vec.y %= fieldSize.y;
    if (vec.x < 0) vec.x = fieldSize.x + vec.x;
    if (vec.y < 0) vec.y = fieldSize.y + vec.y;
}

void Window::AddMouseHandler(MouseHandler handler) {
    mouseHandlers.push_back(handler);
}

void Window::AddKeyboardHandler(KeyboardHandler handler) {
    keyboardHandlers.push_back(handler);
}

void Window::InitField(const std::vector<Vector> *units, Vector fieldSize) {
    this->units = units;
    this->fieldSize = fieldSize;
}

void Window::Refresh() {
    glutPostRedisplay();
}
