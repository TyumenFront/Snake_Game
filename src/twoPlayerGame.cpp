#include <SFML/Graphics.hpp>
#include "twoPlayerGame.h"
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