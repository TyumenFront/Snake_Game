#include <SFML/Graphics.hpp>
#include "twoPlayaGame.h"
#include <iostream>
#include <stdlib.h> // нужен для вызова функций rand(), srand()
#include <time.h> // нужен для вызова функции time()

// Конструктор класса twoPlayaGame — создаёт игру для двух игроков
twoPlayaGame::twoPlayaGame(std::string _playa1name, std::string _playa2name) : playa1name(_playa1name), playa2name(_playa2name)
{
    // Загружаем настройки игры (клавиши управления, количество раундов и т.д.)
    std::vector<std::pair<std::string, std::string>> settings;

    try {
        settings = getSettings(); // пытаемся прочитать настройки из файла
    }
    catch (...) { // если файл не найден — используем настройки по умолчанию
        setDefaultSettings();
        settings = getSettings();// если файл не найден — используем настройки по умолчанию
    }

    // Создаём первую змейку слева на поле (1/3 ширины экрана)
    // settings[0-3] — клавиши WASD для первого игрока
    player1 = Snake(sf::Vector2f(WIDTH / 3 * CELL_SIZE, HEIGHT / 2 * CELL_SIZE), { settings[0], settings[1], settings[2], settings[3] });
    
    // Создаём вторую змейку справа на поле (2/3 ширины экрана)
    player2 = Snake(sf::Vector2f(2 * WIDTH / 3 * CELL_SIZE, HEIGHT / 2 * CELL_SIZE), { settings[13], settings[14], settings[15], settings[16] });
    // settings[13-16] — клавиши стрелок для второго игрока

    collision = ""; // столкновения


    // Получаем размеры экрана пользователя (полноэкранный режим)
    float width = sf::VideoMode::getDesktopMode().width;
    float height = sf::VideoMode::getDesktopMode().height;

    // Создаём жёлтую рамку-границу игрового поля
    border.setSize(sf::Vector2f(width - 2 * stoi(settings[8].second) * CELL_SIZE, height - 2 * stoi(settings[8].second) * CELL_SIZE));
    border.setFillColor(sf::Color::Transparent);  // прозрачный фон
    border.setOutlineColor(sf::Color::Yellow);      // желтая граница
    border.setOutlineThickness(stoi(settings[8].second) * CELL_SIZE); // толщина из настроек              // Толщина границ

    // Позиционируем рамку с отступом от краёв экрана
    border.setPosition(stoi(settings[8].second) * CELL_SIZE, stoi(settings[8].second) * CELL_SIZE);


    // Загружаем шрифт для текста (счёт, раунды, меню)
    if (!font.loadFromFile("../designe/font/menuFont.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
    }

    // Настраиваем объект текста для отображения длины змейки
    lengthText.setFont(font);
    lengthText.setCharacterSize(20);
    lengthText.setFillColor(sf::Color::White);

    initFont();  // дополнительная инициализация шрифтов 
    score = 0;
};


// Основной метод запуска игры для двух игроков
int twoPlayaGame::startTwoPlayaGame(sf::RenderWindow& window)
{

    // Ограничиваем частоту кадров до 10 FPS — змейка движется медленно и предсказуемо
    window.setFramerateLimit(10);

    // Инициализация генератора случайных чисел
    srand(static_cast<unsigned>(time(nullptr)));

    // Загружаем все текстуры игры (фон, змейки, яблоко)
    initTextures();

    // Создаём спрайты (визуальные объекты) на основе текстур
    createSprites();

    // Устанавливаем начальную скорость обновления игры
    updateSpeed();

    // Создаём меню паузы с тремя пунктами
    std::vector<sf::String> gamePauseName = { "CONTINUE", "RESTART", "GO TO MENU" };
    gameMenu gamePauseMenu(window, 950, 550, gamePauseName, 100, 70);
    
    // Настраиваем цвета меню паузы: жёлтый для выделения, тёмный фон
    gamePauseMenu.setColor(sf::Color::Yellow, sf::Color(40, 40, 40), sf::Color::Yellow);
    gamePauseMenu.alignTextMenu(1); // выравнивание

    bool pause = false; // пауза

    // // Загружаем настройки повторно (на случай, если они изменились)
    std::vector<std::pair<std::string, std::string>> settings;

    try {
        settings = getSettings();
    }
    catch (...) {
        setDefaultSettings();
        settings = getSettings();
    }

    // Количество раундов берём из настроек (например, settings[4].second = "5")
    auto roundsCount = std::stoi(settings[4].second);

    // Счётчики очков для каждого игрока
    int score1 = 0;
    int score2 = 0;

    // Создаём меню конца игры
    std::vector<sf::String> gameOverName = { "PLAY AGAIN", "BACK TO MENU" };
    gameMenu gameOverMenu(window, 950, 550, gameOverName, 100, 70);
    gameOverMenu.setColor(sf::Color::Red, sf::Color(220, 220, 220), sf::Color::Red);
    gameOverMenu.alignTextMenu(1);

    // Объект для обработки событий SFML (нажатия клавиш, закрытие окна)
    sf::Event event;

    // === ОСНОВНОЙ ЦИКЛ ПО РАУНДАМ ===
    for (int roundNumber = 1; roundNumber <= roundsCount; ++roundNumber) {
        collision = "";

        // Создаём текст текущего раунда
        sf::Text round;
        round.setFont(font);
        round.setCharacterSize(30);
        round.setFillColor(sf::Color::White);
        round.setPosition(1560, 10 + stoi(settings[8].second) * CELL_SIZE);
        round.setString("Round: " + std::to_string(roundNumber) + '/' + std::to_string(roundsCount));

        // Перезагружаем змейки в стартовые позиции
        player1.setDefaultSnake(sf::Vector2f(WIDTH / 3 * CELL_SIZE, HEIGHT / 2 * CELL_SIZE));
        player2.setDefaultSnake(sf::Vector2f(2 * WIDTH / 3 * CELL_SIZE, HEIGHT / 2 * CELL_SIZE));


        // === ЦИКЛ ОДНОГО РАУНДА ===
        while (window.isOpen()) {
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                if (event.type == sf::Event::KeyPressed) {
                    auto key = event.key.scancode;

                    // Обрабатываем ввод для обеих змейк
                    handleInput(window, key, player1);
                    handleInput(window, key, player2);
                }
            }

            if (event.key.code == sf::Keyboard::Escape) {
                pause = true;

                // Цикл меню паузы
                while (pause) {

                    while (window.pollEvent(event)) {

                        if (event.type == sf::Event::KeyReleased) {
                            // Выбор нижестоящей кнопки.
                            if (event.key.code == sf::Keyboard::Up) {
                                gamePauseMenu.moveUp();
                            }

                            // Выбор нижестоящей кнопки.
                            if (event.key.code == sf::Keyboard::Down) {
                                gamePauseMenu.moveDown();
                            }

                            if (event.key.code == sf::Keyboard::Enter) {
                                if (gamePauseMenu.getSelected() == 0) {
                                    pause = false; // CONTINUE
                                    break;
                                }

                                switch (gamePauseMenu.getSelected()) {
                                case 1:
                                    return 2;
                                    break; // RESTART

                                case 2:
                                    return 0; // MENU
                                }
                            }
                        }
                    }

                    window.clear();
                    gamePauseMenu.draw();
                    window.display();
                }
            }

            // Перемещаем яблоко, если оно на стене
            getApple().respawn(getWalls());

            // Двигаем обе змейки
            move();

            // Обновляем скорость игры
            updateSpeed(); 


            // Проверяем столкновения (змейки со стенами, друг с другом, собой)
            if (checkCollision()) {

                // Начисляем очки по длине змейки (минус 3 стартовых сегмента)
                score1 = score1 + getPlayer1().getBody().size() - 3;
                score2 = score2 + getPlayer2().getBody().size() - 3;

                // Бонусы за тип столкновения
                if (collision == playa1name) {
                    score1 += 5;
                }
                else if (collision == playa2name) {
                    score2 += 5;
                }
                else if (collision == playa1name + " kill") {
                    score1 += 10;
                }
                else if (collision == playa2name + " kill") {
                    score2 += 10;
                }
                break;
            }

            // Отрисовка кадра
            window.clear();
            draw(window, sf::Vector2i(score1, score2));
            window.draw(round);
            window.display();
        }
    }

    // === ЭКРАН КОНЦА ИГРЫ (после всех раундов) ===
    while (true) {
        window.clear();

        // Определяем победителя и его счёт
        int winScore = (score1 > score2) ? score1 : score2;
        std::string winner = (score1 == winScore) ? ((score2 == winScore) ? "nobody" : playa1name) : playa2name;
        score = winScore;

        sf::RectangleShape winnerCol; // иконка победителя

       

        std::string winnerColor = (score1 > score2) ? settings[9].second : settings[10].second;

        sf::Texture winnerTexture;
        std::string winnerTexturePath = "../designe/snake" + winnerColor + ".png";
        
        if (!winnerTexture.loadFromFile(winnerTexturePath)) return -1; // ошибка загрузки
        winnerCol.setSize(sf::Vector2f(30, 30));
        winnerCol.setPosition(sf::Vector2f(730, 440));
        winnerCol.setTexture(&winnerTexture);
        
        // скрываем иконку при ничьей
        if (score1 == score2) {
            winnerCol.setSize(sf::Vector2f(0, 0));
        }

        // Текст победителя
        sf::Text gameOverText;
        gameOverText.setFont(getFont());
        gameOverText.setCharacterSize(40);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setString(winner + " win!");
        gameOverText.setPosition(WIDTH * CELL_SIZE / 2 - 100, HEIGHT * CELL_SIZE / 2 - 20);

        // Текст финального счёта
        sf::Text lengthText;
        lengthText.setFont(getFont());
        lengthText.setCharacterSize(20);
        lengthText.setFillColor(sf::Color::White);
        lengthText.setString("Score: " + std::to_string(winScore));
        lengthText.setPosition(WIDTH * CELL_SIZE / 2 - 20, HEIGHT * CELL_SIZE / 2 + 40);

        // Обработка событий экрана Game Over
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape) {
                window.close();
                return EXIT_SUCCESS;
            }

            // Обрабатывает нажатие кнопки.
            if (event.type == sf::Event::KeyReleased) {

                // Выбор нижестоящей кнопки.
                if (event.key.code == sf::Keyboard::Up) {
                    gameOverMenu.moveUp();
                }

                // Выбор нижестоящей кнопки.
                if (event.key.code == sf::Keyboard::Down) {
                    gameOverMenu.moveDown();
                }

                if (event.key.code == sf::Keyboard::Enter) {
                    switch (gameOverMenu.getSelected()) {
                    case 0:
                        //startSingleplayer(window);
                        return 2;

                    case 1:
                        //std::vector<sf::String> name = { "START", "SETTINGS", "ABOUT", "EXIT" };
                        //menu.pressButton(name, 0);
                        return 0;
                    }
                }
            }
        }

        // Отрисовка финального экрана
        window.draw(gameOverText);
        window.draw(lengthText);
        window.draw(winnerCol);
        gameOverMenu.draw();

        window.display(); // возвращаем финальный счёт
    }


    return score;
}


// Метод инициализации шрифта для отображения текста в игре для двух игроков
void twoPlayaGame::initFont() {
    // Пытаемся загрузить шрифт из файла по указанному пути
    if (!font.loadFromFile("../designe/font/menuFont.ttf")) {
        // Если загрузка не удалась, выводим сообщение об ошибке в стандартный поток ошибок
        std::cerr << "Failed to load font." << std::endl;
    }
}

// Метод обновления скорости игры на основе длины змейки первого игрока
void twoPlayaGame::updateSpeed() {
    // Вычисляем новый интервал обновления:
    // 1. Умножаем размер тела змейки первого игрока на 0.05 (коэффициент замедления)
    // 2. Вычитаем результат из 1.0 (базовый интервал)
    // 3. Используем std::max для гарантии, что интервал не будет меньше 0.1 секунды
    updateInterval = std::max(0.1f, 1.0f - 0.05f * static_cast<float>(player1.getBody().size()));
}

// Метод проверки всех возможных столкновений в игре для двух игроков
bool twoPlayaGame::checkCollision() {
    // Флаг результата проверки столкновений, инициализированный значением false
    bool result = false;

    // Вектор для хранения настроек игры
    std::vector<std::pair<std::string, std::string>> settings;

    // Блок try-catch для безопасного получения настроек из файла
    try {
        // Пытаемся получить настройки из файла с помощью функции getSettings()
        settings = getSettings();
    }
    catch (...) {
        // Если произошла ошибка при получении настроек, устанавливаем настройки по умолчанию
        setDefaultSettings();
        // И повторно получаем настройки после установки значений по умолчанию
        settings = getSettings();
    }

    // Получаем ширину и высоту окна в пикселях для проверки границ
    float width = sf::VideoMode::getDesktopMode().width;
    float height = sf::VideoMode::getDesktopMode().height;

    // Проверка столкновения первого игрока с границами игрового поля
    // Получаем текущую позицию головы змейки первого игрока
    sf::Vector2f headPosition1 = player1.getHeadPosition();
    // Проверяем все четыре границы игрового поля с учетом отступа (settings[8].second)
    if (headPosition1.x < stoi(settings[8].second) * CELL_SIZE || headPosition1.x >= width - stoi(settings[8].second) * CELL_SIZE ||
        headPosition1.y < stoi(settings[8].second) * CELL_SIZE || headPosition1.y >= height - stoi(settings[8].second) * CELL_SIZE) {
        // Если столкновение произошло, устанавливаем победителем второго игрока
        collision = playa2name;
        // Устанавливаем флаг результата в true
        result = true;
    }

    // Проверка столкновения второго игрока с границами игрового поля
    // Получаем текущую позицию головы змейки второго игрока
    sf::Vector2f headPosition2 = player2.getHeadPosition();
    // Проверяем все четыре границы игрового поля с учетом отступа
    if (headPosition2.x < stoi(settings[8].second) * CELL_SIZE || headPosition2.x >= width - stoi(settings[8].second) * CELL_SIZE ||
        headPosition2.y < stoi(settings[8].second) * CELL_SIZE || headPosition2.y >= height - stoi(settings[8].second) * CELL_SIZE) {
        // Определяем победителя: если первый игрок уже проиграл, то "Nobody", иначе - первый игрок
        collision = (!result) ? playa1name : "Nobody";
        // Устанавливаем флаг результата в true
        result = true;
    }

    // Проверка столкновения голов двух змеек друг с другом
    // Вычисляем расстояние между головами двух змеек с помощью теоремы Пифагора
    float distance = std::hypot(headPosition1.x - headPosition2.x, headPosition1.y - headPosition2.y);
    // Минимальное расстояние для столкновения голов (в пикселях), в текущей реализации равно 0
    float minDistance = 0;
    // Если расстояние меньше или равно минимальному, фиксируем столкновение
    if (distance <= minDistance) {
        // Устанавливаем результат столкновения как "Nobody" (ничья)
        collision = "Nobody";
        // Устанавливаем флаг результата в true
        result = true;
    }

    // Проверка столкновения головы первого игрока с телом второго игрока
    if (player2.checkCollisionWithBody(headPosition1)) {
        // Если голова первого игрока столкнулась с телом второго, это "kill"
        collision = playa2name + " kill";
        // Устанавливаем флаг результата в true
        result = true;
    }

    // Проверка столкновения головы второго игрока с телом первого игрока
    if (player1.checkCollisionWithBody(headPosition2)) {
        // Если голова второго игрока столкнулась с телом первого, это "kill"
        collision = playa1name + " kill";
        // Устанавливаем флаг результата в true
        result = true;
    }

    // Проверка столкновения первого игрока с самим собой
    if (player1.checkCollisionWithMyself()) {
        // Если первый игрок столкнулся сам с собой, побеждает второй игрок
        collision = playa2name;
        // Устанавливаем флаг результата в true
        result = true;
    }

    // Проверка столкновения второго игрока с самим собой
    if (player2.checkCollisionWithMyself()) {
        // Если второй игрок столкнулся сам с собой, побеждает первый игрок
        collision = playa1name;
        // Устанавливаем флаг результата в true
        result = true;
    }

    // Возвращаем итоговый результат проверки столкновений
    return result;
}

// Метод случайной смены текстуры яблока (с забавным названием)
void twoPlayaGame::appleSkinChangerFunction2023AkaMultyplySomeSkinsTogether() {
    // Инициализируем генератор случайных чисел текущим временем
    srand(time(NULL));
    // Генерируем случайное число от 1 до 4 включительно для выбора текстуры яблока
    int appleSkinNum = 1 + rand() % (4 - 1 + 1);
    // Пытаемся загрузить текстуру яблока с выбранным номером
    if (!appleTexture.loadFromFile("../designe/apple" + std::to_string(appleSkinNum) + ".png")) {
        // Если загрузка не удалась, выводим сообщение об ошибке
        std::cerr << "Failed to load apple texture." << std::endl;
    }
}

// Метод обработки движения в игре для двух игроков
void twoPlayaGame::move() {
    // Получаем прошедшее время с последнего обновления в секундах
    float elapsedTime = clock.getElapsedTime().asSeconds();

    // Обработка движения первого игрока
    // Получаем голову змейки первого игрока
    sf::RectangleShape newHead = player1.getBody().front();
    // Перемещаем голову в направлении движения змейки
    newHead.move(player1.getDirection());
    // Получаем позицию новой головы первого игрока
    sf::Vector2f headPosition = newHead.getPosition();

    // Обработка движения второго игрока
    // Получаем голову змейки второго игрока
    sf::RectangleShape newHead2 = player2.getBody().front();
    // Перемещаем голову в направлении движения змейки
    newHead2.move(player2.getDirection());
    // Получаем позицию новой головы второго игрока
    sf::Vector2f headPosition2 = newHead2.getPosition();

    // Проверка на поедание яблока первым игроком
    if (headPosition == apple.getPosition()) {
        // Первый игрок съедает яблоко
        player1.eatApple();
        // Меняем текстуру яблока на случайную
        appleSkinChangerFunction2023AkaMultyplySomeSkinsTogether();
        // Респавним яблоко на новой позиции
        apple.respawn({});
        // Перезапускаем таймер респавна яблока
        appleRespawnTimer.restart();
        // Обновляем спрайты для отображения изменений
        createSprites();
    }
    // Проверка на поедание яблока вторым игроком
    else if (headPosition2 == apple.getPosition()) {
        // Второй игрок съедает яблоко
        player2.eatApple();
        // Меняем текстуру яблока на случайную
        appleSkinChangerFunction2023AkaMultyplySomeSkinsTogether();
        // Респавним яблоко на новой позиции
        apple.respawn({});
        // Перезапускаем таймер респавна яблока
        appleRespawnTimer.restart();
        // Обновляем спрайты для отображения изменений
        createSprites();
    }
    else {
        // Если никто не съел яблоко, просто двигаем обе змейки
        player1.move();
        player2.move();
    }

    // Проверка времени с момента последнего респавна яблока
    if (appleRespawnTimer.getElapsedTime().asSeconds() >= 5) {
        // Если прошло 5 секунд, меняем текстуру яблока
        appleSkinChangerFunction2023AkaMultyplySomeSkinsTogether();
        // Респавним яблоко с учетом стен (если они есть)
        apple.respawn(getWalls());
        // Перезапускаем таймер респавна
        appleRespawnTimer.restart();
    }

    // Проверяем, прошло ли достаточно времени для обновления скорости
    if (elapsedTime > updateInterval) {
        // Перезапускаем таймер
        clock.restart();
        // Обновляем скорость игры на основе текущей длины змейки
        updateSpeed();
    }

    // Очищаем очередь спрайтов перед созданием новых
    bodySprites.clear();
    // Создаем новые спрайты для всех объектов
    createSprites();
}

// Метод отрисовки игровой сцены для двух игроков
void twoPlayaGame::draw(sf::RenderWindow& window, sf::Vector2i playersScore) {
    // Отрисовываем все спрайты (змейки, яблоки)
    drawSprites(window);

    // Получаем настройки для позиционирования текста и элементов интерфейса
    std::vector<std::pair<std::string, std::string>> settings;

    try {
        settings = getSettings();
    }
    catch (...) {
        setDefaultSettings();
        settings = getSettings();
    }

    // Отображение счета первого игрока
    // Формируем строку: имя игрока + текущий счет (накопленный + длина змейки - 3)
    lengthText.setString(playa1name + ":" + std::to_string(playersScore.x + getPlayer1().getBody().size() - 3));
    // Устанавливаем позицию текста с учетом отступа от границ
    lengthText.setPosition(50 + stoi(settings[8].second) * CELL_SIZE, 10 + stoi(settings[8].second) * CELL_SIZE);
    // Отрисовываем текст первого игрока в окне
    window.draw(lengthText);

    // Отображение счета второго игрока
    // Формируем строку: имя игрока + текущий счет
    lengthText.setString(playa2name + ":" + std::to_string(playersScore.y + getPlayer2().getBody().size() - 3));
    // Устанавливаем позицию текста ниже, чем у первого игрока
    lengthText.setPosition(50 + stoi(settings[8].second) * CELL_SIZE, 10 + (stoi(settings[8].second) + 1) * CELL_SIZE);
    // Отрисовываем текст второго игрока в окне
    window.draw(lengthText);

    // Создание и отрисовка графического индикатора для первого игрока
    sf::RectangleShape player1Indicator;
    sf::Texture player1Texture;
    // Формируем путь к текстуре змейки первого игрока с учетом выбранного цвета
    std::string player1TexturePath = "../designe/snake" + settings[9].second + ".png";
    // Пытаемся загрузить текстуру
    if (!player1Texture.loadFromFile(player1TexturePath)) return; // Выходим, если не удалось загрузить
    // Устанавливаем размер индикатора
    player1Indicator.setSize(sf::Vector2f(30, 30));
    // Устанавливаем позицию индикатора слева от текста счета
    player1Indicator.setPosition(sf::Vector2f(10 + stoi(settings[8].second) * CELL_SIZE, 10 + stoi(settings[8].second) * CELL_SIZE));
    // Устанавливаем текстуру для индикатора
    player1Indicator.setTexture(&player1Texture);
    // Отрисовываем индикатор первого игрока
    window.draw(player1Indicator);

    // Создание и отрисовка графического индикатора для второго игрока
    sf::RectangleShape player2Indicator;
    sf::Texture player2Texture;
    // Формируем путь к текстуре змейки второго игрока с учетом выбранного цвета
    std::string player2TexturePath = "../designe/snake" + settings[10].second + ".png";
    // Пытаемся загрузить текстуру
    if (!player2Texture.loadFromFile(player2TexturePath)) return; // Выходим, если не удалось загрузить
    // Устанавливаем размер индикатора
    player2Indicator.setSize(sf::Vector2f(30, 30));
    // Устанавливаем позицию индикатора слева от текста счета (ниже первого индикатора)
    player2Indicator.setPosition(sf::Vector2f(10 + stoi(settings[8].second) * CELL_SIZE, 10 + (stoi(settings[8].second) + 1) * CELL_SIZE));
    // Устанавливаем текстуру для индикатора
    player2Indicator.setTexture(&player2Texture);
    // Отрисовываем индикатор второго игрока
    window.draw(player2Indicator);

    // Отрисовываем границы игрового поля
    window.draw(border);
}

// Метод отрисовки всех спрайтов в окне
void twoPlayaGame::drawSprites(sf::RenderWindow& window) {
    // Отрисовываем фон игрового поля (клетчатую сетку)
    drawField(window);

    // Отрисовываем все спрайты из очереди (сегменты змеек)
    for (const auto& sprite : bodySprites) {
        window.draw(sprite);
    }

    // Отрисовываем спрайт яблока
    window.draw(appleSprite);
}

// Метод инициализации текстур для всех игровых объектов
void twoPlayaGame::initTextures() {
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

    // Загружаем текстуру змейки первого игрока с учетом выбранного цвета
    if (!snake1Texture.loadFromFile("../designe/snake" + settings[9].second + ".png")) {
        std::cerr << "Failed to load snake texture." << std::endl;
    }

    // Загружаем текстуру змейки второго игрока с учетом выбранного цвета
    if (!snake2Texture.loadFromFile("../designe/snake" + settings[10].second + ".png")) {
        std::cerr << "Failed to load snake texture." << std::endl;
    }

    // Загружаем текстуру яблока с учетом выбранного цвета из настроек
    if (!appleTexture.loadFromFile("../designe/apple" + settings[11].second + ".png")) {
        std::cerr << "Failed to load apple texture." << std::endl;
    }

    // Вызываем метод для случайной смены текстуры яблока при инициализации
    appleSkinChangerFunction2023AkaMultyplySomeSkinsTogether();
}

// Метод создания спрайтов для всех игровых объектов
void twoPlayaGame::createSprites() {
    // Создаем спрайт яблока
    appleSprite.setTexture(appleTexture);
    // Масштабируем спрайт под размер клетки игрового поля
    appleSprite.setScale(CELL_SIZE / appleTexture.getSize().x, CELL_SIZE / appleTexture.getSize().y);
    // Устанавливаем позицию спрайта равной позиции яблока
    appleSprite.setPosition(apple.getPosition());

    // Создаем спрайты для каждого сегмента змейки первого игрока
    for (const auto& segment : player1.getBody()) {
        // Создаем новый спрайт с текстурой змейки первого игрока
        sf::Sprite segmentSprite(snake1Texture);
        // Масштабируем спрайт под размер клетки
        segmentSprite.setScale(CELL_SIZE / snake1Texture.getSize().x, CELL_SIZE / snake1Texture.getSize().y);
        // Устанавливаем позицию спрайта равной позиции сегмента змейки
        segmentSprite.setPosition(segment.getPosition());
        // Добавляем созданный спрайт в очередь для отрисовки
        bodySprites.push_back(segmentSprite);
    }

    // Создаем спрайты для каждого сегмента змейки второго игрока
    for (const auto& segment : player2.getBody()) {
        // Создаем новый спрайт с текстурой змейки второго игрока
        sf::Sprite segmentSprite(snake2Texture);
        // Масштабируем спрайт под размер клетки
        segmentSprite.setScale(CELL_SIZE / snake2Texture.getSize().x, CELL_SIZE / snake2Texture.getSize().y);
        // Устанавливаем позицию спрайта равной позиции сегмента змейки
        segmentSprite.setPosition(segment.getPosition());
        // Добавляем созданный спрайт в очередь для отрисовки
        bodySprites.push_back(segmentSprite);
    }
}

// Метод обработки ввода с клавиатуры для конкретной змейки
void twoPlayaGame::handleInput(sf::RenderWindow& window, sf::Keyboard::Scancode key, Snake& snake) {
    // Делегируем обработку ввода самому объекту змейки
    // Метод handleInputTwo использует внутренние настройки управления змейки
    snake.handleInputTwo(window, key);
}

// Метод отрисовки фона игрового поля (клетчатой сетки)
void twoPlayaGame::drawField(sf::RenderWindow& window) {
    // Получаем размеры окна в пикселях
    int width = sf::VideoMode::getDesktopMode().width;
    int height = sf::VideoMode::getDesktopMode().height;

    // Проходим по всей площади окна с шагом CELL_SIZE (размер клетки)
    for (int i = 0; (i * CELL_SIZE) < width; ++i) {
        for (int j = 0; (j * CELL_SIZE) < height; ++j) {
            // Создаем спрайт для текущей клетки поля
            sf::Sprite cellSprite;
            // Устанавливаем текстуру поля для спрайта
            cellSprite.setTexture(fieldTexture);
            // Устанавливаем масштаб спрайта (1:1, без изменения размера)
            cellSprite.setScale(1, 1);
            // Устанавливаем позицию спрайта на основе индексов i и j
            cellSprite.setPosition(sf::Vector2f(i * CELL_SIZE, j * CELL_SIZE));
            // Отрисовываем клетку поля в окне
            window.draw(cellSprite);
        }
    }
}