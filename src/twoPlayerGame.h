#pragma once
#include <SFML/Graphics.hpp>
#include "snake.h"
#include "gameMenu.h"
#include <deque>
#include <fstream>
#include "serialize.h"
#include <string>

// Класс twoPlayaGame - реализует режим игры для двух игроков
class twoPlayaGame {
public:
    // Конструктор с параметрами имен игроков
    // Значения по умолчанию: "ChostPlaya1" и "LowTabPlaya2"
    twoPlayaGame(std::string _playa1name = "ChostPlaya1", std::string _playa2name = "LowTabPlaya2");

    // Основной метод запуска игры для двух игроков
    // Возвращает: результат игры (код завершения)
    int startTwoPlayaGame(sf::RenderWindow& window);

    // Метод обновления скорости игры на основе длины змеек
    void updateSpeed();

    // Геттер для получения объекта яблока
    Apple getApple() const {
        return apple;
    }

    // Геттер для получения объекта часов
    sf::Clock getClock() const {
        return clock;
    }

    // Геттер для получения интервала обновления игры
    float getUpdateInterval() const {
        return updateInterval;
    }

    // Геттер для получения объекта змейки первого игрока
    Snake getPlayer1() {
        return player1;
    }

    // Геттер для получения объекта змейки второго игрока
    Snake getPlayer2() {
        return player2;
    }

    // Публичные поля для доступа к змейкам (можно сделать private с геттерами)
    Snake player1;  // Змейка первого игрока
    Snake player2;  // Змейка второго игрока

    // Имена игроков
    std::string playa1name;  // Имя первого игрока
    std::string playa2name;  // Имя второго игрока

    // Строка для хранения информации о столкновении
    std::string collision;

protected:
    // Метод проверки всех столкновений в игре
    bool checkCollision();

    // Геттер для получения шрифта
    const sf::Font& getFont() const {
        return font;
    }

    // Геттер для получения списка стен (возвращает константную ссылку)
    const std::vector<Wall>& getWalls() const {
        return walls;
    }

    // Метод отрисовки игровой сцены с информацией о счете
    // Параметр playersScore: счет игроков в формате Vector2i (x - первый, y - второй)
    void draw(sf::RenderWindow& window, sf::Vector2i playersScore);

    // Метод обработки движения в игре
    void move();

    // Метод обработки ввода с клавиатуры для конкретной змейки
    // Параметр snake: ссылка на змейку, для которой обрабатывается ввод
    void handleInput(sf::RenderWindow& window, sf::Keyboard::Scancode key, Snake& snake);

    // Метод инициализации текстур для всех игровых объектов
    void initTextures();

    // Метод отрисовки всех спрайтов в окне
    void drawSprites(sf::RenderWindow& window);

    // Метод создания спрайтов для всех объектов на основе текущего состояния
    void createSprites();

    // Метод случайной смены текстуры яблока (забавное название)
    void appleSkinChangerFunction2023AkaMultyplySomeSkinsTogether();

    // Метод отрисовки фона игрового поля (клетчатой сетки)
    void drawField(sf::RenderWindow& window);

private:
    // Объект яблока на игровом поле
    Apple apple;

    // Объект шрифта для отображения текста в игре
    sf::Font font;

    // Текстовый объект для отображения длины змеек (счета игроков)
    sf::Text lengthText;

    // Объект часов для измерения времени между обновлениями
    sf::Clock clock;

    // Интервал обновления игровой логики (в секундах)
    float updateInterval;

    // Прямоугольная форма для отображения границ игрового поля
    sf::RectangleShape border;

    // Вспомогательный метод инициализации шрифта
    void initFont();

    // Вектор (список) стен на игровом поле (в этом режиме может быть пустым)
    std::vector<Wall> walls;

    // Таймер для отслеживания времени респавна яблока
    sf::Clock appleRespawnTimer;

    // Текстура фона игрового поля
    sf::Texture fieldTexture;

    // Текстура змейки первого игрока
    sf::Texture snake1Texture;

    // Текстура яблока
    sf::Texture appleTexture;

    // Текстура змейки второго игрока
    sf::Texture snake2Texture;

    // Спрайт яблока для отрисовки
    sf::Sprite appleSprite;

    // Двусторонняя очередь спрайтов для отрисовки всех сегментов змеек
    std::deque<sf::Sprite> bodySprites;

    // Общий счет игры (победный счет)
    int score;
};