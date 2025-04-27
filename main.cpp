#include <QApplication>
#include "snake.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle("Fusion");
    SnakeGame game;
    game.show();

    return a.exec();
}
