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

typedef std::function<void(void)> IdleHandler;
typedef std::function<void(int, int, int, int)> MouseHandler;
typedef std::function<void(unsigned char, int, int)> KeyboardHandler;

class Window {
    float cellSizeRatio;
    float cellSize;
    float unitAngleStep;
    float unitRadiusRatio;
    Vector windowSize;
    std::vector<IdleHandler> idleHandlers;
    std::vector<MouseHandler> mouseHandlers;
    std::vector<KeyboardHandler> keyboardHandlers;
    std::vector<Vector> units;
    
public:
    static Window &Instance();
    
    void MainLoop(int &argc, char **argv, const char *label, int width, int heigth);
    void AddIdleHandler(IdleHandler handler);
    void AddMouseHandler(MouseHandler handler);
    void AddKeyboardHandler(KeyboardHandler handler);
    
private:
    static void Reshape(int w, int h);
    static void Display();
    static void IdleFunc();
    static void MouseFunc(int button, int state, int x, int y);
    static void KeyboardFunc(unsigned char key, int x, int y);
    
    void DrawGrid();
    void DrawPoints();
    void DrawPoint(Vector pos);
    void RecalculateSize();
    
    Window();
    ~Window();
    Window(const Window &other);
    Window &operator = (const Window &other);
};

#endif /* Window_hpp */
