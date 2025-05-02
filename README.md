# 🐍 Qt Snake Game

[![Qt Version](https://img.shields.io/badge/Qt-6.0%2B-brightgreen)](https://www.qt.io/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++ Version](https://img.shields.io/badge/C%2B%2B-11%2B-blue)](https://isocpp.org/)

A classic Snake game implementation using Qt framework with modern UI features and smooth gameplay.

<p align="center">
  <img src="snake.png" style="width: 300px;" />
</p>

## ✨ Features

- 🕹️ Intuitive controls (WASD or Arrow keys)
- 📊 Score system with persistent high score saving
- ⚡ Dynamic game speed acceleration as you progress
- 🎨 Visually appealing design with:
- 🐍 Triangular snake head that changes direction
- 🌳 Alternating body segment colors
- 🍪 Golden food items
- 🏞 Custom background (sand color fallback)
- ⏸️ Automatic pause when window loses focus
- 🔄 Restart functionality with dedicated button
- 🖌️ Custom rendering using QPainter

## 🛠️ Building the Project

### Prerequisites
- Qt 6.0 or later
- C++11 compatible compiler

### Build Instructions
```bash
sudo apt install qtchooser
sudo apt install qt5-qmake
sudo apt install qtbase5-dev qt5-qmake

git clone https://github.com/pashayevan/SnakeGame-Qt.git
cd SnakeGame-Qt
qmake SnakeGame-Qt.pro
make
./SnakeGame-Qt

```
- 🤲🏻 If there is an error:
```bash
Project ERROR: Unknown module(s) in QT: multimedia
```
It means that your Qt installation does not include the Qt Multimedia module, or it is not properly installed/configured.

✒️ Here’s how you can fix it depending on your environment:
```bash
sudo apt install qtmultimedia5-dev
```
---

### 🎮 How to Play

1.Launch the game

2.Click "Play" button or any of the control buttons on keypad

3.Use arrow keys to control the snake

4.Eat the golden food to grow and score points

5.Avoid walls and your own tail

6.Game speeds up every 5 points

### 🏗️ Project Structure
```bash
qt-snake-game/
├── snake.h          # Main game class declaration
├── snake.cpp        # Game logic implementation
├── main.cpp         # Application entry point
├── SnakeGame-Qt.pro   # pro-file build configuration

```

### 🧠 Technical Details
Core Components
> SnakeGame (QWidget subclass) - Main game window

> QTimer - Game loop controller

> QPainter - Custom rendering engine

> QSettings - Persistent high score storage

## 🎨 Color Palette

| Element         | Hex Code  | Sample |
|-----------------|-----------|--------|
| Background      | `#F0E5D8` | 🟫      |
| Snake Head      | `#5E1914` | 🟤      |
| Body Segment 1  | `#8B4513` | 🟫      |
| Body Segment 2  | `#A0522D` | 🟤      |
| Food            | `#D4A017` | 🟨      |
| Borders         | `#5E1914` | 🟤      |

### Key Algorithms
```cpp
void SnakeGame::gameLoop()
{
    if (!gameRunning) return;

    if (movePending) {
        direction = nextDirection;
        movePending = false;
    }

    QPoint head = snake.first();
    switch (direction) {
    case 0: head.ry()--; break;
    case 1: head.rx()++; break;
    case 2: head.ry()++; break;
    case 3: head.rx()--; break;
    }

    snake.prepend(head);
    if (head == food) {
        score++;
        updateScore();
        generateFood();

        if (score % 5 == 0) {
            int newInterval = timer->interval() - 10;
            if (newInterval < 50) {
                newInterval = 50;
            }
            timer->setInterval(newInterval);
        }
    } else {
        snake.removeLast();
    }
    checkCollision();
    update();
}
```


