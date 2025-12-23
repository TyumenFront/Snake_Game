#include <SFML/Graphics.hpp>
#include "singlepalyer.h"
#include <string>

// Конструктор одиночного режима, получает готовый объект змейки
singleplayer::singleplayer(Snake _snake) : snake(_snake)
{
    std::vector<std::pair<std::string, std::string>> settings;

    try {
        // Пытаемся прочитать настройки из файла/конфига
        settings = getSettings();
    }
    catch (...) {
        // Если не получилось — выставляем настройки по умолчанию и снова читаем
        setDefaultSettings();
        settings = getSettings();
    }

    // Берём текущий размер экрана пользователя
    float width = sf::VideoMode::getDesktopMode().width;
    float height = sf::VideoMode::getDesktopMode().height;

    // Инициализация границ игрового поля
    border.setSize(sf::Vector2f(
        width - 2 * stoi(settings[8].second) * CELL_SIZE, 
        height - 2 * stoi(settings[8].second) * CELL_SIZE)); // ширина и высота с учётом отступов от краёв
    border.setFillColor(sf::Color::Transparent);  // прямоугольник внутри прозрачный
    border.setOutlineColor(sf::Color::Yellow);      // контур границ - желтый
    border.setOutlineThickness(stoi(settings[8].second) * CELL_SIZE);              // толщина рамки в клетках

    // Сдвигаем прямоугольник так, чтобы вокруг него остались одинаковые отступы
    border.setPosition(stoi(settings[8].second) * CELL_SIZE, stoi(settings[8].second) * CELL_SIZE);

    // Загружаем шрифт для текста интерфейса
    if (!font.loadFromFile("../designe/font/menuFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
    }

    // Базовая настройка текста для отображения длины/очков
    lengthText.setFont(font);
    lengthText.setCharacterSize(20);
    lengthText.setFillColor(sf::Color::White);

    initFont();  // дополнительная инициализация шрифтов (если есть)
    collision = ""; // информации о столкновении пока нет
    _score = 0; // счет игрока в начале
};

// Запуск одиночной игры, возвращает код, по которому меню решит, что делать дальше
int singleplayer::startSingleplayer(sf::RenderWindow& window)
{
    // Ограничиваем частоту кадров (скорость игры)
    window.setFramerateLimit(10);

    std::vector<std::pair<std::string, std::string>> settings;

    // Загружаем настройки
    try {
        settings = getSettings();
    }
    // При ошибке — сбрасываем на умолчание
    catch (...) {
        setDefaultSettings();
        settings = getSettings();
    }

    // Включён ли игровой бот по настройкам
    if (settings[5].second == "ON") {
        botIsOn = true;
    }
    else {
        botIsOn = false;
    }

    int score = 0; // счет игрока
    int botScore = 0; // счет бота

    // Текст с никнеймом игрока
    sf::Text nickname;
    nickname.setFont(font);
    nickname.setCharacterSize(30);
    nickname.setFillColor(sf::Color::White);
    nickname.setPosition(860, 10 + stoi(settings[8].second) * CELL_SIZE);
    nickname.setString(settings[6].second); // берем ник из настроек

    // Пункты меню, которое показывается после окончания игры (Game Over)
    std::vector<sf::String> gameOverName = { "PLAY AGAIN", "BACK TO MENU" };
    gameMenu gameOverMenu(window, 950, 550, gameOverName, 100, 70);
    gameOverMenu.setColor(sf::Color::Red, sf::Color(220, 220, 220), sf::Color::Red);
    gameOverMenu.alignTextMenu(1); // Центруем текст в пунктах меню


    sf::Event event;

    // Инициализируем генератор случайных чисел (используется для яблок/стен и т.п.)
    srand(static_cast<unsigned>(time(nullptr)));
    int roundsCount = stoi(settings[4].second); // количество раундов из настроек

    // Цикл по раундам игры
    for (int roundNumber = 1; roundNumber <= roundsCount; ++roundNumber) {
        collision = ""; // сбрасываем последнее столкновение
        
        // Текст с номером раунда
        sf::Text round;
        round.setFont(font);
        round.setCharacterSize(30);
        round.setFillColor(sf::Color::White);
        round.setPosition(1560, 10 + stoi(settings[8].second) * CELL_SIZE);
        round.setString("Round: " + std::to_string(roundNumber) + '/' + std::to_string(roundsCount));

        deleteWalls(); // очистка стен с прошлого раунда
        snake.setDefaultSnake(); // возвращаем змейку игрока в начальное состояние
        
        // Если бот включён — тоже сбрасываем его позицию
        if (botIsOn) {
            bot.setDefaultSnake(sf::Vector2f(2 * WIDTH / 3 * CELL_SIZE, HEIGHT / 2 * CELL_SIZE));
        }

        // Добавление стен
        map();
        initTextures(); 
        createSprites();

        // Установка начальной скорости игры
        updateSpeed();

        // Меню паузы (отображается при Esc)
        std::vector<sf::String> gamePauseName = { "CONTINUE", "RESTART", "GO TO MENU" };
        gameMenu gamePauseMenu(window, 950, 550, gamePauseName, 100, 70);
        gamePauseMenu.setColor(sf::Color::Yellow, sf::Color(40, 40, 40), sf::Color::Yellow);
        gamePauseMenu.alignTextMenu(1);

        bool pause = false; // стоит ли игра на паузе

        // Главный цикл одного раунда
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close(); // закрытие окна по крестику
                }
            }

            // Проверка на нажатие Esc для постановки игры на паузу
            if (event.key.code == sf::Keyboard::Escape) {
                pause = true;

                // Пауза
                while (pause) {

                    while (window.pollEvent(event)) {

                        if (event.type == sf::Event::KeyReleased) {
                            // Перемещение выбора вверх в меню паузы
                            if (event.key.code == sf::Keyboard::Up) {
                                gamePauseMenu.moveUp();
                            }

                            // Перемещение выбора вниз
                            if (event.key.code == sf::Keyboard::Down) {
                                gamePauseMenu.moveDown();
                            }

                            // Подтверждение пункта (Enter)
                            if (event.key.code == sf::Keyboard::Enter) {
                                if (gamePauseMenu.getSelected() == 0) {

                                    // Выходим из паузы
                                    pause = false;
                                    break;
                                }

                                switch (gamePauseMenu.getSelected()) {
                                case 1: // перезапустить одиночную игру
                                    return 2;
                                    break;

                                case 2: // вернуться в главное меню
                                    return 0;
                                }
                            }
                        }
                    }

                    // Отрисовка меню паузы
                    window.clear();
                    gamePauseMenu.draw();
                    window.display();
                }
            }

            // Обработка действий игрока по нажатию клавиш
            if (sf::Event::KeyReleased) {

                auto key = event.key.scancode;

                // Передаём код клавиши в обработчик управления змейкой
                handleInput(window, key);
            }

            // Обновляем позицию яблока, чтобы оно не появлялось в стенах
            getApple().respawn(getWalls());

            // Если бот включён — пересчитываем его направление
            if (botIsOn) {
                bot.changeBotDirection(apple.getPosition(), walls);
            }

            // Движение змейки
            move();

            updateSpeed(); // Вызываем для начальной установки интервала обновления

            // Проверяем столкновения (стены, яблоки, змейки и т.п.)
            if (checkCollision()) {

                // Начисляем очки за длину игрока и бота
                score = score + getSnake().getBody().size() - 3;
                botScore = botScore + bot.getLength() - 3;

                // Дополнительные очки за конкретные типы столкновений
                if (collision == "playa" && botIsOn) {
                    score += 5;
                }
                else if (collision == "bot" && botIsOn) {
                    botScore += 5;
                }
                else if (collision == "playa kill" && botIsOn) {
                    score += 5;
                }
                else if (collision == "bot kill" && botIsOn) {
                    botScore += 5;
                }

                // раунд завершилс
                break;
            }

            // Спрайт с иконкой игрока (картинка змейки рядом с ником)
            sf::RectangleShape player;
            sf::Texture playerTexture;
            std::string playerTexturePath = "../designe/snake" + settings[9].second + ".png";
            if (!playerTexture.loadFromFile(playerTexturePath)) return -1;
            player.setSize(sf::Vector2f(30, 30));
            player.setPosition(sf::Vector2f(820, 10 + stoi(settings[8].second) * CELL_SIZE));
            player.setTexture(&playerTexture);

            // Основная отрисовка игрового кадра
            window.clear();
            draw(window, score); // рисуем поле, змейки, яблоки, стены и т.п.
            window.draw(player);
            window.draw(nickname);
            window.draw(round);

            window.display();
        }
    }


    // Этап после окончания всех раундов — экран Game Over
    while (true) {

        window.clear();

        // Определяем максимальный счёт и победителя (игрок / бот / никто)
        int winScore = (score > botScore) ? score : botScore;
        std::string winner = (score == winScore) ? ((botScore == winScore) ? "nobody" : settings[6].second) : "Bot";
        winner = (botIsOn) ? winner + " win!" : "Game Over";
        _score = winScore; // сохранение итогового счета

        // Отображение "Game Over" и длины змейки
        sf::Text gameOverText;
        gameOverText.setFont(getFont());
        gameOverText.setCharacterSize(40);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setString(winner);
        gameOverText.setPosition(WIDTH * CELL_SIZE / 2 - 100, HEIGHT * CELL_SIZE / 2 - 20);

        // Текст с финальными очками
        sf::Text lengthText;
        lengthText.setFont(getFont());
        lengthText.setCharacterSize(20);
        lengthText.setFillColor(sf::Color::White);
        lengthText.setString("Score: " + std::to_string(winScore));
        lengthText.setPosition(WIDTH * CELL_SIZE / 2 - 20, HEIGHT * CELL_SIZE / 2 + 40);

        // Обработка событий на экране Game Over
        while (window.pollEvent(event)) {
            // Закрыть игру по крестику или Esc
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape) {
                window.close();
                return EXIT_SUCCESS;
            }

            // Обрабатывает нажатие кнопки.
            if (event.type == sf::Event::KeyReleased) {

                if (event.key.code == sf::Keyboard::Up) {
                    gameOverMenu.moveUp(); // Переместить выбор вверх
                }

                if (event.key.code == sf::Keyboard::Down) {
                    gameOverMenu.moveDown(); // Переместить выбор вниз
                }

                if (event.key.code == sf::Keyboard::Enter) {
                    switch (gameOverMenu.getSelected()) {
                    case 0:
                        //играть снова (перезапуск одиночки)
                        return 2;

                    case 1:
                        // вернуться в меню
                        return 0;
                    }
                }
            }
        }

        // Рисуем текст и меню конца игры
        window.draw(gameOverText);
        window.draw(lengthText);
        gameOverMenu.draw();

        window.display();
    }
    return _score; // Возвращаем итоговый счёт (на случай, если while когда-нибудь закончится)
}


// Метод инициализации шрифта для отображения текста в игре
void singleplayer::initFont() {
    // Пытаемся загрузить шрифт из файла по указанному пути
    if (!font.loadFromFile("../designe/font/menuFont.ttf")) {
        // Если загрузка не удалась, выводим сообщение об ошибке в стандартный поток ошибок
        std::cerr << "Failed to load font." << std::endl;
    }
}

// Метод обновления скорости игры на основе текущей длины змейки
void singleplayer::updateSpeed() {
    // Вычисляем новый интервал обновления:
    // 1. Умножаем размер тела змейки на 0.05 (коэффициент замедления)
    // 2. Вычитаем результат из 1.0 (базовый интервал)
    // 3. Используем std::max для гарантии, что интервал не будет меньше 0.1 секунды
    updateInterval = std::max(0.1f, 1.0f - 0.05f * static_cast<float>(snake.getBody().size()));
}

// Метод проверки всех возможных столкновений в игре
bool singleplayer::checkCollision() {
    // Флаг результата проверки столкновений
    bool result = false;
    // Получаем ширину и высоту окна в пикселях
    float width = sf::VideoMode::getDesktopMode().width;
    float height = sf::VideoMode::getDesktopMode().height;

    // Вектор для хранения настроек игры
    std::vector<std::pair<std::string, std::string>> settings;

    // Блок try-catch для безопасного получения настроек
    try {
        // Пытаемся получить настройки из файла
        settings = getSettings();
    }
    catch (...) {
        // Если произошла ошибка, устанавливаем настройки по умолчанию
        setDefaultSettings();
        // И повторно получаем их
        settings = getSettings();
    }

    // Получаем позицию головы змейки игрока
    sf::Vector2f headPosition1 = snake.getHeadPosition();
    // Проверяем столкновение с границами игрового поля:
    // 1. Проверка выхода за левую границу (отступ от края)
    // 2. Проверка выхода за правую границу (ширина минус отступ)
    // 3. Проверка выхода за верхнюю границу
    // 4. Проверка выхода за нижнюю границу
    if (headPosition1.x < stoi(settings[8].second) * CELL_SIZE || headPosition1.x >= width - stoi(settings[8].second) * CELL_SIZE ||
        headPosition1.y < stoi(settings[8].second) * CELL_SIZE || headPosition1.y >= height - stoi(settings[8].second) * CELL_SIZE) {
        // Если столкновение с границами, фиксируем столкновение с ботом
        collision = "bot";
        // Устанавливаем флаг результата в true
        result = true;
    }

    // Проверяем, включен ли режим бота
    if (botIsOn) {
        // Получаем позицию головы бота
        sf::Vector2f headPosition2 = bot.getHeadPosition();
        // Проверяем столкновение бота с границами игрового поля
        if (headPosition2.x < stoi(settings[8].second) * CELL_SIZE || headPosition2.x >= width - stoi(settings[8].second) * CELL_SIZE ||
            headPosition2.y < stoi(settings[8].second) * CELL_SIZE || headPosition2.y >= height - stoi(settings[8].second) * CELL_SIZE) {
            // Определяем тип столкновения:
            // Если уже было столкновение игрока - "Nobody", иначе - "playa"
            collision = (!result) ? "playa" : "Nobody";
            // Устанавливаем флаг результата в true
            result = true;
        }

        // Проверяем столкновение голов змейки игрока и бота
        // Вычисляем расстояние между головами с помощью теоремы Пифагора
        float distance = std::hypot(headPosition1.x - headPosition2.x, headPosition1.y - headPosition2.y);
        // Минимальное расстояние для столкновения (в пикселях)
        float minDistance = 0;
        // Если расстояние меньше минимального, фиксируем столкновение
        if (distance <= minDistance) {
            collision = "Nobody";
            result = true;
        }

        // Проверяем столкновение бота со стенами
        // Проходим по всем стенам на игровом поле
        for (const auto& wall : walls) {
            // Если позиция головы бота совпадает с позицией стены
            if (headPosition2 == wall.getPosition()) {
                // Фиксируем столкновение игрока
                collision = "playa";
                result = true;
            }
        }

        // Проверяем столкновение головы игрока с телом бота
        if (bot.checkCollisionWithBody(headPosition1)) {
            collision = "bot kill";
            result = true;
        }

        // Проверяем столкновение бота с самим собой
        if (bot.checkCollisionWithMyself()) {
            collision = "playa";
            result = true;
        }

        // Проверяем столкновение головы бота с телом игрока
        if (snake.checkCollisionWithBody(headPosition2)) {
            collision = "playa kill";
            result = true;
        }
    }

    // Проверяем столкновение игрока со стенами
    // Проходим по всем стенам на игровом поле
    for (const auto& wall : walls) {
        // Если позиция головы игрока совпадает с позицией стены
        if (headPosition1 == wall.getPosition()) {
            collision = "bot";
            result = true;
        }
    }

    // Проверяем столкновение игрока с самим собой
    if (snake.checkCollisionWithMyself()) {
        collision = "bot";
        result = true;
    }

    // Возвращаем результат проверки столкновений
    return result;
}

// Метод генерации случайной карты стен
void singleplayer::map() {
    // Вектор для хранения настроек игры
    std::vector<std::pair<std::string, std::string>> settings;

    // Блок try-catch для безопасного получения настроек
    try {
        settings = getSettings();
    }
    catch (...) {
        setDefaultSettings();
        settings = getSettings();
    }

    // Генерация 25 стен (5x5)
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 5; i++) {
            // Генерация случайной позиции для стены:
            // 1. Случайная X-координата в пределах поля с учетом отступов
            // 2. Случайная Y-координата аналогично
            // 3. Переводим координаты в пиксели умножением на CELL_SIZE
            addWall(sf::Vector2f(
                static_cast<int>(rand() % (WIDTH - 2 * stoi(settings[8].second)) + stoi(settings[8].second)) * CELL_SIZE,
                static_cast<int>(rand() % (HEIGHT - 2 * stoi(settings[8].second)) + stoi(settings[8].second)) * CELL_SIZE
            ));
        }
    }
}

// Метод обработки движения в игре
void singleplayer::move() {
    // Получаем прошедшее время с последнего обновления в секундах
    float elapsedTime = clock.getElapsedTime().asSeconds();

    // Получаем голову змейки игрока и создаем новую голову в следующей позиции
    sf::RectangleShape newHead = snake.getBody().front();
    // Перемещаем новую голову в направлении движения змейки
    newHead.move(snake.getDirection());
    // Получаем позицию новой головы
    sf::Vector2f headPosition1 = newHead.getPosition();

    // Проверяем, совпадает ли позиция головы с позицией яблока
    if (headPosition1 == apple.getPosition()) {
        // Если совпадает, змейка "съедает" яблоко
        snake.eatApple();
        // Респавним яблоко на новой позиции
        apple.respawn({});
        // Перезапускаем таймер респавна яблока
        appleRespawnTimer.restart();
        // Обновляем спрайты для отображения изменений
        createSprites();
    }
    else {
        // Если яблоко не съедено, просто двигаем змейку
        snake.move();
    }

    // Если режим бота включен
    if (botIsOn) {
        // Получаем голову бота и создаем новую голову
        sf::RectangleShape newHead2 = bot.getBody().front();
        // Перемещаем голову бота в направлении его движения
        newHead2.move(bot.getDirection());
        // Получаем позицию головы бота
        sf::Vector2f headPosition2 = newHead2.getPosition();

        // Проверяем, съел ли бот яблоко
        if (headPosition2 == apple.getPosition()) {
            // Бот съедает яблоко
            bot.eatApple();
            // Респавним яблоко
            apple.respawn({});
            // Перезапускаем таймер респавна
            appleRespawnTimer.restart();
            // Обновляем спрайты
            createSprites();
        }
        else {
            // Если яблоко не съедено, двигаем бота
            bot.move();
        }
    }

    // Проверяем, прошло ли 5 секунд с момента последнего респавна яблока
    if (appleRespawnTimer.getElapsedTime().asSeconds() >= 5) {
        // Если прошло, респавним яблоко с учетом стен
        apple.respawn(getWalls());
        // Перезапускаем таймер
        appleRespawnTimer.restart();
    }

    // Проверяем, прошло ли достаточно времени для обновления скорости
    if (elapsedTime > updateInterval) {
        // Перезапускаем таймер
        clock.restart();
        // Обновляем скорость игры
        updateSpeed();
    }

    // Очищаем очередь спрайтов перед созданием новых
    bodySprites.clear();
    // Создаем новые спрайты для всех объектов
    createSprites();
}

// Метод отрисовки игровой сцены
void singleplayer::draw(sf::RenderWindow& window, int& score) {
    // Отрисовываем все спрайты (змейки, яблоки, стены)
    drawSprites(window);

    // Получаем настройки для позиционирования текста
    std::vector<std::pair<std::string, std::string>> settings;

    try {
        settings = getSettings();
    }
    catch (...) {
        setDefaultSettings();
        settings = getSettings();
    }

    // Устанавливаем текст для отображения счета:
    // Текущий счет + длина змейки - 3 (начальная длина)
    lengthText.setString("Score: " + std::to_string(score + this->getSnake().getBody().size() - 3));
    // Устанавливаем позицию текста с учетом отступа от границ
    lengthText.setPosition(10 + stoi(settings[8].second) * CELL_SIZE, 10 + stoi(settings[8].second) * CELL_SIZE);
    // Отрисовываем текст в окне
    window.draw(lengthText);

    // Отрисовываем границы игрового поля
    window.draw(border);
}

// Метод отрисовки всех спрайтов
void singleplayer::drawSprites(sf::RenderWindow& window) {
    // Отрисовываем фон игрового поля
    drawField(window);

    // Отрисовываем все стены
    for (const auto& wall : walls) {
        window.draw(wall.getSprite());
    }

    // Отрисовываем все спрайты из очереди (змейки, яблоки)
    for (const auto& sprite : bodySprites) {
        window.draw(sprite);
    }

    // Отрисовываем спрайт яблока
    window.draw(appleSprite);
}

// Метод инициализации текстур для всех игровых объектов
void singleplayer::initTextures() {
    // Получаем настройки для определения цветов объектов
    std::vector<std::pair<std::string, std::string>> settings;

    try {
        settings = getSettings();
    }
    catch (...) {
        setDefaultSettings();
        settings = getSettings();
    }

    // Загружаем текстуру фона игрового поля
    if (!fieldTexture.loadFromFile("../designe/field.png")) {
        // Если загрузка не удалась, выводим сообщение об ошибке
        std::cerr << "Failed to load field texture." << std::endl;
    }

    // Загружаем текстуру змейки игрока с учетом выбранного цвета
    if (!snakeTexture.loadFromFile("../designe/snake" + settings[9].second + ".png")) {
        std::cerr << "Failed to load snake texture." << std::endl;
    }

    // Загружаем текстуру яблока с учетом выбранного цвета
    if (!appleTexture.loadFromFile("../designe/apple" + settings[11].second + ".png")) {
        std::cerr << "Failed to load apple texture." << std::endl;
    }

    // Загружаем текстуру стен
    if (!wallTexture.loadFromFile("../designe/wall.png")) {
        std::cerr << "Failed to load wall texture." << std::endl;
    }

    // Загружаем текстуру бота с учетом выбранного цвета
    if (!botTexture.loadFromFile("../designe/bot" + settings[12].second + ".png")) {
        std::cerr << "Failed to load bots texture." << std::endl;
    }
}

// Метод создания спрайтов для всех игровых объектов
void singleplayer::createSprites() {
    // Создаем спрайт яблока
    appleSprite.setTexture(appleTexture);
    // Масштабируем спрайт под размер клетки
    appleSprite.setScale(CELL_SIZE / appleTexture.getSize().x, CELL_SIZE / appleTexture.getSize().y);
    // Устанавливаем позицию спрайта равной позиции яблока
    appleSprite.setPosition(apple.getPosition());

    // Создаем спрайты для каждого сегмента змейки игрока
    for (const auto& segment : snake.getBody()) {
        // Создаем новый спрайт с текстурой змейки
        sf::Sprite segmentSprite(snakeTexture);
        // Масштабируем под размер клетки
        segmentSprite.setScale(CELL_SIZE / snakeTexture.getSize().x, CELL_SIZE / snakeTexture.getSize().y);
        // Устанавливаем позицию равной позиции сегмента
        segmentSprite.setPosition(segment.getPosition());
        // Добавляем спрайт в очередь
        bodySprites.push_back(segmentSprite);
    }

    // Если бот включен, создаем спрайты для него
    if (botIsOn) {
        for (const auto& segment : bot.getBody()) {
            // Создаем спрайт бота
            sf::Sprite segmentSprite(botTexture);
            // Масштабируем под размер клетки
            segmentSprite.setScale(CELL_SIZE / botTexture.getSize().x, CELL_SIZE / botTexture.getSize().y);
            // Устанавливаем позицию
            segmentSprite.setPosition(segment.getPosition());
            // Добавляем в очередь
            bodySprites.push_back(segmentSprite);
        }
    }

    // Создаем спрайты для всех стен
    for (const auto& wall : walls) {
        // Создаем спрайт стены
        sf::Sprite wallSprite(wallTexture);
        // Масштабируем под размер клетки
        wallSprite.setScale(CELL_SIZE / wallTexture.getSize().x, CELL_SIZE / wallTexture.getSize().y);
        // Устанавливаем позицию
        wallSprite.setPosition(wall.getPosition());
        // Добавляем в очередь
        bodySprites.push_back(wallSprite);
    }
}

// Метод обработки ввода с клавиатуры
void singleplayer::handleInput(sf::RenderWindow& window, sf::Keyboard::Scancode key) {
    // Получаем настройки для определения управляющих клавиш
    std::vector<std::pair<std::string, std::string>> settings;

    try {
        settings = getSettings();
    }
    catch (...) {
        setDefaultSettings();
        settings = getSettings();
    }

    // Передаем ввод в объект змейки для обработки
    snake.handleInput(window, settings, key);
}

// Геттер для получения объекта змейки игрока
Snake singleplayer::getSnake() {
    // Возвращаем объект змейки
    return this->snake;
}

// Метод удаления всех стен с игрового поля
void singleplayer::deleteWalls() {
    // Очищаем вектор стен
    walls.clear();
}

// Метод отрисовки фона игрового поля
void singleplayer::drawField(sf::RenderWindow& window) {
    // Получаем размеры окна
    int width = sf::VideoMode::getDesktopMode().width;
    int height = sf::VideoMode::getDesktopMode().height;

    // Проходим по всей площади окна с шагом CELL_SIZE
    for (int i = 0; (i * CELL_SIZE) < width; ++i) {
        for (int j = 0; (j * CELL_SIZE) < height; ++j) {
            // Создаем спрайт для текущей клетки
            sf::Sprite cellSprite;
            // Устанавливаем текстуру поля
            cellSprite.setTexture(fieldTexture);
            // Устанавливаем масштаб (1:1)
            cellSprite.setScale(1, 1);
            // Устанавливаем позицию клетки
            cellSprite.setPosition(sf::Vector2f(i * CELL_SIZE, j * CELL_SIZE));
            // Отрисовываем клетку
            window.draw(cellSprite);
        }
    }
}