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
#include <stdlib.h>
#include "Window.hpp"
#include "GameField.hpp"
#include "Presets.hpp"

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
    presets(nullptr),
    loadedUnits(nullptr),
    loadedUnitsTRS(Matrix3x3::Identity()),
    rightButtonPressed(false),
    leftButtonPressed(false),
    cellSelected(false),
    cameraScrolled(false) {}

Window::~Window() {}

Window &Window::Instance() {
    static Window window;
    return window;
}

void Window::MainLoop(int &argc, char **argv, const char *label, Vector size) {
    if (gameField == nullptr || presets == nullptr) throw std::invalid_argument("GameField or Presets does not exist!");
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
    instance.DrawCell();
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
    Window &instance = Instance();
    if (button == GLUT_LEFT_BUTTON) {
        instance.LeftMouseHandle(Vector(x, y), state == GLUT_DOWN);
    } else if (button == GLUT_RIGHT_BUTTON) {
        instance.RightMouseHandle(Vector(x, y), state == GLUT_DOWN);
    }
}

void Window::KeyboardFunc(unsigned char key, int x, int y) {
    Instance().KeyboardHandle(key, Vector(x, y));
}

void Window::MotionFunc(int x, int y) {
    Window &instance = Instance();
    instance.cellSelected = false;
    instance.loadedUnits = nullptr;
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
    glColor3f(0.0f, 1.0f, 0.0f);
    for (const auto &unit : *gameField->GetUnits()) {
        Vector pos(unit + cellOffset);
        gameField->ClampVector(pos);
        DrawPoint(pos);
    }
    if (loadedUnits == nullptr) return;
    glColor3f(1.0f, 1.0f, 0.5f);
    for (const auto &unit : *loadedUnits) {
        Vector pos = Matrix3x3::Translation(cellOffset) * loadedUnitsTRS * unit;
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
        if (!selectedCells.empty()) selectedCells.clear();
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

void Window::DrawCell() {
    if (!cellSelected || loadedUnits != nullptr) return;
    Vector cell = ScreenToCell(rightButtonPressedPos) + cellOffset;
    gameField->ClampVector(cell);
    const float cellX = cell.x * cellSize;
    const float cellY = cell.y * cellSize;
    glColor3f(1.f, 1.f, 0.5f);
    glBegin(GL_POLYGON);
    glVertex2f(cellX, cellY);
    glVertex2f(cellX, cellY + cellSize);
    glVertex2f(cellX + cellSize, cellY + cellSize);
    glVertex2f(cellX + cellSize, cellY);
    glEnd();
}

void Window::LeftMouseHandle(Vector mousePos, bool pressed) {
    if (!pressed) {
        if (cameraScrolled) {
            cameraScrolled = false;
        } else {
            const Vector cell = ScreenToCell(mousePos);
            gameField->AddUnit(cell);
            Refresh();
        }
    }
    if (!leftButtonPressed && pressed) {
        leftButtonPressed = true;
        leftButtonPressedPos = Vector(mousePos.x, windowSize.y - mousePos.y) - cellOffset * cellSize;
    } else if (leftButtonPressed && !pressed) {
        leftButtonPressed = false;
    }
}

void Window::RightMouseHandle(Vector mousePos, bool pressed) {
    if (!pressed) {
        if (rightButtonPressedPos == mousePosition) {
            cellSelected = true;
            Refresh();
        }
    }
    if (!rightButtonPressed && pressed) {
        rightButtonPressed = true;
        if (loadedUnits == nullptr) {
            loadedUnitsTRS = Matrix3x3::Translation(ScreenToCell(mousePos));
        } else {
            loadedUnitsTRS = Matrix3x3::Translation(ScreenToCell(mousePos) - ScreenToCell(rightButtonPressedPos)) * loadedUnitsTRS;
        }
        rightButtonPressedPos = mousePos;
    } else if (rightButtonPressed && !pressed) {
        rightButtonPressed = false;
        CalulateSelectedCells();
    }
}

void Window::KeyboardHandle(unsigned char key, Vector mousePos) {
    if (key == KeyEscape) {
        presets->SaveOnDisk();
        exit(0);
    } else if (key == KeyMinus) {
        cellSizeRatio -= cellSizeRatioStep;
        if (cellSizeRatio < cellSizeRatioMin) cellSizeRatio = cellSizeRatioMin;
        RecalculateSize();
    } else if (key == KeyPlus) {
        cellSizeRatio += cellSizeRatioStep;
        if (cellSizeRatio > cellSizeRatioMax) cellSizeRatio = cellSizeRatioMax;
        RecalculateSize();
    } else if (key == KeySpace) {
        gameField->ProcessUnits();
    } else if (key >= '0' && key <= '9') {
        NumbersHandle(key, mousePos);
    } else if (loadedUnits != nullptr && (key == 'a' || key == 'd' || key == 'w' || key == 's')) {
        switch (key) {
            case 'a':
                loadedUnitsTRS = loadedUnitsTRS * Matrix3x3::Rotation(90.f);
                break;
            case 'd':
                loadedUnitsTRS = loadedUnitsTRS * Matrix3x3::Rotation(-90.f);
                break;
            case 'w':
                loadedUnitsTRS = loadedUnitsTRS * Matrix3x3::Scale(Vector(1, -1));
                break;
            case 's':
                loadedUnitsTRS = loadedUnitsTRS * Matrix3x3::Scale(Vector(-1, 1));
                break;
        }
    } else {
        cellSelected = false;
        loadedUnits = nullptr;
    }
    Refresh();
}

void Window::NumbersHandle(unsigned char key, Vector mousePos) {
    const std::vector<Vector> *cells = GetSelectedCells();
    if (!cells->empty()) {
        presets->Save(key, cells);
        return;
    }
    const std::vector<Vector> *newLoadedUnits = presets->Load(key);
    if (loadedUnits == newLoadedUnits && loadedUnits != nullptr) {
        for (int i = 0; i < loadedUnits->size(); i++) {
            Vector pos = loadedUnitsTRS * loadedUnits->at(i);
            gameField->ClampVector(pos);
            gameField->AddUnit(pos);
        }
        loadedUnits = nullptr;
        cellSelected = false;
    } else if (cellSelected) {
        loadedUnits = newLoadedUnits;
    }
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

Vector Window::CellToScreen(int x, int y) const {
    Vector result(x, y);
    result += cellOffset;
    gameField->ClampVector(result);
    result *= cellSize;
    result += Vector(cellSize, cellSize) / 2;
    result.y = windowSize.y - result.y;
    return result;
}

Vector Window::CellToScreen(Vector vec) const {
    return CellToScreen(vec.x, vec.y);
}

void Window::CalulateSelectedCells() const {
    selectedCells.clear();
    Vector min = mousePosition;
    Vector max = rightButtonPressedPos;
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
    const Rect screentRect(min, max - min);
    Vector cellMin = Vector::One() * std::numeric_limits<int>::max();
    Vector cellMax = Vector::One() * std::numeric_limits<int>::min();
    for (const auto &unit : *gameField->GetUnits()) {
        Vector screenUnit = CellToScreen(unit);
        if (screentRect.Contains(screenUnit)) {
            cellMin.x = screenUnit.x < cellMin.x ? screenUnit.x : cellMin.x;
            cellMin.y = screenUnit.y < cellMin.y ? screenUnit.y : cellMin.y;
            cellMax.x = screenUnit.x > cellMax.x ? screenUnit.x : cellMax.x;
            cellMax.y = screenUnit.y > cellMax.y ? screenUnit.y : cellMax.y;
            selectedCells.push_back(screenUnit);
        }
    }
    const Rect rect(cellMin, cellMax - cellMin);
    const Vector center = rect.Center();
    for (int i = 0; i < selectedCells.size(); i++) {
        selectedCells[i] = selectedCells[i] - center;
        float cellX = selectedCells[i].x / cellSize;
        float cellY = selectedCells[i].y / cellSize;
        selectedCells[i] = Vector(ceilf(cellX + 0.5f), -ceilf(cellY + 0.5f));
    }
}

const std::vector<Vector> *Window::GetSelectedCells() const {
    if (rightButtonPressed) {
        CalulateSelectedCells();
    }
    return &selectedCells;
}

void Window::Init(GameField *gameField, Presets *presets) {
    this->gameField = gameField;
    this->presets = presets;
}

void Window::Refresh() const {
    glutPostRedisplay();
}

Vector Window::GetCellUnderMouse() const {
    return ScreenToCell(mousePosition.x, mousePosition.y);
}
