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
const int Window::KeySpace = 32;

Window::Window() :
    cameraMoveSensititity(1.f),
    unitAngleStep(10.0f),
    unitRadiusRatio(0.3f),
    cellSizeRatioMin(0.02f),
    cellSizeRatioMax(0.1f),
    cellSizeRatioStep(0.01f),
    cellSizeRatio(0.05f),
    cellSize(0.0f),
    gameField(nullptr),
    rightButtonPressed(false),
    leftButtonPressed(false),
    cameraScrolled(false) {}

Window::~Window() {}

Window &Window::Instance() {
    static Window window;
    return window;
}

void Window::MainLoop(int &argc, char **argv, const char *label, Vector size) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(size.x, size.y);
    glutCreateWindow(label);
    glutReshapeFunc(Window::Reshape);
    glutDisplayFunc(Window::Display);
    glutMouseFunc(Window::MouseFunc);
    glutKeyboardFunc(Window::KeyboardFunc);
    glutMotionFunc(Window::MotionFunc);
    glutPassiveMotionFunc(Window::PassiveMotionFunc);
    Instance().RecalculateSize();
    glutMainLoop();
}

void Window::Display() {
    Window &instance = Instance();
    glClear(GL_COLOR_BUFFER_BIT);
    instance.DrawGrid();
    instance.DrawPoints();
    instance.DrawNumbers();
    instance.DrawRect();
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
    Instance().MouseHandle(button, state, x, y);
}

void Window::KeyboardFunc(unsigned char key, int x, int y) {
    Instance().KeyboardHandle(key, x, y);
}

void Window::MotionFunc(int x, int y) {
    Window &instance = Instance();
    instance.mousePosition = Vector(x, y);
    if (instance.leftButtonPressed) {
        instance.CameraScroll(x, y);
    }
    instance.Refresh();
}

void Window::PassiveMotionFunc(int x, int y) {
    Instance().mousePosition = Vector(x, y);
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
    if (gameField == nullptr) return;
    for (const auto &unit : *gameField->GetUnits()) {
        Vector pos(unit + cellOffset);
        gameField->ClampVector(pos);
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
    const Vector &fieldSize = gameField->GetSize();
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

void Window::DrawRect() {
    if (!rightButtonPressed || rightButtonPressedPos == mousePosition) {
        if (!selectedCells.IsZero()) selectedCells = Rect();
        return;
    }
    const Vector min = rightButtonPressedPos;
    const Vector max = mousePosition;
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(min.x, windowSize.y - min.y);
    glVertex2f(min.x, windowSize.y - max.y);
    glVertex2f(max.x, windowSize.y - max.y);
    glVertex2f(max.x, windowSize.y - min.y);
    glEnd();
}

void Window::MouseHandle(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_UP) {
            if (cameraScrolled) {
                cameraScrolled = false;
            } else {
                const Vector fieldCell = ScreenToCell(x, y);
                for (const auto &handler : mouseHandlers) {
                    handler(fieldCell);
                }
            }
        }
        if (!leftButtonPressed && state == GLUT_DOWN) {
            leftButtonPressed = true;
            leftButtonPressedPos = Vector(x, windowSize.y - y) - cellOffset * cellSize;
        } else if (leftButtonPressed && state == GLUT_UP) {
            leftButtonPressed = false;
        }
    } else if (button == GLUT_RIGHT_BUTTON) {
        if (!rightButtonPressed && state == GLUT_DOWN) {
            rightButtonPressed = true;
            rightButtonPressedPos = Vector(x, y);
        } else if (rightButtonPressed && state == GLUT_UP) {
            rightButtonPressed = false;
            selectedCells = CalulateSelectedCells();
        }
    }
}

void Window::KeyboardHandle(unsigned char key, int x, int y) {
    for (const auto &handler : keyboardHandlers) {
        handler(key);
    }
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
    const float sensitivity = cameraMoveSensititity;
    const Vector &fieldSize = gameField->GetSize();
    Vector newOffset = Vector(x, windowSize.y - y) - leftButtonPressedPos;
    cameraScrolled = Vector::Dot(newOffset, newOffset) >= cellSize * cellSize;
    newOffset *= sensitivity;
    newOffset /= cellSize;
    newOffset.x %= fieldSize.x;
    newOffset.y %= fieldSize.y;
    cellOffset = newOffset;
}

Vector Window::ScreenToCell(int x, int y) const {
    Vector result(x / cellSize, (windowSize.y - y) / cellSize);
    result -= cellOffset;
    gameField->ClampVector(result);
    return result;
}

Vector Window::ScreenToCell(Vector vec) const {
    return ScreenToCell(vec.x, vec.y);
}

Rect Window::CalulateSelectedCells() const {
    Vector min = ScreenToCell(mousePosition);
    Vector max = ScreenToCell(rightButtonPressedPos);
    if (min.x > max.x) {
        const int tempX = min.x;
        min.x = max.x;
        max.x = tempX;
    }
    if (min.y > max.y) {
        const int tempY = min.y;
        min.y = max.y;
        max.y = tempY;
    }
    return Rect(min, max - min);
}

void Window::AddMouseHandler(MouseHandler handler) {
    mouseHandlers.push_back(handler);
}

void Window::AddKeyboardHandler(KeyboardHandler handler) {
    keyboardHandlers.push_back(handler);
}

void Window::InitField(const GameField *gameField) {
    this->gameField = gameField;
}

void Window::Refresh() const {
    glutPostRedisplay();
}

Vector Window::GetCellUnderMouse() const {
    return ScreenToCell(mousePosition.x, mousePosition.y);
}

Rect Window::GetSelectedCells() const {
    if (rightButtonPressed) {
        return CalulateSelectedCells();
    } else {
        return selectedCells;
    }
}
