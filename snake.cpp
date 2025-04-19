#include "snake.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QMessageBox>
#include <QSettings>
#include <QRandomGenerator>

SnakeGame::SnakeGame(QWidget *parent) : QWidget(parent)
{
    // Load high score from settings
    QSettings settings;
    highScore = settings.value("highScore", 0).toInt();

    // Initialize UI
    setWindowTitle("Snake Game");
    resize(600, 500);

    // Create main layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);

    // Create score labels
    scoreLabel = new QLabel("Score: 0", this);
    highScoreLabel = new QLabel(QString("High Score: %1").arg(highScore), this);

    // Style score labels
    QFont scoreFont("Arial", 14, QFont::Bold);
    scoreLabel->setFont(scoreFont);
    highScoreLabel->setFont(scoreFont);

    // Add score labels to layout
    QHBoxLayout *scoreLayout = new QHBoxLayout();
    scoreLayout->addWidget(scoreLabel);
    scoreLayout->addStretch();
    scoreLayout->addWidget(highScoreLabel);
    mainLayout->addLayout(scoreLayout);

    // Create game frame
    gameFrame = new QFrame(this);
    gameFrame->setFrameStyle(QFrame::Box | QFrame::Plain);
    gameFrame->setLineWidth(2);
    gameFrame->setFixedSize(FIELD_WIDTH * DOT_SIZE + 4, FIELD_HEIGHT * DOT_SIZE + 4);
    mainLayout->addWidget(gameFrame, 0, Qt::AlignCenter);

    // Create buttons
    playButton = new QPushButton("Play", this);
    restartButton = new QPushButton("Restart", this);

    // Style buttons
    playButton->setFixedSize(100, 40);
    restartButton->setFixedSize(100, 40);
    QFont buttonFont("Arial", 12);
    playButton->setFont(buttonFont);
    restartButton->setFont(buttonFont);

    // Add buttons to layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(restartButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // Connect buttons
    connect(playButton, &QPushButton::clicked, this, &SnakeGame::startGame);
    connect(restartButton, &QPushButton::clicked, this, &SnakeGame::restartGame);

    // Initialize game state
    gameRunning = false;
    restartButton->setVisible(false);

    // Create timer
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SnakeGame::gameLoop);

    // Initialize game state
    gameRunning = false;
    restartButton->setVisible(false);

    // Критически важная строка - добавляем возможность получать фокус
    setFocusPolicy(Qt::StrongFocus);
}


SnakeGame::~SnakeGame()
{
    // Save high score
    QSettings settings;
    settings.setValue("highScore", highScore);
}

void SnakeGame::startGame()
{
    playButton->setVisible(false);
    restartButton->setVisible(false);
    initGame();
    timer->start(150);
    setFocus();  // Устанавливаем фокус на виджет
}

void SnakeGame::restartGame()
{
    restartButton->setVisible(false);
    initGame();
    timer->start(150);
    setFocus();  // Устанавливаем фокус на виджет
}

void SnakeGame::initGame()
{
    snake.clear();
    snake.append(QPoint(5, 7));
    snake.append(QPoint(5, 6));
    snake.append(QPoint(5, 5));
    direction = 2; // Start moving down
    score = 0;
    updateScore();
    generateFood();
    gameRunning = true;
    update();
}

void SnakeGame::generateFood()
{
    // Generate random position for food
    int x, y;
    do {
        x = QRandomGenerator::global()->bounded(FIELD_WIDTH);
        y = QRandomGenerator::global()->bounded(FIELD_HEIGHT);
    } while (snake.contains(QPoint(x, y)));

    food = QPoint(x, y);
}

void SnakeGame::gameLoop()
{
    if (!gameRunning) return;

    // Move snake
    QPoint head = snake.first();
    switch (direction) {
    case 0: head.ry()--; break; // Up
    case 1: head.rx()++; break; // Right
    case 2: head.ry()++; break; // Down
    case 3: head.rx()--; break; // Left
    }

    snake.prepend(head);

    // Check if snake ate food
    if (head == food) {
        score++;
        updateScore();
        generateFood();
    } else {
        snake.removeLast(); // Remove tail if no food eaten
    }

    checkCollision();
    update();
}

void SnakeGame::checkCollision()
{
    QPoint head = snake.first();

    // Check wall collision
    if (head.x() < 0 || head.x() >= FIELD_WIDTH ||
        head.y() < 0 || head.y() >= FIELD_HEIGHT) {
        gameOver();
        return;
    }

    // Check self collision
    for (int i = 1; i < snake.size(); ++i) {
        if (head == snake.at(i)) {
            gameOver();
            return;
        }
    }
}

void SnakeGame::gameOver()
{
    gameRunning = false;
    timer->stop();

    if (score > highScore) {
        highScore = score;
        highScoreLabel->setText(QString("High Score: %1").arg(highScore));
    }

    restartButton->setVisible(true);
}

void SnakeGame::updateScore()
{
    scoreLabel->setText(QString("Score: %1").arg(score));
}

void SnakeGame::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event); // Помечаем параметр как неиспользуемый

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Рисуем игровое поле
    painter.translate(gameFrame->geometry().topLeft() + QPoint(2, 2));
    painter.fillRect(0, 0, FIELD_WIDTH * DOT_SIZE, FIELD_HEIGHT * DOT_SIZE, Qt::white);

    if (!gameRunning && snake.isEmpty()) return;

    // Рисуем еду
    painter.setBrush(Qt::red);
    painter.drawEllipse(food.x() * DOT_SIZE, food.y() * DOT_SIZE, DOT_SIZE, DOT_SIZE);

    // Рисуем тело змейки
    painter.setBrush(Qt::green);
    for (const QPoint &point : snake) {
        painter.drawEllipse(point.x() * DOT_SIZE, point.y() * DOT_SIZE, DOT_SIZE, DOT_SIZE);
    }

    // Рисуем голову змейки другим цветом
    if (!snake.isEmpty()) {
        painter.setBrush(Qt::darkGreen);
        painter.drawEllipse(snake.first().x() * DOT_SIZE,
                            snake.first().y() * DOT_SIZE,
                            DOT_SIZE, DOT_SIZE);
    }
}
void SnakeGame::keyPressEvent(QKeyEvent *event)
{
    if (!gameRunning) {
        QWidget::keyPressEvent(event);
        return;
    }

    // Обрабатываем только стрелки и WASD
    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        if (direction != 2) direction = 0; // Вверх
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        if (direction != 3) direction = 1; // Вправо
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        if (direction != 0) direction = 2; // Вниз
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        if (direction != 1) direction = 3; // Влево
        break;
    default:
        QWidget::keyPressEvent(event);
        return;
    }

    event->accept();
}

void SnakeGame::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    if (gameRunning) {
        timer->start();
    }
}

void SnakeGame::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    if (gameRunning) {
        timer->stop();
    }
}
