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
#include <functional>
#include "GameField.hpp"
#include "Vector.hpp"
#include "Rect.hpp"

typedef std::function<void(Vector)> MouseHandler;
typedef std::function<void(unsigned char)> KeyboardHandler;

class Window {
    Rect selectedCells;
    Vector rightButtonPressedPos;
    Vector leftButtonPressedPos;
    Vector cellOffset;
    Vector windowSize;
    Vector mousePosition;
    
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
    
    std::vector<MouseHandler> mouseHandlers;
    std::vector<KeyboardHandler> keyboardHandlers;
    const GameField *gameField;
    
public:
    const static int KeyMinus;
    const static int KeyPlus;
    const static int KeyEscape;
    const static int KeySpace;
    
    static Window &Instance();
    
    void MainLoop(int &argc, char **argv, const char *label, Vector size);
    void AddMouseHandler(MouseHandler handler);
    void AddKeyboardHandler(KeyboardHandler handler);
    void InitField(const GameField *gameField);
    void Refresh() const;
    Vector GetCellUnderMouse() const;
    Rect GetSelectedCells() const;
    
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
    
    void MouseHandle(int button, int state, int x, int y);
    void KeyboardHandle(unsigned char key, int x, int y);
    void CameraScroll(int x, int y);
    Vector ScreenToCell(int x, int y) const;
    Vector ScreenToCell(Vector vec) const;
    Vector CellToScreen(int x, int y) const;
    Vector CellToScreen(Vector vec) const;
    Rect CalulateSelectedCells() const;
    
    Window();
    ~Window();
    Window(const Window &other);
    Window &operator = (const Window &other);
};

#endif /* Window_hpp */
