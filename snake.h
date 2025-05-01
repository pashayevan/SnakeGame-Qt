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
#include <QMediaPlayer>
#include <QAudioOutput>


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

    // Цветовая палитра
    static constexpr QRgb BACKGROUND_COLOR = 0xF0E5D8;  // Песочный фон, если не загрузится изображение
    static constexpr QRgb SNAKE_HEAD_COLOR = 0x5E1914;  // Темно-бордовая голова
    static constexpr QRgb SNAKE_BODY_1 = 0x8B4513;      // Коричневое тело
    static constexpr QRgb SNAKE_BODY_2 = 0xA0522D;      // Светло-коричневое тело
    static constexpr QRgb FOOD_COLOR = 0xD4A017;        // Золотистая еда
    static constexpr QRgb BORDER_COLOR = 0x5E1914;      // Бордовая рамка

    bool movePending;
    int nextDirection;
    QVector<QPoint> snake;
    QPoint food;
    int direction;
    bool gameRunning;
    QPixmap backgroundImage;


    QFrame *gameFrame;
    QLabel *scoreLabel;
    QLabel *highScoreLabel;
    QPushButton *playButton;
    QPushButton *restartButton;
    QVBoxLayout *mainLayout;
    QMediaPlayer *musicPlayer;
    QAudioOutput *audioOutput;


    int score;
    int highScore;
    static const int DOT_SIZE = 20;
    static const int FIELD_WIDTH = 20;
    static const int FIELD_HEIGHT = 15;

    QTimer *timer;
};

#endif // SNAKE_H
