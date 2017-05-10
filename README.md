# LifeGame
The purpose of this project is a creation of multiplayer strategy game where players don't have to build buildings or control units, instead they should create life in order to eliminate all others life.

Single life and its patterns behave exactly like in a Conway's Game of Life as long as cells of different players don't collide with each other. Of course it is very difficult to manipulate with only one cell, so players put on the field different patterns of cells in order to destroy others.

- project does not use any framework and written fully in C++. It works on Windows and Mac, hopefully on Linux too
- simple graphics drawn with OpenGL, player input handles by GLUT
- all network connections implemented with C socket library
- game uses peer to peer connection scheme
- game uses lockstep mechanism and some checks to synchronize instances
- game works also in a single player regime

Game supports some flags:
- "field 1000 1000" - size of universe in cells
- "window 800 600" - size of window
- "server 127.0.0.1:1000" - address of master game instance to connect with
- "presets ../presets.txt" - where file with patterns located
- "turn 10" - game speed, how often game instances performs player commands and sends it to each other
- "players 2" - player count for a single session
Unfortunatly, they were practically not tested.

To launch the game:
1) At first you should build it, then run master instance with any flags except "server".
2) Game window should appear and port number also appears.
3) With this port number you can run slave instances with flag "server address:port".
4) Game will not start until all players are connected.
5) Camera scroll available with left mouse button pressed.
6) To put some pattern on the field click right mouse button and then press any number 1- 9 on your keyboard. You can also flip or rotate pattern with WASD buttons.
7) Some debug info appears in stdout while game is running.
