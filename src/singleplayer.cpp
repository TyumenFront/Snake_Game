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