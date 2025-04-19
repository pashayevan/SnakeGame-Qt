#ifndef SNAKE_H
#define SNAKE_H

#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QKeyEvent>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFrame>

class SnakeGame : public QWidget
{
    Q_OBJECT

public:
    explicit SnakeGame(QWidget *parent = nullptr);
    ~SnakeGame();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void startGame();
    void restartGame();
    void gameLoop();

private:
    void initGame();
    void generateFood();
    void checkCollision();
    void updateScore();
    void gameOver();

    // Game elements
    QVector<QPoint> snake;
    QPoint food;
    int direction; // 0: up, 1: right, 2: down, 3: left
    bool gameRunning;

    // UI elements
    QFrame *gameFrame;
    QLabel *scoreLabel;
    QLabel *highScoreLabel;
    QPushButton *playButton;
    QPushButton *restartButton;
    QVBoxLayout *mainLayout;

    // Game stats
    int score;
    int highScore;
    static const int DOT_SIZE = 20;
    static const int FIELD_WIDTH = 20;
    static const int FIELD_HEIGHT = 15;

    // Timer
    QTimer *timer;
};

#endif // SNAKE_H
