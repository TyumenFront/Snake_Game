#include "snake.h"

// Возвращает текущую длину тела змейки
std::size_t Snake::getLength() const {
    return body.size();
}

// Конструктор по умолчанию — создает змейку со стандартными параметрами
Snake::Snake() {
    setDefaultSnake(); // тело и направление
}

// Конструктор, задающий позицию старта и клавиши управления
Snake::Snake(sf::Vector2f startPosition, const std::vector<std::pair<std::string, std::string>>& control)
    : _control(control) { // сохраняем управление
    
    // Создаем три сегмента — из них будет начальное тело змейки
    for (int i = 0; i < 3; ++i) {
        sf::RectangleShape segment(sf::Vector2f(CELL_SIZE, CELL_SIZE)); // каждый сегмент - квадрат
        segment.setPosition(startPosition); // устанавливаем позицию
        segment.setFillColor(sf::Color::Green); //задаем цвет тела змейки
        body.push_front(segment); // голова змейки спереди
    }

    // Изначально змейка движется вниз
    direction = sf::Vector2f(0, SPEED);
}

// Метод для создания “дефолтной” змейки в заданной позиции
void Snake::setDefaultSnake(sf::Vector2f pos)
{
    // Удаляем старое тело змейки, если оно было
    body.clear();

    // Создаем три сегмента тела
    for (int i = 0; i < 3; ++i) {
        sf::RectangleShape segment(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        segment.setPosition(pos); // все начинаются в одной точке
        segment.setFillColor(sf::Color::Green);
        body.push_front(segment); 
    }

    // Начальное направление движения - вниз
    direction = sf::Vector2f(0, SPEED);
}

// Конструктор копирования — создает точную копию другой змейки
Snake::Snake(const Snake& other) : body(other.body) {
    direction = other.direction;
}

// Метод увеличивает длину змейки, добавляя копию головного сегмента
void Snake::grow() {
    // дублируем голову, не двигая змейку
    body.push_front(body.front());
}

// Рисует змейку в игровом окне
void Snake::draw(sf::RenderWindow& window) {

    // каждый прямоугольник тела рисуется отдельно
    for (const auto& segment : body) {
        window.draw(segment);
    }

}

// Обработка нажатий клавиш для одной змейки (обычное управление)
void Snake::handleInput(sf::RenderWindow& window, std::vector<std::pair<std::string, std::string>> control, sf::Keyboard::Scancode key) {
    // Обработка нажатий клавиш
    //если нажата влево и змейка уже не движется по горизонтали
    if (sf::Keyboard::getDescription(key) == control[1].second && direction.x == 0) {
        direction = sf::Vector2f(-SPEED, 0); // движение влево
    }
    // если нажата клавиша "вправо"
    else if (sf::Keyboard::getDescription(key) == control[3].second && direction.x == 0) {
        direction = sf::Vector2f(SPEED, 0); // движение вправо
    }
    // если нажата клавиша "вверх"
    else if (sf::Keyboard::getDescription(key) == control[0].second && direction.y == 0) {
        direction = sf::Vector2f(0, -SPEED); // движение вверх
    }
    // если нажата клавиша "вниз"
    else if (sf::Keyboard::getDescription(key) == control[2].second && direction.y == 0) {
        direction = sf::Vector2f(0, SPEED); // движение вниз
    }
}

// Такой же метод, но использует _control, заданный в конструкторе (например, для второй змейки)
void Snake::handleInputTwo(sf::RenderWindow& window, sf::Keyboard::Scancode key) {
    // Обработка нажатий клавиш
    if (sf::Keyboard::getDescription(key) == _control[1].second && direction.x == 0) {
        direction = sf::Vector2f(-SPEED, 0);
    }
    else if (sf::Keyboard::getDescription(key) == _control[3].second && direction.x == 0) {
        direction = sf::Vector2f(SPEED, 0);
    }
    else if (sf::Keyboard::getDescription(key) == _control[0].second && direction.y == 0) {
        direction = sf::Vector2f(0, -SPEED);
    }
    else if (sf::Keyboard::getDescription(key) == _control[2].second && direction.y == 0) {
        direction = sf::Vector2f(0, SPEED);
    }
}

// Возвращает координаты головы змейки
sf::Vector2f Snake::getHeadPosition() const {
    return body.front().getPosition();
}

// Метод, если змейка съедает яблоко
void Snake::eatApple() {
    sf::RectangleShape newHead = body.front(); // копия текущей головы
    newHead.move(direction); // двигаем копию вперед в текущем направлении
    body.push_front(newHead); // добавляем ее как новую голову
};

// Движение змейки 
void Snake::move() {
    sf::RectangleShape newHead = body.front(); // копия текущей головы
    newHead.move(direction); // сдвигаем по текущему направлению

    // добавляем новую голову и удаляем последний сегмент, чтобы длина не менялась
    body.push_front(newHead);
    body.pop_back();
};

// Возвращает текущее тело змейки (для проверки столкновений)
std::deque<sf::RectangleShape> Snake::getBody() {
    return body;
}

// Возвращает текущее направление движения
sf::Vector2f Snake::getDirection() {
    return direction;
}

// Проверяет, не врезалась ли змейка сама в себя
bool Snake::checkCollisionWithMyself() {
    sf::Vector2f headPosition = getHeadPosition();

    // Сравниваем позицию головы с каждым сегментом тела
    for (std::size_t i = 1; i < body.size(); ++i) {
        if ((body[i].getPosition().x == headPosition.x) && (body[i].getPosition().y == headPosition.y)) {
            return true; // то есть столкновение есть
        }
    }

    return false; // столкновений нет
}

// Проверяет, находится ли заданная точка в теле змейки (кроме головы)
bool Snake::checkCollisionWithBody(const sf::Vector2f& position) const {
    
    for (std::size_t i = 1; i < body.size(); ++i) {
        if (body[i].getPosition() == position) {
            return true; // точка занята телом
        }
    }
    return false;
}

// Логика передвижения “бота” — змейки, которая сама ищет яблоко
void Snake::changeBotDirection(const sf::Vector2f& applePosition, const std::vector<Wall>& walls) {
    
    // Вычисляем вектор от головы змейки до яблока
    sf::Vector2f targetDirection = applePosition - getHeadPosition();
    sf::Vector2f newDirection;

    // Проверяем, находится ли яблоко внутри змейки
    if (checkCollisionWithBody(applePosition)) {
        // Если яблоко находится внутри змейки, просто оставляем текущее направление
        return;
    }

    // Определяем основное направление: по X или по Y в зависимости от положения яблока
    if (std::abs(targetDirection.x) > std::abs(targetDirection.y)) {
        newDirection = sf::Vector2f((targetDirection.x > 0) ? SPEED : -SPEED, 0);
    }
    else {
        newDirection = sf::Vector2f(0, (targetDirection.y > 0) ? SPEED : -SPEED);
    }

    // Если новое направление безопасно (нет столкновений с телом и стеной)
    if (!checkCollisionWithBody(getHeadPosition() + newDirection) && !checkCollisionWithWalls(getHeadPosition() + newDirection, walls)) {
        direction = newDirection; // меняем направление на новое
    }
    else {
        // Если путь заблокирован, пробуем повернуть в перпендикулярную сторону
        if (std::abs(targetDirection.x) > std::abs(targetDirection.y)) {
            newDirection = sf::Vector2f(0, (targetDirection.y > 0) ? SPEED : -SPEED);
        }
        else {
            newDirection = sf::Vector2f((targetDirection.x > 0) ? SPEED : -SPEED, 0);
        }

        // Проверяем, не приведет ли новое направление к столкновению с телом змейки
        // и стенами на карте
        if (!checkCollisionWithBody(getHeadPosition() + newDirection) && !checkCollisionWithWalls(getHeadPosition() + newDirection, walls)) {
            direction = newDirection;
        }
        // Если и это направление приводит к столкновению, оставляем текущее направление
    }
}

// Проверяет, столкнется ли змейка со стеной в заданной координате
bool Snake::checkCollisionWithWalls(const sf::Vector2f& position, const std::vector<Wall>& walls) const {
    // Перебираем все стены
    for (const auto& wall : walls) {
        // Если позиция совпадает с координатами стены — столкновение
        if (wall.getPosition().x == position.x && wall.getPosition().y == position.y)
            return true;
    }
    return false;  // столкновений нет
}
