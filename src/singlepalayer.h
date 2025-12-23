#pragma once
#include <SFML/Graphics.hpp>
#include "snake.h"
#include "gameMenu.h"
#include <deque>
#include <fstream>
#include "serialize.h"

// Класс singleplayer - реализует режим одиночной игры
class singleplayer {
public:
    // Конструктор, принимающий объект змейки
    singleplayer(Snake _snake);

    // Основной метод запуска одиночной игры
    // Возвращает: результат игры (код завершения)
    int startSingleplayer(sf::RenderWindow& window);

    // Метод обновления скорости игры на основе длины змейки
    void updateSpeed();

    // Геттер для получения объекта яблока
    Apple getApple() const {
        return apple;
    }

    // Геттер для получения объекта часов
    sf::Clock getClock() const {
        return clock;
    }

    // Геттер для получения интервала обновления
    float getUpdateInterval() const {
        return updateInterval;
    }

protected:
    // Метод проверки столкновений в игре
    bool checkCollision();

    // Геттер для получения шрифта
    const sf::Font& getFont() const {
        return font;
    }

    // Метод добавления стены на указанную позицию
    void addWall(sf::Vector2f position) {
        walls.emplace_back(position);
    }

    // Метод отрисовки всех стен в окне
    void drawWalls(sf::RenderWindow& window) {
        for (const auto& wall : walls) {
            window.draw(wall.getSprite());
        }
    }

    // Метод генерации карты (размещения стен)
    void map();

    // Геттер для получения списка стен
    const std::vector<Wall>& getWalls() const {
        return walls;
    }

    // Метод отрисовки игровой сцены
    // Параметр score: текущий счет игрока
    void draw(sf::RenderWindow& window, int& score);

    // Метод обработки движения в игре
    void move();

    // Метод обработки ввода с клавиатуры
    // Параметр key: код нажатой клавиши
    void handleInput(sf::RenderWindow& window, sf::Keyboard::Scancode key);

    // Метод получения текущего счета (длины змейки)
    int score() { return snake.getLength(); };

    // Метод инициализации текстур игры
    void initTextures();

    // Метод отрисовки спрайтов в окне
    void drawSprites(sf::RenderWindow& window);

    // Метод создания спрайтов для всех объектов
    void createSprites();

    // Геттер для получения объекта змейки игрока
    Snake getSnake();

    // Метод удаления всех стен
    void deleteWalls();

    // Метод отрисовки игрового поля (фона)
    void drawField(sf::RenderWindow& window);

private:
    // Объект змейки игрока
    Snake snake;

    // Объект змейки бота (если включен)
    Snake bot;

    // Флаг, указывающий включен ли режим бота
    bool botIsOn;

    // Объект яблока на игровом поле
    Apple apple;

    // Текущий счет игрока
    int _score;

    // Объект шрифта для текста в игре
    sf::Font font;

    // Текстовый объект для отображения количества клеток змейки
    sf::Text lengthText;

    // Объект часов для измерения времени между обновлениями
    sf::Clock clock;

    // Интервал обновления игровой логики (в секундах)
    float updateInterval;

    // Прямоугольная форма границ игрового поля
    sf::RectangleShape border;

    // Вспомогательный метод инициализации шрифта
    void initFont();

    // Вектор (список) стен на игровом поле
    std::vector<Wall> walls;

    // Таймер для отслеживания времени респавна яблока
    sf::Clock appleRespawnTimer;

    // Текстура игрового поля (фон)
    sf::Texture fieldTexture;

    // Текстура змейки игрока
    sf::Texture snakeTexture;

    // Текстура яблока
    sf::Texture appleTexture;

    // Текстура стен
    sf::Texture wallTexture;

    // Спрайт яблока для отрисовки
    sf::Sprite appleSprite;

    // Двусторонняя очередь спрайтов для отрисовки всех объектов
    std::deque<sf::Sprite> bodySprites;

    // Текстура бота (змейки противника)
    sf::Texture botTexture;

    // Строка для хранения информации о типе столкновения
    std::string collision;
};