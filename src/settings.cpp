#include "settings.h"

// Функция получения настроек из файла
std::vector<std::pair<std::string, std::string>> getSettings()
{
    // Создаем объект входного файлового потока для чтения файла настроек
    std::ifstream inpF("settings.ini");

    // Проверяем, удалось ли открыть файл настроек
    if (!inpF.is_open()) {
        // Если файл не удалось открыть, выбрасываем исключение с сообщением об ошибке
        throw "settings is not exist";
        // Возвращаем пустой вектор пар для обработки ошибки (хотя код после throw не выполнится)
        return std::vector<std::pair<std::string, std::string>>();
    }

    // Инициализируем пустую строку для хранения содержимого файла настроек
    std::string settingsStr = "";

    // Читаем файл построчно до достижения конца файла (end of file)
    while (!inpF.eof()) {
        // Создаем временную строку для хранения одной строки из файла
        std::string temp;
        // Читаем одно слово/строку из файла (оператор >> читает до пробела или конца строки)
        inpF >> temp;
        // Добавляем прочитанную строку в общую строку с добавлением символа новой строки
        settingsStr += temp + "\n";
    }

    // Вызываем функцию unserialize для преобразования строки в вектор пар "ключ-значение"
    auto settings = unserialize(settingsStr);

    // Возвращаем полученный вектор настроек вызывающему коду
    return settings;
}

// Функция сохранения настроек в файл
void setSettings(std::vector<std::pair<std::string, std::string>> data)
{
    // Создаем объект выходного файлового потока для записи в файл настроек
    // Файл будет создан, если не существует, или перезаписан, если существует
    std::ofstream outF("settings.ini");

    // Сериализуем вектор настроек в строку и записываем ее в файл
    outF << serialize(data);

    // Закрываем файловый поток для гарантии сохранения данных и освобождения ресурсов
    outF.close();
}

// Функция установки настроек по умолчанию
void setDefaultSettings()
{
    // Создаем вектор пар для хранения настроек по умолчанию
    std::vector<std::pair<std::string, std::string>> settings;

    // Добавляем настройки управления для первого игрока:
    // Клавиша W для движения вверх
    settings.push_back(std::pair("Up1", "W"));
    // Клавиша A для движения влево
    settings.push_back(std::pair("Left1", "A"));
    // Клавиша S для движения вниз
    settings.push_back(std::pair("Down1", "S"));
    // Клавиша D для движения вправо
    settings.push_back(std::pair("Right1", "D"));

    // Добавляем настройку количества раундов (по умолчанию 1)
    settings.push_back(std::pair("Rounds", "1"));
    // Добавляем настройку бота (по умолчанию выключен)
    settings.push_back(std::pair("Bot", "OFF"));

    // Добавляем настройки ников игроков
    settings.push_back(std::pair("Nickname1", "Player1"));
    settings.push_back(std::pair("Nickname2", "Player2"));

    // Добавляем настройку размера ячейки/отступа (по умолчанию 1)
    settings.push_back(std::pair("CellSize", "1"));

    // Добавляем настройки цветов объектов:
    // Цвет первого игрока (по умолчанию 1)
    settings.push_back(std::pair("Player1Color", "1"));
    // Цвет второго игрока (по умолчанию 1)
    settings.push_back(std::pair("Player2Color", "1"));
    // Цвет яблока (по умолчанию 1)
    settings.push_back(std::pair("AppleColor", "1"));
    // Цвет бота (по умолчанию 1)
    settings.push_back(std::pair("BotColor", "1"));

    // Добавляем настройки управления для второго игрока (стрелки клавиатуры):
    // Стрелка вверх для движения вверх
    settings.push_back(std::pair("Up2", "Up"));
    // Стрелка влево для движения влево
    settings.push_back(std::pair("Left2", "Left"));
    // Стрелка вниз для движения вниз
    settings.push_back(std::pair("Down2", "Down"));
    // Стрелка вправо для движения вправо
    settings.push_back(std::pair("Right2", "Right"));

    // Вызываем функцию setSettings для сохранения настроек по умолчанию в файл
    setSettings(settings);
}