#include "snake.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QSettings>
#include <QRandomGenerator>

SnakeGame::SnakeGame(QWidget *parent) : QWidget(parent)
{
    QSettings settings;
    highScore = settings.value("highScore", 0).toInt();

    setWindowTitle("Snake Game");
    resize(600, 500);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);

    scoreLabel = new QLabel("Score: 0", this);
    highScoreLabel = new QLabel(QString("High Score: %1").arg(highScore), this);

    QFont scoreFont("Arial", 14, QFont::Bold);
    scoreLabel->setFont(scoreFont);
    highScoreLabel->setFont(scoreFont);

    scoreLabel->setStyleSheet("border: 2px solid black; background-color: brown; padding: 4px;");
    highScoreLabel->setStyleSheet("border: 2px solid black; background-color: brown; padding: 4px;");


    QHBoxLayout *scoreLayout = new QHBoxLayout();
    scoreLayout->addWidget(scoreLabel);
    scoreLayout->addStretch();
    scoreLayout->addWidget(highScoreLabel);
    mainLayout->addLayout(scoreLayout);

    gameFrame = new QFrame(this);
    gameFrame->setFrameStyle(QFrame::Box | QFrame::Plain);
    gameFrame->setLineWidth(2);
    gameFrame->setFixedSize(FIELD_WIDTH * DOT_SIZE + 4, FIELD_HEIGHT * DOT_SIZE + 4);

    // Добавляем игровое поле с выравниванием по центру и без растяжения
    mainLayout->addWidget(gameFrame, 0, Qt::AlignCenter);
    mainLayout->setAlignment(gameFrame, Qt::AlignCenter);  // Фиксируем позицию

    playButton = new QPushButton("Play", this);
    restartButton = new QPushButton("Restart", this);

    playButton->setFixedSize(100, 40);
    restartButton->setFixedSize(100, 40);
    QFont buttonFont("Arial", 12);
    playButton->setFont(buttonFont);
    restartButton->setFont(buttonFont);

    // Создаем отдельный контейнер для кнопок, чтобы они не влияли на положение игрового поля
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(restartButton);
    buttonLayout->addStretch();

    // Добавляем кнопки внизу, но без влияния на остальную компоновку
    mainLayout->addStretch();  // Растягиваем пространство между игровым полем и кнопками
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();  // Дополнительное пространство внизу (опционально)

    connect(playButton, &QPushButton::clicked, this, &SnakeGame::startGame);
    connect(restartButton, &QPushButton::clicked, this, &SnakeGame::restartGame);

    gameRunning = false;
    restartButton->setVisible(false);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SnakeGame::gameLoop);

    setFocusPolicy(Qt::StrongFocus);
    QPalette pal = palette();
    backgroundImage = QPixmap("../../background.jpg");
    pal.setColor(QPalette::Window, QColor::fromRgb(BACKGROUND_COLOR));
    setPalette(pal);

    QPalette framePal = gameFrame->palette();
    framePal.setColor(QPalette::Window, QColor::fromRgb(0xF8F4E6));
    framePal.setColor(QPalette::WindowText, QColor::fromRgb(BORDER_COLOR));
    gameFrame->setPalette(framePal);
}
SnakeGame::~SnakeGame()
{
    QSettings settings;
    settings.setValue("highScore", highScore);
}

void SnakeGame::startGame()
{
    playButton->setVisible(false);
    restartButton->setVisible(false);
    initGame();
    timer->start(150);
    setFocus();
}

void SnakeGame::restartGame()
{
    restartButton->setVisible(false);
    initGame();
    timer->start(150);
    setFocus();
}

void SnakeGame::initGame()
{
    snake.clear();
    snake.append(QPoint(5, 7));
    snake.append(QPoint(5, 6));
    snake.append(QPoint(5, 5));
    direction = 2;
    nextDirection = 2;
    movePending = false;
    score = 0;
    updateScore();
    generateFood();
    gameRunning = true;
    update();
}

void SnakeGame::generateFood()
{
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

        // --- Добавляем ускорение здесь ---
        if (score % 5 == 0) {  // каждые 5 очков
            int newInterval = timer->interval() - 10;
            if (newInterval < 50) {
                newInterval = 50;  // минимальная скорость
            }
            timer->setInterval(newInterval);
        }
        // -----------------------------------
    } else {
        snake.removeLast();
    }


    checkCollision();
    update();
}

void SnakeGame::checkCollision()
{
    QPoint head = snake.first();

    if (head.x() < 0 || head.x() >= FIELD_WIDTH ||
        head.y() < 0 || head.y() >= FIELD_HEIGHT) {
        gameOver();
        return;
    }

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

    // Рисуем фоновое изображение на всем виджете, кроме области игрового поля
    if (!backgroundImage.isNull()) {
        // Рисуем изображение на всем виджете
        painter.drawPixmap(rect(), backgroundImage);

        // Закрашиваем область игрового поля (чтобы изображение не было видно под ним)
        QRect frameRect = gameFrame->geometry();
        painter.fillRect(frameRect, QColor::fromRgb(0xF8F4E6));
    } else {
        // Если изображения нет, рисуем просто цветной фон
        painter.fillRect(rect(), QColor::fromRgb(BACKGROUND_COLOR));
    }

    // Рисуем игровое поле
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(gameFrame->geometry().topLeft() + QPoint(2, 2));

    // Фон игрового поля (без изображения)
    painter.fillRect(0, 0, FIELD_WIDTH * DOT_SIZE, FIELD_HEIGHT * DOT_SIZE,
                     QColor::fromRgb(0xF8F4E6));

    if (!snake.isEmpty()) {
        // Остальной код отрисовки змейки и еды остается без изменений
        // Сначала рисуем голову
        painter.setBrush(QColor::fromRgb(SNAKE_HEAD_COLOR));
        QPoint head = snake.first();
        int x = head.x() * DOT_SIZE;
        int y = head.y() * DOT_SIZE;

        QPoint points[3];

        switch (direction) {
        case 0: // Вверх
            points[0] = QPoint(x + DOT_SIZE / 2, y);
            points[1] = QPoint(x, y + DOT_SIZE);
            points[2] = QPoint(x + DOT_SIZE, y + DOT_SIZE);
            break;
        case 1: // Вправо
            points[0] = QPoint(x + DOT_SIZE, y + DOT_SIZE / 2);
            points[1] = QPoint(x, y);
            points[2] = QPoint(x, y + DOT_SIZE);
            break;
        case 2: // Вниз
            points[0] = QPoint(x + DOT_SIZE / 2, y + DOT_SIZE);
            points[1] = QPoint(x, y);
            points[2] = QPoint(x + DOT_SIZE, y);
            break;
        case 3: // Влево
            points[0] = QPoint(x, y + DOT_SIZE / 2);
            points[1] = QPoint(x + DOT_SIZE, y);
            points[2] = QPoint(x + DOT_SIZE, y + DOT_SIZE);
            break;
        }

        painter.drawPolygon(points, 3);

        // Теперь рисуем тело
        for (int i = 1; i < snake.size(); ++i) {
            painter.setBrush(QColor::fromRgb(i % 2 ? SNAKE_BODY_1 : SNAKE_BODY_2));
            QPoint p = snake[i];
            painter.drawRect(p.x() * DOT_SIZE, p.y() * DOT_SIZE, DOT_SIZE, DOT_SIZE);
        }
    }

    // Рисуем еду (ромб)
    painter.setBrush(QColor::fromRgb(FOOD_COLOR));
    int fx = food.x() * DOT_SIZE;
    int fy = food.y() * DOT_SIZE;

    QPoint foodPoints[4] = {
        QPoint(fx + DOT_SIZE / 2, fy),
        QPoint(fx + DOT_SIZE, fy + DOT_SIZE / 2),
        QPoint(fx + DOT_SIZE / 2, fy + DOT_SIZE),
        QPoint(fx, fy + DOT_SIZE / 2)
    };
    painter.drawPolygon(foodPoints, 4);
}
void SnakeGame::keyPressEvent(QKeyEvent *event)
{
    if (!gameRunning) {
        QWidget::keyPressEvent(event);
        return;
    }

    int newDirection = direction;

    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        if (direction != 2) newDirection = 0;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        if (direction != 3) newDirection = 1;
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        if (direction != 0) newDirection = 2;
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        if (direction != 1) newDirection = 3;
        break;
    default:
        QWidget::keyPressEvent(event);
        return;
    }

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
