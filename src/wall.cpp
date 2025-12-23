#include "Wall.h"

// Конструктор стены: принимает позицию и сохраняет её в поле position
Wall::Wall(sf::Vector2f position) : position(position) {
    sf::Texture wallTexture; // текстура для стены
    
    // Загружаем текстуру из файла
    if (!wallTexture.loadFromFile("wall.png")) {
        std::cout << "Failed to load wall texture." << std::endl; // сообщение об ошибке
    }
    // Текстура спрайта стены, позиция спрайта в мире
    // графическое изображение для визуализации объектов
    sprite.setTexture(wallTexture);
    sprite.setPosition(position);
}

// Геттер позиции: возвращает ссылку на вектор с позицией стены
const sf::Vector2f& Wall::getPosition() const {
    return position;
}

// Геттер спрайта: возвращает константную ссылку на спрайт,
// чтобы можно было его рисовать, но не менять снаружи
const sf::Sprite& Wall::getSprite() const {
    return sprite;
}