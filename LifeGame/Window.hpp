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
#include "Vector.hpp"

typedef std::function<void(Vector)> MouseHandler;
typedef std::function<void(unsigned char)> KeyboardHandler;

class Window {
    Vector leftButtonPressedPos;
    Vector cellOffset;
    Vector windowSize;
    Vector fieldSize;
    
    bool leftButtonPressed;
    float cameraMoveSensititity;
    float cellSizeRatio;
    float cellSize;
    float unitAngleStep;
    float unitRadiusRatio;
    
    std::vector<MouseHandler> mouseHandlers;
    std::vector<KeyboardHandler> keyboardHandlers;
    const std::vector<Vector> *units;
    
public:
    static Window &Instance();
    
    void MainLoop(int &argc, char **argv, const char *label, int width, int heigth);
    void AddMouseHandler(MouseHandler handler);
    void AddKeyboardHandler(KeyboardHandler handler);
    void InitField(const std::vector<Vector> *units, Vector fieldSize);
    void Refresh();
    
private:
    static void Reshape(int w, int h);
    static void Display();
    static void MouseFunc(int button, int state, int x, int y);
    static void KeyboardFunc(unsigned char key, int x, int y);
    static void MotionFunc(int x, int y);
    
    void DrawGrid();
    void DrawPoints();
    void DrawPoint(Vector pos);
    void RecalculateSize();
    void DrawNumbers();
    void DrawNumber(int number);
    
    void MouseHandle(int button, int state, int x, int y);
    void CameraScroll(int x, int y);
    void ClampVector(Vector &vec);
    
    Window();
    ~Window();
    Window(const Window &other);
    Window &operator = (const Window &other);
};

#endif /* Window_hpp */
