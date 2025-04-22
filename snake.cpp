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
    // Устанавливаем фон через палитру (самый эффективный способ)
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor::fromRgb(BACKGROUND_COLOR));
    setPalette(pal);

    // Игровое поле
    gameFrame->setFrameStyle(QFrame::Box | QFrame::Plain);
    gameFrame->setLineWidth(3);
    QPalette framePal = gameFrame->palette();
    framePal.setColor(QPalette::Window, QColor::fromRgb(0xF8F4E6));
    framePal.setColor(QPalette::WindowText, QColor::fromRgb(BORDER_COLOR));
    gameFrame->setPalette(framePal);
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
    nextDirection = 2; // То же начальное направление
    movePending = false; // Нет ожидающих движений
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

    if (movePending) {
        direction = nextDirection;
        movePending = false;
    }

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
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Рисуем игровое поле
    painter.translate(gameFrame->geometry().topLeft() + QPoint(3, 3));

    // Заливка поля (оптимальный способ)
    painter.fillRect(0, 0, FIELD_WIDTH*DOT_SIZE, FIELD_HEIGHT*DOT_SIZE,
                     QColor::fromRgb(0xF8F4E6));

    // Тело змейки
    for(int i = 0; i < snake.size(); ++i) {
        painter.setBrush(QColor::fromRgb(i % 2 ? SNAKE_BODY_1 : SNAKE_BODY_2));
        painter.drawEllipse(snake[i].x()*DOT_SIZE, snake[i].y()*DOT_SIZE,
                            DOT_SIZE, DOT_SIZE);
    }

    // Голова змейки (ромб)
    if(!snake.isEmpty()) {
        painter.setBrush(QColor::fromRgb(SNAKE_HEAD_COLOR));
        QPoint head = snake.first();
        int x = head.x() * DOT_SIZE;
        int y = head.y() * DOT_SIZE;

        QPoint points[4] = {
            QPoint(x + DOT_SIZE/2, y),
            QPoint(x + DOT_SIZE, y + DOT_SIZE/2),
            QPoint(x + DOT_SIZE/2, y + DOT_SIZE),
            QPoint(x, y + DOT_SIZE/2)
        };
        painter.drawPolygon(points, 4);
    }

    // Пахлава (ромб с узором)
    painter.setBrush(QColor::fromRgb(FOOD_COLOR));
    int fx = food.x() * DOT_SIZE;
    int fy = food.y() * DOT_SIZE;

    QPoint foodPoints[4] = {
        QPoint(fx + DOT_SIZE/2, fy),
        QPoint(fx + DOT_SIZE, fy + DOT_SIZE/2),
        QPoint(fx + DOT_SIZE/2, fy + DOT_SIZE),
        QPoint(fx, fy + DOT_SIZE/2)
    };
    painter.drawPolygon(foodPoints, 4);
}

void SnakeGame::keyPressEvent(QKeyEvent *event)
{
    if (!gameRunning) {
        QWidget::keyPressEvent(event);
        return;
    }

    int newDirection = direction; // По умолчанию оставляем текущее направление

    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        if (direction != 2) newDirection = 0; // Вверх
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        if (direction != 3) newDirection = 1; // Вправо
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        if (direction != 0) newDirection = 2; // Вниз
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        if (direction != 1) newDirection = 3; // Влево
        break;
    default:
        QWidget::keyPressEvent(event);
        return;
    }

    // Если направление изменилось, сохраняем его для следующего хода
    if (newDirection != direction) {
        nextDirection = newDirection;
        movePending = true;
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
