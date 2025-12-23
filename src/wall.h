#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "settings.h"
#include <stdlib.h>

// Размер одной ячейки
const int CELL_SIZE = 30.0f;

class Wall {
public:
    // Конструктор: создаёт объект стены в заданной позиции на поле
    Wall(sf::Vector2f position);

    // Геттер позиции: возвращает ссылку на вектор с координатами стены
    const sf::Vector2f& getPosition() const;
    
    // Геттер спрайта: возвращает константную ссылку на спрайт,
    // чтобы его можно было рисовать, но не менять снаружи класса
    const sf::Sprite& getSprite() const;

private:
    // Позиция стены в игровой области
    sf::Vector2f position;
    // Графическое представление стены
    sf::Sprite sprite;
};
