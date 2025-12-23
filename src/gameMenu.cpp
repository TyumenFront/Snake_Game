#include "gameMenu.h"
#include <vector>

// Метод инициализации текстового элемента меню
void gameMenu::setInitText(sf::Text& mtext, sf::String str, float xpos, float ypos)
{
    // Устанавливаем шрифт для текстового элемента
    mtext.setFont(_font);
    // Устанавливаем строку текста
    mtext.setString(str);
    // Устанавливаем позицию текста на экране
    mtext.setPosition(xpos, ypos);
    // Устанавливаем цвет заполнения текста (для невыбранных элементов)
    mtext.setFillColor(_unselected_color);
    // Устанавливаем размер шрифта
    mtext.setCharacterSize(_size_font);
    // Устанавливаем толщину контура текста
    mtext.setOutlineThickness(3);
    // Устанавливаем цвет контура текста
    mtext.setOutlineColor(_border_color);
}

// Метод установки вертикальной позиции меню
void gameMenu::setPositionY(float ypos)
{
    // Присваиваем новое значение вертикальной координате меню
    this->_menu_y = ypos;
}

// Конструктор класса gameMenu с инициализацией списком
gameMenu::gameMenu(sf::RenderWindow& window, float menuX, float menuY, std::vector<sf::String> name, int menu_step, int font_size) :
    // Инициализация ссылки на окно
    _window(window),
    // Инициализация горизонтальной координаты меню
    _menu_x(menuX),
    // Инициализация вертикальной координаты меню
    _menu_y(menuY),
    // Инициализация шага между пунктами меню
    _menu_step(menu_step),
    // Инициализация размера шрифта
    _size_font(font_size)
{
    // Загружаем шрифт из файла
    // Если загрузка не удалась, завершаем программу с кодом 15
    if (!_font.loadFromFile("../designe/font/menuFont.ttf")) exit(15);

    // Устанавливаем элементы меню
    setMenu(name);
}

// Метод установки пунктов меню
void gameMenu::setMenu(std::vector<sf::String> name)
{
    // Освобождаем память от предыдущего массива меню (если был)
    delete[] _menu;

    // Устанавливаем максимальное количество элементов меню
    _max_menu = std::size(name);
    // Устанавливаем выбранный элемент на первый (индекс 0)
    _menu_selected = 0;
    // Выделяем память под массив текстовых элементов меню
    _menu = new sf::Text[std::size(name)];

    // Цикл инициализации всех элементов меню
    for (int i = 0; i < _max_menu; ++i) {
        // Устанавливаем текст и позицию для каждого элемента меню
        setInitText(_menu[i], name[i], _menu_x, _menu_y + i * _menu_step);
    }

    // Устанавливаем цвет выбранного элемента (первого по умолчанию)
    _menu[_menu_selected].setFillColor(_selected_color);
}

// Метод установки цветов меню
void gameMenu::setColor(sf::Color unselected_color, sf::Color selected_color, sf::Color border_color)
{
    // Устанавливаем цвет невыбранных элементов
    _unselected_color = unselected_color;
    // Устанавливаем цвет выбранного элемента
    _selected_color = selected_color;
    // Устанавливаем цвет границы текста
    _border_color = border_color;

    // Цикл обновления цветов всех элементов меню
    for (int i = 0; i < _max_menu; ++i) {
        // Устанавливаем цвет заполнения для каждого элемента
        this->_menu[i].setFillColor(_unselected_color);
        // Устанавливаем цвет контура для каждого элемента
        this->_menu[i].setOutlineColor(_border_color);
    }

    // Устанавливаем цвет для выбранного элемента (выделение)
    _menu[_menu_selected].setFillColor(_selected_color);
}

// Метод выравнивания текста меню по горизонтали
void gameMenu::alignTextMenu(int posX)
{
    // Переменная для смещения при выравнивании
    int nullx = 0;

    // Цикл выравнивания каждого элемента меню
    for (int i = 0; i < _max_menu; ++i) {
        // Выбор типа выравнивания
        switch (posX)
        {
        case 0:
            // Выравнивание по левому краю (без смещения)
            nullx = 0;
            break;
        case 1:
            // Выравнивание по центру (смещение на половину ширины)
            nullx = _menu[i].getLocalBounds().width / 2;
            break;
        case 2:
            // Выравнивание по правому краю (смещение на всю ширину)
            nullx = _menu[i].getLocalBounds().width;
            break;
        }

        // Устанавливаем новую позицию с учетом выравнивания
        // Вычитаем смещение из текущей X-координаты
        _menu[i].setPosition(_menu[i].getPosition().x - nullx, _menu[i].getPosition().y);
    }
}

// Метод перемещения выбора вверх по меню
void gameMenu::moveUp()
{
    // Проверяем, не является ли текущий элемент первым
    if (_menu_selected != 0) {
        // Уменьшаем индекс выбранного элемента
        --_menu_selected;
        // Сбрасываем цвет предыдущего элемента (стал невыбранным)
        _menu[_menu_selected + 1].setFillColor(_unselected_color);
        // Устанавливаем цвет нового выбранного элемента
        _menu[_menu_selected].setFillColor(_selected_color);
    }
    else {
        // Если это первый элемент, переходим к последнему
        // Сбрасываем цвет текущего элемента
        _menu[_menu_selected].setFillColor(_unselected_color);
        // Устанавливаем индекс на последний элемент
        _menu_selected = _max_menu - 1;
        // Устанавливаем цвет нового выбранного элемента
        _menu[_menu_selected].setFillColor(_selected_color);
    }
}

// Метод перемещения выбора вниз по меню
void gameMenu::moveDown()
{
    // Проверяем, не является ли текущий элемент последним
    if (_menu_selected != _max_menu - 1) {
        // Увеличиваем индекс выбранного элемента
        ++_menu_selected;
        // Сбрасываем цвет предыдущего элемента (стал невыбранным)
        _menu[_menu_selected - 1].setFillColor(_unselected_color);
        // Устанавливаем цвет нового выбранного элемента
        _menu[_menu_selected].setFillColor(_selected_color);
    }
    else {
        // Если это последний элемент, переходим к первому
        // Сбрасываем цвет текущего элемента
        _menu[_menu_selected].setFillColor(_unselected_color);
        // Устанавливаем индекс на первый элемент
        _menu_selected = 0;
        // Устанавливаем цвет нового выбранного элемента
        _menu[_menu_selected].setFillColor(_selected_color);
    }
}

// Метод отрисовки меню в окне
void gameMenu::draw()
{
    // Цикл отрисовки всех элементов меню
    for (int i = 0; i < _max_menu; ++i) {
        // Отрисовываем каждый текстовый элемент в окне
        _window.draw(_menu[i]);
    }
}

// Метод обработки нажатия кнопки (смена меню)
void gameMenu::pressButton(std::vector<sf::String> name, int mode, int align)
{
    // Устанавливаем новые элементы меню
    this->setMenu(name);
    // Устанавливаем новый режим меню
    this->setMode(mode);
    // Выравниваем текст меню согласно указанному способу
    this->alignTextMenu(align);
}