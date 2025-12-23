#pragma once
#pragma once
#include <iostream>

// Скорость змейки
const float SPEED = CELL_SIZE;

// Snake описывает игровую змейку :
// её поведение, внешний вид, управление и взаимодействие с другими объектами (яблоками, стенами и т.д.)
class Snake {
public:

    // Конструктор по умолчанию — создаёт змейку со стандартными параметрами и стартовой позицией
    Snake();

    // Конструктор, позволяющий задать стартовую позицию и управление (вектор пар: направление и клавиша)
    Snake(sf::Vector2f startPosition, const std::vector<std::pair<std::string, std::string>>& control);
    
    // Конструктор копирования — создаёт новую змейку, копируя состояние другой
    Snake(const Snake& other);
    
    // Устанавливает "дефолтную" змейку в центре поля (или в произвольной позиции pos)
    void setDefaultSnake(sf::Vector2f pos = sf::Vector2f(WIDTH / 2 * CELL_SIZE, HEIGHT / 2 * CELL_SIZE + CELL_SIZE));
    
    // Вызывается, когда змейка съедает яблоко
    void grow();
    
    // Обработка управления для одной змейки (использует переданный вектор control)
    void handleInput(sf::RenderWindow& window, std::vector<std::pair<std::string, std::string>> control, sf::Keyboard::Scancode key);
    
    // Альтернативная версия управления, использующая внутреннее поле _control
    void handleInputTwo(sf::RenderWindow& window, sf::Keyboard::Scancode key);
    
    // Возвращает позицию головы змейки (для проверки столкновений и т.д.)
    sf::Vector2f getHeadPosition() const;
    
    // Отрисовывает тело змейки на экране
    void draw(sf::RenderWindow& window);
    
    // Возвращает текущее количество сегментов змейки
    std::size_t getLength() const;
    
    // Метод вызывается, когда змейка съедает яблоко
    void eatApple();
    
    // Передвигает змейку на один шаг в выбранном направлении
    void move();
    
    // Проверяет, не столкнулась ли змейка сама с собой (например, при развороте или длинном теле)
    bool checkCollisionWithMyself();
    
    // Возвращает тело змейки (используется для визуализации или проверок)
    std::deque<sf::RectangleShape> getBody();
    
    // Возвращает текущее направление движения змейки в виде вектора
    sf::Vector2f getDirection();
    
    // Проверяет, занята ли указанная позиция телом змейки (кроме головы)
    bool checkCollisionWithBody(const sf::Vector2f& position) const;
    
    // Логика передвижения "бота" — выбирает направление движения в сторону яблока,
    // избегая столкновений с телом и стенами
    void changeBotDirection(const sf::Vector2f& applePosition, const std::vector<Wall>& walls);
    
    // Проверяет, не столкнётся ли змейка со стеной, если переместится в указанную позицию
    bool checkCollisionWithWalls(const sf::Vector2f& position, const std::vector<Wall>& walls) const;

private:
    // Контейнер, хранящий все сегменты тела змейки
    std::deque<sf::RectangleShape> body;

    // Текущее направление движения (вектор со значением по оси X и Y)
    sf::Vector2f direction;
    
    // Вектор, содержащий схему управления змейкой
    std::vector<std::pair<std::string, std::string>> _control;
};