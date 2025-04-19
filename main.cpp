#include <QApplication>
#include "snake.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set style for better appearance
    a.setStyle("Fusion");

    // Create and show game
    SnakeGame game;
    game.show();

    return a.exec();
}
