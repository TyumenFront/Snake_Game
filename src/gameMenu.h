#pragma once
#include <SFML/Graphics.hpp>

class gameMenu {

	public:

		/**
		*	Конструктор.
		* 
		*	@param sf::RenderWindow& window		Обрабатываемое окно.
		* 	@param float menuX					X-овая позиция кнопок.
		* 	@param float menuY					Y-овая позиция кнопок.
		* 	@param std::vector<sf::String> name Содержимое всех кнопок меню.
		* 	@param int menu_step				Отступ между кнопками.
		* 	@param int font_size				Размер шрифта.
		*/
		gameMenu(sf::RenderWindow& window, float menuX, float menuY, std::vector<sf::String> name, int menu_step = 80, int font_size = 60);

		/// Деструктор.
		~gameMenu()
		{
			//delete[] _menu;
		};

		/**
		*	Отрисовка меню.
		* 
		*	@return void
		*/
		void draw();

		/**
		*	Выбор вышестоящей кнопки.
		*
		*	@return void
		*/
		void moveUp();

		/**
		*	Выбор нижестоящей кнопки.
		*
		*	@return void
		*/
		void moveDown();

		/**
		*	Установка цвета меню.
		*
		*	@param sf::Color unselected_color Цвет невыбранной кнопки.
		* 	@param sf::Color selected_color	  Цвет выбранной кнопки.
		* 	@param sf::Color border_color	  Цвет контура.
		* 
		*	@return void
		*/
		void setColor(sf::Color unselected_color, sf::Color selected_color, sf::Color border_color);

		/**
		*	Выравнивание меню.
		* 
		*	@param integer posX Тип выравнивания по горизонтали.
		* 
		*	@return void
		*/
		void alignTextMenu(int posX);

		/**
		*	Возвращает номер выбранного пункта меню.
		* 
		*	@return integer
		*/
		int getSelected()
		{
			return _menu_selected;
		};

		/**
		*	Устанавливает кнопки меню.
		* 
		*	@param std::vector<sf::String> name Содержимое кнопок.
		* 
		*	@return void
		*/
		void setMenu(std::vector<sf::String> name);

		/**
		*	Устанавливает режим меню.
		* 
		*	@param integer mode Режим меню.
		* 
		*	@return void
		*/
		void setMode(int mode)
		{
			_menuMode = mode;
		};

		/**
		*	Возвращает режим меню.
		*
		*	@return integer
		*/
		int getMode()
		{
			return _menuMode;
		};

		/**
		*	Меняет содержимое меню.
		* 
		*	@param std::vector<sf::String> name Содержимое кнопок.
		*	@param int mode				        Режим меню.
		*	@param int align					Тип выравнивания.
		* 
		*	@return void
		*/
		void pressButton(std::vector<sf::String> name, int mode, int align = 1);

		void setPositionY(float ypos);

	private:
		float _menu_x;		// X-овая координата меню.
		float _menu_y;		// Y-овая координата меню.
		int _menu_step;		// Расстояние между пунктами меню.
		int _max_menu;		// Максимальное количество элементов меню.
		int _size_font;		// Размер шрифта.
		int _menu_selected;	// Выбранный элемент меню.
		int _menuMode = 0;  // Режим в котором находится меню.

		sf::Font _font;		// Шрифт меню.
		sf::Text* _menu;		// Массив элементов меню.

		sf::Color _unselected_color = sf::Color(40, 40, 40);	// Цвет не выбранного элемента меню.
		sf::Color _selected_color = sf::Color::Yellow;		// Цвет выбранного элемента меню.
		sf::Color _border_color = sf::Color::Yellow;			// Цвет окантовки шрифта.

		/**
		*	Устанавливает стиль кнопок.
		* 
		*	@param sf::Text& mtext	Результирующая кнопка.
		* 	@param sf::String str	Содержимое кнопки.
		* 	@param float xpos		X-овая координата кнопки.
		*  	@param float ypos		Y-овая координата кнопки.
		* 
		*	@return void
		*/
		void setInitText(sf::Text& mtext, sf::String str, float xpos, float ypos);

		sf::RenderWindow& _window; // Ссылка на графическое окно.
};
