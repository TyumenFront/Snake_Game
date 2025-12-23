// Подключаем заголовочный файл библиотеки SFML для работы с графикой
#include <SFML/Graphics.hpp>

// Подключаем заголовочный файл класса Apple для реализации его методов
#include "apple.h"

// Конструктор класса Apple
Apple::Apple() {
    // Вызываем метод respawn с пустым списком стен для начального размещения яблока
    respawn({});
}

// Метод получения текущей позиции яблока
sf::Vector2f Apple::getPosition() const {
    // Возвращаем значение приватного поля position
    return position;
}

// Метод переразмещения (респавна) яблока на игровом поле
void Apple::respawn(const std::vector<Wall>& walls) {
    // Объявляем вектор для хранения настроек игры
    std::vector<std::pair<std::string, std::string>> settings;

    // Блок try-catch для обработки возможных ошибок при получении настроек
    try {
        // Пытаемся получить настройки из файла
        settings = getSettings();
    }
    catch (...) {
        // Если произошла ошибка, устанавливаем настройки по умолчанию
        setDefaultSettings();
        // И повторно получаем настройки
        settings = getSettings();
    }

    // Генерируем случайную координату X для яблока:
    // 1. rand() % (WIDTH - 2 * stoi(settings[8].second)) - случайное число от 0 до (ширина - 2*отступ)
    // 2. + stoi(settings[8].second) - добавляем отступ от края
    // 3. * CELL_SIZE - переводим из клеток в пиксели
    position.x = static_cast<int>(rand() % (WIDTH - 2 * stoi(settings[8].second)) + stoi(settings[8].second)) * CELL_SIZE;

    // Генерируем случайную координату Y для яблока аналогичным образом
    position.y = static_cast<int>(rand() % (HEIGHT - 2 * stoi(settings[8].second)) + stoi(settings[8].second)) * CELL_SIZE;

    // Проверяем, не оказалось ли яблоко на стене
    if (isOnWall(walls)) {
        // Если яблоко на стене, рекурсивно вызываем respawn снова
        respawn(walls);
    }
}

// Метод проверки, находится ли яблоко на позиции стены
bool Apple::isOnWall(const std::vector<Wall>& walls) const {
    // Проходим по всем стенам в переданном списке
    for (const auto& wall : walls) {
        // Сравниваем координаты яблока и стены, приведенные к целым числам
        if (static_cast<int>(position.x) == static_cast<int>(wall.getPosition().x)
            && static_cast<int>(position.y) == static_cast<int>(wall.getPosition().y)) {
            // Если координаты совпадают, возвращаем true
            return true;
        }
    }
    // Если ни одна стена не совпала с позицией яблока, возвращаем false
    return false;
}