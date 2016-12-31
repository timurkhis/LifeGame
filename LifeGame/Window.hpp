//
//  Window.hpp
//  LifeGame
//
//  Created by Максим Бакиров on 15.12.16.
//  Copyright © 2016 Arsonist (gmoximko@icloud.com). All rights reserved.
//

#ifndef Window_hpp
#define Window_hpp

#include <vector>
#include "Geometry.h"

using namespace Geometry;

class Window {
    const static int KeyMinus;
    const static int KeyPlus;
    const static int KeyEscape;
    const static int KeySpace;
    
    mutable std::vector<Vector> selectedCells;
    Vector rightButtonPressedPos;
    Vector leftButtonPressedPos;
    Vector cellOffset;
    Vector windowSize;
    Vector mousePosition;
    Matrix3x3 loadedUnitsTRS;
    
    bool cellSelected;
    bool cameraScrolled;
    bool rightButtonPressed;
    bool leftButtonPressed;
    float cellSizeRatio;
    float cellSize;
    const float cameraMoveSensititity;
    const float unitAngleStep;
    const float unitRadiusRatio;
    const float cellSizeRatioMin;
    const float cellSizeRatioMax;
    const float cellSizeRatioStep;
    
    class GameField *gameField;
    class Presets *presets;
    const std::vector<Vector> *loadedUnits;
    
public:
    static Window &Instance();
    
    void MainLoop(int &argc, char **argv, const char *label, Vector size);
    void Init(GameField *gameField, Presets *presets);
    void Refresh() const;
    Vector GetCellUnderMouse() const;
    bool CellSelected() const { return cellSelected; }
    
private:
    static void Reshape(int w, int h);
    static void Display();
    static void MouseFunc(int button, int state, int x, int y);
    static void KeyboardFunc(unsigned char key, int x, int y);
    static void MotionFunc(int x, int y);
    static void PassiveMotionFunc(int x, int y);
    
    void DrawGrid();
    void DrawPoints();
    void DrawPoint(Vector pos);
    void RecalculateSize();
    void DrawNumbers();
    void DrawNumber(int number);
    void DrawRect();
    void DrawCell();
    
    void Zoom(float zoom);
    void LeftMouseHandle(Vector mousePos, bool pressed);
    void RightMouseHandle(Vector mousePos, bool pressed);
    void KeyboardHandle(unsigned char key, Vector mousePos);
    void NumbersHandle(unsigned char key, Vector mousePos);
    void CameraScroll(Vector pos);
    Vector ScreenToCell(int x, int y) const;
    Vector ScreenToCell(Vector vec) const;
    Vector CellToScreen(int x, int y) const;
    Vector CellToScreen(Vector vec) const;
    void CalulateSelectedCells() const;
    const std::vector<Vector> *GetSelectedCells() const;
    
    explicit Window();
    ~Window();
    Window(const Window &other) = delete;
    Window &operator = (const Window &other) = delete;
};

#endif /* Window_hpp */
