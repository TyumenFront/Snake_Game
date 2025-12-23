#include <SFML/Graphics.hpp>
#include "gameMenu.h"
#include "singleplayer.h"
#include <iostream>
#include <string>
#include "settings.h"
#include "twoPlayaGame.h"

/**
*   Инициализирует название игры в меню.
*
*   @param sf::Text& mtext           Результирующий текст.
*   @param float xpos                X-овая позиция текста.
*   @param float ypos                Y-овая позиция текста.
*   @param sf::String str            Содержимое текста.
*   @param int size_font             Размер шрифта.
*   @param sf::Color menu_text_color Цвет текста.
*   @param int bord                  Толщина контура.
*   @param sf::Color border_color    Цвет контура.
*
*   @return void
*/
void InitText(sf::Text& mtext, float xpos, float ypos, sf::String str, int size_font = 60,
	sf::Color menu_text_color = sf::Color::Black, int bord = 0, sf::Color border_color = sf::Color::White); // Функция настройки текста: позиция, шрифт, цвет, контур (параметры по умолчанию)

// Устанавливает позицию и текст для элементов управления/настроек
void setControl(sf::Text& key, sf::String keyName, float ypos);

// Запускает одиночку и двойную игру, возвращает код завершения
int gameSingleplayerStart(sf::RenderWindow& window, singleplayer game);
int gameMultyplayerStart(sf::RenderWindow& window, twoPlayaGame& game);

int main()
{
	// Создаем окно игры на весь экран с разрешением рабочего стола
	sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "SNAKES", sf::Style::Fullscreen);

	// Ограничиваем FPS до 60 для стабильности
	window.setFramerateLimit(60);

	// Загружаем настройки игры из файла, если ошибка — используем по умолчанию
	std::vector<std::pair<std::string, std::string>> settings;

	try { // Читаем настройки (раунды, боты, цвета, ники и т.д.)
		settings = getSettings();
	}
	catch (...) {
		// Если файл настроек повреждён или отсутствует
		setDefaultSettings();
		settings = getSettings(); // Устанавливаем настройки по умолчанию и перечитываем их
	}

	// Делаем курсор невидимым — управление только клавиатурой
	window.setMouseCursorVisible(false);

	// Получаем размеры экрана для адаптивного интерфейса
	float width = sf::VideoMode::getDesktopMode().width;
	float height = sf::VideoMode::getDesktopMode().height;

	// Создаём фон меню размером с экран
	sf::RectangleShape background(sf::Vector2f(width, height));
	sf::Texture texture_window;
	if (!texture_window.loadFromFile("../designe/background_menu2.jpg")) return 4;
	
	// Если текстура фона не загрузилась — аварийный выход с кодом 4
	background.setTexture(&texture_window);

	// Загружаем шрифт для всего текста меню
	sf::Font font;
	if (!font.loadFromFile("../designe/font/menuFont.ttf")) return 5;
	// Если шрифт не загрузился — аварийный выход с кодом 5


	// Создаём статичные тексты заголовков (название игры + разработчики)
	sf::Text titul;
	titul.setFont(font);
	InitText(titul, 580, 50, "SNAKES", 150, sf::Color(40, 40, 40), 3, sf::Color::Yellow);

	// Устанавливает заголовок DEVELOPERS
	sf::Text developers;
	developers.setFont(font);
	InitText(developers, 580, 350, "DEVELOPERS:", 90, sf::Color(40, 40, 40), 3, sf::Color::Yellow);

	// Устанавливает заголовок Имя руководителя
	sf::Text owner;
	owner.setFont(font);
	InitText(owner, 580, 450, "LUBICKIY R. E.", 90, sf::Color(40, 40, 40), 3, sf::Color::Yellow);

	// Устанавливает заголовок Имя разработчика 1
	sf::Text developer;
	developer.setFont(font);
	InitText(developer, 580, 550, "NIKOLAEVA A.A.", 90, sf::Color(40, 40, 40), 3, sf::Color::Yellow);

	// Устанавливает заголовок Имя разработчика 2
	sf::Text kanye;
	kanye.setFont(font);
	InitText(kanye, 580, 650, "KALAEV A.V.", 90, sf::Color(40, 40, 40), 3, sf::Color::Yellow);

	// Инициализируем главное меню с 4 кнопками
	std::vector<sf::String> name = { "START", "SETTINGS", "ABOUT", "EXIT" };
	gameMenu gameMenu_(window, 950, 400, name, 130, 100);
	gameMenu_.alignTextMenu(1); // Выравниваем текст в меню (режим 1)

	// Определяем списки кнопок для подменю (будут подставляться динамически)
	std::vector<sf::String> startGameName = { "SINGLEPLAYER", "MULTIPLAYER", "GO BACK" };
	std::vector<sf::String> settingsGameName = { "NICKNAME", "CONTROL", "THEME", "FIELD SIZE", "SET DEFAULT SETTINGS", "GO BACK" };
	std::vector<sf::String> aboutGameName = { "GO BACK" };

	// Меню управления клавишами для одного игрока
	std::vector<sf::String> playerControlName = { "UP", "LEFT", "DOWN", "RIGHT", "SAVE", "GO BACK" };
	gameMenu playerControlMenu(window, 950, 350, playerControlName, 100, 70);
	playerControlMenu.alignTextMenu(2);

	// Меню выбора управления (игрок 1/игрок 2)
	std::vector<sf::String> controlName = { "PLAYER1", "PLAYER2", "GO BACK" };
	gameMenu controlMenu(window, 950, 350, controlName, 100, 70);
	controlMenu.alignTextMenu(2);

	// Подменю одиночной игры (настройка раундов + ботов)
	std::vector<sf::String> singleplayerName = { "ROUNDS", "BOTS", "PLAY", "GO BACK" };
	gameMenu singleplayerMenu(window, 950, 350, singleplayerName, 100, 70);
	singleplayerMenu.alignTextMenu(2);

	// Подменю многопользовательской игры
	std::vector<sf::String> multyplayerName = { "ROUNDS", "PLAY", "GO BACK" };
	gameMenu multyplayerMenu(window, 950, 450, multyplayerName, 100, 70);
	multyplayerMenu.alignTextMenu(2);

	// Меню ввода ников игроков
	std::vector<sf::String> NicknamesName = { "PLAYER1", "PLAYER2", "SAVE", "GO BACK" };
	gameMenu NicknamesMenu(window, 950, 350, NicknamesName, 100, 70);
	NicknamesMenu.alignTextMenu(2);

	// Меню выбора размера игрового поля
	std::vector<sf::String> fieldName = { "BIG", "MEDIUM", "SMALL", "GO BACK" };
	gameMenu fieldMenu(window, 950, 350, fieldName, 100, 70);
	fieldMenu.alignTextMenu(1);

	// Меню выбора тем/цветов
	std::vector<sf::String> themsName = { "PLAYER1", "PLAYER2", "APPLE", "BOT", "SAVE", "GO BACK" };
	gameMenu themsMenu(window, 950, 350, themsName, 100, 70);
	themsMenu.alignTextMenu(2);

	// Тексты для диалога выхода из игры
	std::vector<sf::String> exitGameName = { "GO BACK", "EXIT" };

	sf::Text exit1;
	exit1.setFont(font);
	InitText(exit1, 380, 280, "DO YOU WANT TO", 100, sf::Color(40, 40, 40), 3, sf::Color::Yellow);
	sf::Text exit2;
	exit2.setFont(font);
	InitText(exit2, 380, 400, "LEAVE THE GAME?", 100, sf::Color(40, 40, 40), 3, sf::Color::Yellow);

	// Переменные для динамических текстов (ники, клавиши, настройки)

	sf::Text nickname1;
	sf::Text nickname2;
	sf::Text keyUp;
	sf::Text keyLeft;
	sf::Text keyDown;
	sf::Text keyRight;
	
	// количество раундов и статус бота
	sf::Text keyRoundsCount;
	sf::Text keyBot;

	sf::String keyName;

	bool bot = false;

	// Списки возможных значений для настроек (циклический выбор)
	std::vector<std::string> botSettings;
	botSettings.push_back("OFF");
	botSettings.push_back("ON");
	auto botIsSetIt = std::find(botSettings.begin(), botSettings.end(), settings[5].second);

	// 9 цветов для игроков
	std::vector<std::string> playerColorSettings;
	playerColorSettings.push_back("1");
	playerColorSettings.push_back("2");
	playerColorSettings.push_back("3");
	playerColorSettings.push_back("4");
	playerColorSettings.push_back("5");
	playerColorSettings.push_back("6");
	playerColorSettings.push_back("7");
	playerColorSettings.push_back("8");
	playerColorSettings.push_back("9");

	std::vector<std::string> appleColorSettings;
	appleColorSettings.push_back("1");
	appleColorSettings.push_back("2");
	appleColorSettings.push_back("3");
	appleColorSettings.push_back("4");

	std::vector<std::string> botColorSettings;
	botColorSettings.push_back("1");
	botColorSettings.push_back("2");
	botColorSettings.push_back("3");
	botColorSettings.push_back("4");

	// Флаги состояний меню
	bool isSettings = false;
	bool about = false;

	// Текущие цвета из настроек (индексы в массивах цветов)
	auto botColor = settings[12].second;
	auto appleColor = settings[11].second;
	auto player2Color = settings[10].second;
	auto player1Color = settings[9].second;

	// === ОСНОВНОЙ ЦИКЛ ИГРЫ ===
	while (window.isOpen())
	{
		sf::Event event; // Объект для обработки событий SFML

		// Создаём объект многопользовательской игры (каждую итерацию — баг!)
		twoPlayaGame multyGame(settings[6].second, settings[7].second);

		// Обработка всех событий текущего кадра
		while (window.pollEvent(event))
		{
			// Закрытие окна (крестик или Escape)
			if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
				window.close();

			// Обработка отпускания клавиш (навигация + выбор)
			if (event.type == sf::Event::KeyReleased) {

				// Навигация: стрелка вверх — вверх по меню, вниз — вниз по меню
				if (event.key.code == sf::Keyboard::Up) {
					gameMenu_.moveUp(); // курсор вверх
				}

				// Выбор нижестоящей кнопки.
				if (event.key.code == sf::Keyboard::Down) {
					gameMenu_.moveDown(); //курсор вниз
				}

				// === ОБРАБОТКА РЕЖИМОВ МЕНЮ ПО GetMode() ===
				switch (gameMenu_.getMode()) {

					// РЕЖИМ 0: ГЛАВНОЕ МЕНЮ (START/SETTINGS/ABOUT/EXIT)
				case 0:

					// Enter активирует выбранную кнопку
					if (event.key.code != sf::Keyboard::Enter) {
						break;
					}

					switch (gameMenu_.getSelected()) {
						//getSelected() = номер выделенной кнопки
					case 0: // START - подменю SINGLEPLAYER/MULTIPLAYER
						gameMenu_.setPositionY(450);
						gameMenu_.pressButton(startGameName, 1);
						// Переключаем на режим 1
						break;
					case 1: // SETTINGS - подменю настроек
						gameMenu_.setPositionY(250);
						gameMenu_.pressButton(settingsGameName, 2);
						break;
					case 2: // ABOUT - экран разработчиков
						about = true;
						gameMenu_.setPositionY(750);
						gameMenu_.pressButton(aboutGameName, 3);
						break;
					case 3: // EXIT - диалог подтверждения
						gameMenu_.setPositionY(550);
						gameMenu_.pressButton(exitGameName, 4);
						break;
					}

					break;

					// РЕЖИМ 1: ПОДМЕНЮ START (SINGLEPLAYER/MULTIPLAYER/GO BACK)
				case 1: {

					// Обрабатывает нажатие кнопки.
					if (event.key.code != sf::Keyboard::Enter) {
						break;
					}

					switch (gameMenu_.getSelected()) {
					case 0: { // SINGLEPLAYER - настройка раундов/ботов

						bool singleplayerIsStaart = true; // активное подменю

						// Обрабатывает нажатие кнопки.
						sf::String roundsCount = settings[4].second; // кол-во раундов
						sf::String botIsSet = settings[5].second; // статус бота

						// === ВЛОЖЕННЫЙ ЦИКЛ НАСТРОЕК ОДИНОЧНОЙ ИГРЫ ===
						while (singleplayerIsStaart) {

							sf::Keyboard::Scancode code = sf::Keyboard::Scancode::Unknown;

							// Внутренний цикл обработки событий подменю
							while (window.pollEvent(event))
							{
								// Закрывает окно.
								if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
									window.close();

								// Обрабатывает нажатие кнопки.
								if (event.type == sf::Event::KeyReleased) {

									// Выбор нижестоящей кнопки.
									if (event.key.code == sf::Keyboard::Up) {
										singleplayerMenu.moveUp();
									}

									// Выбор нижестоящей кнопки.
									if (event.key.code == sf::Keyboard::Down) {
										singleplayerMenu.moveDown();
									}

									// Обработка выбора пунктов подменю
									switch (singleplayerMenu.getSelected()) {

									case 0: // ROUNDS — ввод цифры 1-9
										if (event.type == sf::Event::KeyReleased) {
											// Принимаем только цифры клавиатуры
											code = event.key.scancode;
											if (sf::Keyboard::getDescription(code) >= '1' && sf::Keyboard::getDescription(code) <= '9') {
												roundsCount = sf::Keyboard::getDescription(code);
											}
										}

										break;
									case 1: // BOTS — переключение Left / Right(ON / OFF циклически)
										if (event.type == sf::Event::KeyReleased) {

											code = event.key.scancode;
											if (sf::Keyboard::getDescription(code) == "Left") {

												// Цикл назад (OFF-ON, ON-OFF)
												if (botIsSetIt != botSettings.begin()) {
													botIsSet = *(--botIsSetIt);
												}
												else {
													botIsSet = *(++botIsSetIt);
												}
											}

											if (sf::Keyboard::getDescription(code) == "Right") {
												// Цикл вперёд
												if (botIsSetIt != --botSettings.end()) {
													botIsSet = *(++botIsSetIt);
												}
												else {
													botIsSet = *(--botIsSetIt);
												}
											}
										}

										break;
									case 2: // PLAY — запуск одиночной игры

										if (event.key.code != sf::Keyboard::Enter) {
											break;
										}

										if (event.type == sf::Event::KeyReleased) {

											std::string rounds = keyRoundsCount.getString();

											// Сохраняем настройки
											settings[4].second = rounds;
											settings[5].second = botIsSet;

											setSettings(settings);

											// Создаём и запускаем игру
											Snake snake;
											singleplayer game(snake);

											gameSingleplayerStart(window, game);
										}


										break;
									case 3: // GO BACK — выход из подменю

										if (event.key.code == sf::Keyboard::Enter) {
											singleplayerIsStaart = false;
										}
										break;
									}


								}
							}

							// Обновляем и отображаем тексты настроек
							keyRoundsCount.setFont(font);
							setControl(keyRoundsCount, roundsCount, 360); // Показываем кол-во раундов
							keyBot.setFont(font);
							setControl(keyBot, botIsSet, 460); // Показываем статус бота

							// Отрисовка кадра подменю одиночной игры
							window.clear();
							window.draw(background);
							window.draw(titul);
							singleplayerMenu.draw();
							window.draw(keyRoundsCount);
							window.draw(keyBot);
							window.display();
						}
						break;
					}
					case 1: { // РЕЖИМ 1: ПОДМЕНЮ START → MULTIPLAYER (настройка раундов)

						bool multyplayer = true;

						// Обрабатывает нажатие кнопки.
						sf::String roundsCount = settings[4].second;

						// === ВЛОЖЕННЫЙ ЦИКЛ НАСТРОЕК МНОГОПОЛЬЗОВАТЕЛЬСКОЙ ИГРЫ ===
						while (multyplayer) {


							sf::Keyboard::Scancode code = sf::Keyboard::Scancode::Unknown;


							// Внутренний цикл обработки событий (блокирует отрисовку!)
							while (window.pollEvent(event))
							{
								// Закрытие окна крестиком или Escape
								if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
									window.close();

								// Обработка отпускания клавиш
								if (event.type == sf::Event::KeyReleased) {
									// Навигация по меню multyplayerMenu (ROUNDS/PLAY/GO BACK)

									// Выбор нижестоящей кнопки.
									if (event.key.code == sf::Keyboard::Up) {
										multyplayerMenu.moveUp();
									}

									// Выбор нижестоящей кнопки.
									if (event.key.code == sf::Keyboard::Down) {
										multyplayerMenu.moveDown();
									}

									// Обработка выбранного пункта меню
									switch (multyplayerMenu.getSelected()) {

									case 0: // ROUNDS — ввод количества раундов цифрой 1-9
										if (event.type == sf::Event::KeyReleased) {
											code = event.key.scancode;

											// Принимаем только цифры с клавиатуры
											if (sf::Keyboard::getDescription(code) >= '1' && sf::Keyboard::getDescription(code) <= '9') {
												roundsCount = sf::Keyboard::getDescription(code);
											}
										}

										break;
									case 1: // PLAY — запуск многопользовательской игры

										if (event.key.code != sf::Keyboard::Enter) {
											break; // Только Enter активирует
										}

										if (event.type == sf::Event::KeyReleased) {

											std::string rounds = keyRoundsCount.getString();
											std::string bots = keyBot.getString();

											// Сохраняем количество раундов в настройки
											settings[4].second = rounds;

											setSettings(settings);

											// Запускаем игру вдвоём (multyGame создана выше в цикле)
											gameMultyplayerStart(window, multyGame);
											
										}


										break;
									case 2: // GO BACK — возврат в главное меню START

										if (event.key.code == sf::Keyboard::Enter) {
											multyplayer = false;  // Выход из подменю
										}
										break;
									}


								}
							}

							// Обновляем и отображаем текст с количеством раундов
							keyRoundsCount.setFont(font);
							setControl(keyRoundsCount, roundsCount, 450);

							// Отрисовка кадра подменю многопользовательской игры
							window.clear();
							window.draw(background);
							window.draw(titul);
							multyplayerMenu.draw();
							window.draw(keyRoundsCount);
							window.display();
						}
						break;
					}
						  // GO BACK из подменю START - возврат в главное меню
					case 2:
						gameMenu_.setPositionY(400); // Корректируем позицию Y главного меню
						gameMenu_.pressButton(name, 0); // Возвращаем кнопки START/SETTINGS/ABOUT/EXIT
						break; // конец switch(gameMenu_.getSelected()) для режима 1
					}
					break; // конец case 1 (подменю START)
				} // конец switch(gameMenu_.getMode())

					  // РЕЖИМ 2: МЕНЮ НАСТРОЕК (NICKNAME/CONTROL/THEME/FIELD SIZE/...)
				case 2: {

					switch (gameMenu_.getSelected()) {
					case 0: { // NICKNAME — редактирование имён игроков
						{
							// Активация только по Enter
							if (event.key.code != sf::Keyboard::Enter) {
								break;
							}

							bool nickMenu = true;

							// Загружаем текущие ники из настроек
							sf::String nick1 = settings[6].second;
							sf::String nick2 = settings[7].second;

							// === ВЛОЖЕННЫЙ ЦИКЛ РЕДАКТИРОВАНИЯ НИКОВ ===
							while (nickMenu) {

								sf::Keyboard::Scancode code = sf::Keyboard::Scancode::Unknown;

								// Внутренний цикл обработки событий
								while (window.pollEvent(event))
								{
									// Закрывает окно.
									if (event.type == sf::Event::Closed)
										window.close();

									// Обрабатывает нажатие кнопки.
									if (event.type == sf::Event::KeyReleased) {

										// Навигация по меню NicknamesMenu (PLAYER1/PLAYER2/SAVE/GO BACK)
										// Выбор нижестоящей кнопки.
										if (event.key.code == sf::Keyboard::Up) {
											NicknamesMenu.moveUp();
										}

										// Выбор нижестоящей кнопки.
										if (event.key.code == sf::Keyboard::Down) {
											NicknamesMenu.moveDown();
										}

										// Обработка выбранного пункта меню
										switch (NicknamesMenu.getSelected()) {

											 
										case 0: { // PLAYER1 — редактирование ника игрока 1
											if (event.type != sf::Event::KeyReleased) {
												break;
											}

											auto code = event.key.scancode;

											// Enter — очистить ник
											if (sf::Keyboard::getDescription(code) == "Enter") {
												nick1 = "";
												break;
											}

											// Backspace — удалить последний символ
											if (sf::Keyboard::getDescription(code) == "Backspace") {
												nick1 = nick1.substring(0, nick1.getSize() - 1);
												break;
											}

											// Символы A-Z, 0-9 (scancode 0-35)
											if (code >= 0 && code <= 35) {
												nick1 += sf::Keyboard::getDescription(code);
											}
											break;
										}
										case 1: { // PLAYER2 — редактирование ника игрока 2
											if (event.type != sf::Event::KeyReleased) {
												break;
											}

											auto code = event.key.scancode;

											// Enter — очистить ник
											if (sf::Keyboard::getDescription(code) == "Enter") {
												nick2 = "";
												break;
											}

											// Backspace — удалить последний символ
											if (sf::Keyboard::getDescription(code) == "Backspace") {
												nick2 = nick2.substring(0, nick2.getSize() - 1);
												break;
											}

											// Символы A-Z, 0-9
											if (code >= 0 && code <= 35) {
												nick2 += sf::Keyboard::getDescription(code);
											}

											break;
										}

										case 2: // SAVE — сохранить ники в настройки
											if (event.key.code == sf::Keyboard::Enter) {

												settings[6].second = nick1;
												settings[7].second = nick2;

												setSettings(settings); // Запись в файл
											}
											break;
										case 3: // GO BACK — выход из меню ников
											if (event.key.code == sf::Keyboard::Enter) {
												nickMenu = false;
											}
											break;
										}
									}

								}
								// конец внутреннего while(events)

								// Обновляем и отображаем тексты с никами
								nickname1.setFont(font);
								setControl(nickname1, nick1, 360);
								nickname2.setFont(font);
								setControl(nickname2, nick2, 460);

								// Отрисовка кадра меню ников
								window.clear();
								window.draw(background);
								window.draw(titul);
								NicknamesMenu.draw();
								// Меню (PLAYER1/PLAYER2/SAVE/GO BACK)
								window.draw(nickname1);
								window.draw(nickname2);
								window.display();
							}
						} // конец case 0 (NICKNAME)
						break;
					}

					case 1: {  // Пункт "CONTROL"

						// Проверяем нажатие клавиши Enter
						if (event.key.code != sf::Keyboard::Enter) {
							break;
						}

						// Флаг для отслеживания состояния меню управления
						bool control = true;

						// Цикл меню управления
						while (control) {

							// Вложенный цикл обработки событий в меню управления
							while (window.pollEvent(event))
							{
								// Проверяем событие закрытия окна
								if (event.type == sf::Event::Closed)
									window.close();

								// Обрабатываем отпускание клавиши
								if (event.type == sf::Event::KeyReleased) {

									// Перемещение вверх по меню
									if (event.key.code == sf::Keyboard::Up) {
										controlMenu.moveUp();
									}

									// Перемещение вниз по меню
									if (event.key.code == sf::Keyboard::Down) {
										controlMenu.moveDown();
									}

									// Обработка в зависимости от выбранного пункта меню
									switch (controlMenu.getSelected()) {

									case 0: {  // Пункт "PLAYER1"

										// Проверяем нажатие клавиши Enter
										if (event.key.code != sf::Keyboard::Enter) {
											break;
										}

										// Флаг для отслеживания состояния меню управления первого игрока
										bool player1 = true;

										// Получаем текущие настройки управления для первого игрока
										sf::String keyU = settings[0].second;  // Вверх
										sf::String keyL = settings[1].second;  // Влево
										sf::String keyD = settings[2].second;  // Вниз
										sf::String keyR = settings[3].second;  // Вправо

										// Цикл меню управления первого игрока
										while (player1) {

											// Переменная для хранения кода нажатой клавиши
											sf::Keyboard::Scancode code = sf::Keyboard::Scancode::Unknown;

											// Вложенный цикл обработки событий
											while (window.pollEvent(event))
											{
												// Проверяем событие закрытия окна
												if (event.type == sf::Event::Closed)
													window.close();

												// Обрабатываем отпускание клавиши
												if (event.type == sf::Event::KeyReleased) {

													// Перемещение вверх по меню
													if (event.key.code == sf::Keyboard::Up) {
														playerControlMenu.moveUp();
													}

													// Перемещение вниз по меню
													if (event.key.code == sf::Keyboard::Down) {
														playerControlMenu.moveDown();
													}

													// Обработка в зависимости от выбранного пункта меню
													switch (playerControlMenu.getSelected()) {

													case 0:  // Пункт "UP"
														if (event.type == sf::Event::KeyReleased) {
															code = event.key.scancode;  // Получаем код клавиши
															// Запрещаем использовать клавиши Up и Down для управления
															if (sf::Keyboard::getDescription(code) != "Up" && sf::Keyboard::getDescription(code) != "Down") {
																keyU = sf::Keyboard::getDescription(code);  // Устанавливаем новую клавишу
															}
														}
														break;

													case 1:  // Пункт "LEFT"
														if (event.type == sf::Event::KeyReleased) {
															code = event.key.scancode;
															if (sf::Keyboard::getDescription(code) != "Up" && sf::Keyboard::getDescription(code) != "Down") {
																keyL = sf::Keyboard::getDescription(code);
															}
														}
														break;

													case 2:  // Пункт "DOWN"
														if (event.type == sf::Event::KeyReleased) {
															code = event.key.scancode;
															if (sf::Keyboard::getDescription(code) != "Up" && sf::Keyboard::getDescription(code) != "Down") {
																keyD = sf::Keyboard::getDescription(code);
															}
														}
														break;

													case 3:  // Пункт "RIGHT"
														if (event.type == sf::Event::KeyReleased) {
															code = event.key.scancode;
															if (sf::Keyboard::getDescription(code) != "Up" && sf::Keyboard::getDescription(code) != "Down") {
																keyR = sf::Keyboard::getDescription(code);
															}
														}
														break;

													case 4:  // Пункт "SAVE"
														// Проверяем нажатие Enter для сохранения
														if (event.key.code == sf::Keyboard::Enter) {
															// Получаем строки с выбранными клавишами
															std::string Up = keyUp.getString();
															std::string Left = keyLeft.getString();
															std::string Down = keyDown.getString();
															std::string Right = keyRight.getString();

															// Обновляем настройки управления первого игрока
															settings[0].second = Up;
															settings[1].second = Left;
															settings[2].second = Down;
															settings[3].second = Right;

															// Сохраняем настройки в файл
															setSettings(settings);
														}
														break;

													case 5:  // Пункт "GO BACK"
														// Проверяем нажатие Enter для выхода
														if (event.key.code == sf::Keyboard::Enter) {
															player1 = false;  // Выходим из цикла
														}
														break;
													}
												}
											}

											// Настройка отображения выбранных клавиш
											keyLeft.setFont(font);
											setControl(keyLeft, keyL, 460);
											keyUp.setFont(font);
											setControl(keyUp, keyU, 360);
											keyDown.setFont(font);
											setControl(keyDown, keyD, 560);
											keyRight.setFont(font);
											setControl(keyRight, keyR, 660);

											// Очистка окна и отрисовка интерфейса
											window.clear();
											window.draw(background);
											window.draw(titul);
											playerControlMenu.draw();
											window.draw(keyUp);
											window.draw(keyLeft);
											window.draw(keyDown);
											window.draw(keyRight);
											window.display();
										}
										break;
									}

									case 1: {  // Пункт "PLAYER2"

										// Проверяем нажатие клавиши Enter
										if (event.key.code != sf::Keyboard::Enter) {
											break;
										}

										// Флаг для отслеживания состояния меню управления второго игрока
										bool player2 = true;

										// Получаем текущие настройки управления для второго игрока
										sf::String keyU = settings[13].second;  // Вверх
										sf::String keyL = settings[14].second;  // Влево
										sf::String keyD = settings[15].second;  // Вниз
										sf::String keyR = settings[16].second;  // Вправо

										// Цикл меню управления второго игрока
										while (player2) {

											// Переменная для хранения кода нажатой клавиши
											sf::Keyboard::Scancode code = sf::Keyboard::Scancode::Unknown;
											bool inLoop = false;  // Флаг для отслеживания нахождения в цикле (не используется)

											// Вложенный цикл обработки событий
											while (window.pollEvent(event))
											{
												// Проверяем событие закрытия окна
												if (event.type == sf::Event::Closed)
													window.close();

												// Обрабатываем отпускание клавиши
												if (event.type == sf::Event::KeyReleased) {

													// Перемещение вверх по меню
													if (event.key.code == sf::Keyboard::Up) {
														playerControlMenu.moveUp();
													}

													// Перемещение вниз по меню
													if (event.key.code == sf::Keyboard::Down) {
														playerControlMenu.moveDown();
													}

													bool notPressed = true;  // Флаг (не используется)

													// Обработка в зависимости от выбранного пункта меню
													switch (playerControlMenu.getSelected()) {

													case 0:  // Пункт "UP"
														if (event.type == sf::Event::KeyReleased) {
															code = event.key.scancode;
															// Запрещаем использовать клавиши Up и Down для управления
															if (sf::Keyboard::getDescription(code) != "Up" && sf::Keyboard::getDescription(code) != "Down") {
																keyU = sf::Keyboard::getDescription(code);
															}
														}
														break;

													case 1:  // Пункт "LEFT"
														if (event.type == sf::Event::KeyReleased) {
															code = event.key.scancode;
															if (sf::Keyboard::getDescription(code) != "Up" && sf::Keyboard::getDescription(code) != "Down") {
																keyL = sf::Keyboard::getDescription(code);
															}
														}
														break;

													case 2:  // Пункт "DOWN"
														if (event.type == sf::Event::KeyReleased) {
															code = event.key.scancode;
															if (sf::Keyboard::getDescription(code) != "Up" && sf::Keyboard::getDescription(code) != "Down") {
																keyD = sf::Keyboard::getDescription(code);
															}
														}
														break;

													case 3:  // Пункт "RIGHT"
														if (event.type == sf::Event::KeyReleased) {
															code = event.key.scancode;
															if (sf::Keyboard::getDescription(code) != "Up" && sf::Keyboard::getDescription(code) != "Down") {
																keyR = sf::Keyboard::getDescription(code);
															}
														}
														break;

													case 4:  // Пункт "SAVE"
														// Проверяем нажатие Enter для сохранения
														if (event.key.code == sf::Keyboard::Enter) {
															// Получаем строки с выбранными клавишами
															std::string Up = keyUp.getString();
															std::string Left = keyLeft.getString();
															std::string Down = keyDown.getString();
															std::string Right = keyRight.getString();

															// Обновляем настройки управления второго игрока
															settings[13].second = Up;
															settings[14].second = Left;
															settings[15].second = Down;
															settings[16].second = Right;

															// Сохраняем настройки в файл
															setSettings(settings);
														}
														break;

													case 5:  // Пункт "GO BACK"
														// Проверяем нажатие Enter для выхода
														if (event.key.code == sf::Keyboard::Enter) {
															player2 = false;  // Выходим из цикла
														}
														break;
													}
												}
											}

											// Настройка отображения выбранных клавиш
											keyLeft.setFont(font);
											setControl(keyLeft, keyL, 460);
											keyUp.setFont(font);
											setControl(keyUp, keyU, 360);
											keyDown.setFont(font);
											setControl(keyDown, keyD, 560);
											keyRight.setFont(font);
											setControl(keyRight, keyR, 660);

											// Очистка окна и отрисовка интерфейса
											window.clear();
											window.draw(background);
											window.draw(titul);
											playerControlMenu.draw();
											window.draw(keyUp);
											window.draw(keyLeft);
											window.draw(keyDown);
											window.draw(keyRight);
											window.display();
										}
										break;
									}

									case 2: {  // Пункт "GO BACK" в меню выбора игрока
										// Проверяем нажатие Enter для выхода
										if (event.key.code == sf::Keyboard::Enter) {
											control = false;  // Выходим из цикла
										}
										break;
									}
									}
								}
							}

							// Очистка окна и отрисовка меню выбора игрока
							window.clear();
							window.draw(background);
							window.draw(titul);
							controlMenu.draw();
							window.display();
						}
						break;
					}

					case 2: {  // Пункт "THEME"

						// Проверяем нажатие клавиши Enter
						if (event.key.code != sf::Keyboard::Enter) {
							break;
						}

						// Флаг для отслеживания состояния меню тем
						bool thems = true;

						// Находим итераторы на текущие цвета в соответствующих векторах
						auto player1ColorIt = std::find(playerColorSettings.begin(), playerColorSettings.end(), settings[9].second);
						auto player2ColorIt = std::find(playerColorSettings.begin(), playerColorSettings.end(), settings[10].second);
						auto appleColorIt = std::find(appleColorSettings.begin(), appleColorSettings.end(), settings[11].second);
						auto botColorIt = std::find(botColorSettings.begin(), botColorSettings.end(), settings[12].second);

						// Цикл меню тем
						while (thems) {

							// Создаем и настраиваем графический индикатор цвета первого игрока
							sf::RectangleShape player1Indicator;
							sf::Texture player1Texture;
							std::string player1TexturePath = "../designe/snake" + player1Color + ".png";
							if (!player1Texture.loadFromFile(player1TexturePath)) return -1;

							player1Indicator.setSize(sf::Vector2f(30, 30));
							player1Indicator.setPosition(sf::Vector2f(980, 390));
							player1Indicator.setTexture(&player1Texture);

							// Создаем и настраиваем графический индикатор цвета второго игрока
							sf::RectangleShape player2Indicator;
							sf::Texture player2Texture;
							std::string player2TexturePath = "../designe/snake" + player2Color + ".png";
							if (!player2Texture.loadFromFile(player2TexturePath)) return -1;

							player2Indicator.setSize(sf::Vector2f(30, 30));
							player2Indicator.setPosition(sf::Vector2f(980, 480));
							player2Indicator.setTexture(&player2Texture);

							// Создаем и настраиваем графический индикатор цвета яблока
							sf::RectangleShape appleIndicator;
							sf::Texture appleTextureIndicator;
							std::string appleTexturePath = "../designe/apple" + appleColor + ".png";
							if (!appleTextureIndicator.loadFromFile(appleTexturePath)) return -1;

							appleIndicator.setSize(sf::Vector2f(30, 30));
							appleIndicator.setPosition(sf::Vector2f(980, 570));
							appleIndicator.setTexture(&appleTextureIndicator);

							// Создаем и настраиваем графический индикатор цвета бота
							sf::RectangleShape botIndicator;
							sf::Texture botTextureIndicator;
							std::string botTexturePath = "../designe/bot" + botColor + ".png";
							if (!botTextureIndicator.loadFromFile(botTexturePath)) return -1;

							botIndicator.setSize(sf::Vector2f(30, 30));
							botIndicator.setPosition(sf::Vector2f(980, 670));
							botIndicator.setTexture(&botTextureIndicator);

							// Вложенный цикл обработки событий в меню тем
							while (window.pollEvent(event))
							{
								// Проверяем событие закрытия окна
								if (event.type == sf::Event::Closed)
									window.close();

								// Обрабатываем отпускание клавиши
								if (event.type == sf::Event::KeyReleased) {

									// Перемещение вверх по меню
									if (event.key.code == sf::Keyboard::Up) {
										themsMenu.moveUp();
									}

									// Перемещение вниз по меню
									if (event.key.code == sf::Keyboard::Down) {
										themsMenu.moveDown();
									}

									// Обработка в зависимости от выбранного пункта меню
									switch (themsMenu.getSelected()) {

									case 0: {  // Пункт "PLAYER1"
										if (event.type == sf::Event::KeyReleased) {
											auto code = event.key.scancode;  // Получаем код нажатой клавиши

											// Обработка клавиши "Left" для переключения цвета
											if (sf::Keyboard::getDescription(code) == "Left") {
												// Если текущий цвет не первый в списке
												if (player1ColorIt != playerColorSettings.begin()) {
													player1Color = *(--player1ColorIt);  // Переходим к предыдущему цвету
												}
												else {
													player1ColorIt = --playerColorSettings.end();  // Переходим в конец списка
													player1Color = *(player1ColorIt);  // Берем последний цвет
												}
											}

											// Обработка клавиши "Right" для переключения цвета
											if (sf::Keyboard::getDescription(code) == "Right") {
												// Если текущий цвет не последний в списке
												if (player1ColorIt != --playerColorSettings.end()) {
													player1Color = *(++player1ColorIt);  // Переходим к следующему цвету
												}
												else {
													player1ColorIt = playerColorSettings.begin();  // Переходим в начало списка
													player1Color = *(player1ColorIt);  // Берем первый цвет
												}
											}
										}
										break;
									}

									case 1: {  // Пункт "PLAYER2"
										if (event.type == sf::Event::KeyReleased) {
											auto code = event.key.scancode;

											// Обработка клавиши "Left" для переключения цвета
											if (sf::Keyboard::getDescription(code) == "Left") {
												if (player2ColorIt != playerColorSettings.begin()) {
													player2Color = *(--player2ColorIt);
												}
												else {
													player2ColorIt = --playerColorSettings.end();
													player2Color = *(player2ColorIt);
												}
											}

											// Обработка клавиши "Right" для переключения цвета
											if (sf::Keyboard::getDescription(code) == "Right") {
												if (player2ColorIt != --playerColorSettings.end()) {
													player2Color = *(++player2ColorIt);
												}
												else {
													player2ColorIt = playerColorSettings.begin();
													player2Color = *(player2ColorIt);
												}
											}
										}
										break;
									}

									case 2: {  // Пункт "APPLE"
										if (event.type == sf::Event::KeyReleased) {
											auto code = event.key.scancode;

											// Обработка клавиши "Left" для переключения цвета
											if (sf::Keyboard::getDescription(code) == "Left") {
												if (appleColorIt != appleColorSettings.begin()) {
													appleColor = *(--appleColorIt);
												}
												else {
													appleColorIt = --appleColorSettings.end();
													appleColor = *(appleColorIt);
												}
											}

											// Обработка клавиши "Right" для переключения цвета
											if (sf::Keyboard::getDescription(code) == "Right") {
												if (appleColorIt != --appleColorSettings.end()) {
													appleColor = *(++appleColorIt);
												}
												else {
													appleColorIt = appleColorSettings.begin();
													appleColor = *(appleColorIt);
												}
											}
										}
										break;
									}

									case 3: {  // Пункт "BOT"
										if (event.type == sf::Event::KeyReleased) {
											auto code = event.key.scancode;

											// Обработка клавиши "Left" для переключения цвета
											if (sf::Keyboard::getDescription(code) == "Left") {
												if (botColorIt != botColorSettings.begin()) {
													botColor = *(--botColorIt);
												}
												else {
													botColorIt = --botColorSettings.end();
													botColor = *(botColorIt);
												}
											}

											// Обработка клавиши "Right" для переключения цвета
											if (sf::Keyboard::getDescription(code) == "Right") {
												if (botColorIt != --botColorSettings.end()) {
													botColor = *(++botColorIt);
												}
												else {
													botColorIt = botColorSettings.begin();
													botColor = *(botColorIt);
												}
											}
										}
										break;
									}

									case 4: {  // Пункт "SAVE"
										// Проверяем нажатие Enter для сохранения
										if (event.key.code != sf::Keyboard::Enter)
											break;

										// Сохраняем выбранные цвета в настройки
										settings[9].second = *(player1ColorIt);
										settings[10].second = *(player2ColorIt);
										settings[11].second = *(appleColorIt);
										settings[12].second = *(botColorIt);

										// Сохраняем настройки в файл
										setSettings(settings);
										break;
									}

									case 5:  // Пункт "GO BACK"
										// Проверяем нажатие Enter для выхода
										if (event.key.code == sf::Keyboard::Enter) {
											thems = false;  // Выходим из цикла
										}
										break;
									}
								}
							}

							// Очистка окна и отрисовка интерфейса
							window.clear();
							window.draw(background);
							window.draw(titul);
							themsMenu.draw();
							window.draw(player1Indicator);
							window.draw(player2Indicator);
							window.draw(appleIndicator);
							window.draw(botIndicator);
							window.display();
						}
						break;
					}

					case 3: {  // Пункт "FIELD SIZE"

						// Проверяем нажатие клавиши Enter
						if (event.key.code != sf::Keyboard::Enter) {
							break;
						}

						// Флаг для отслеживания состояния меню размера поля
						bool field = true;

						// Цикл меню размера поля
						while (field)
						{
							// Вложенный цикл обработки событий
							while (window.pollEvent(event))
							{
								// Проверяем событие закрытия окна
								if (event.type == sf::Event::Closed)
									window.close();

								// Обрабатываем отпускание клавиши
								if (event.type == sf::Event::KeyReleased) {

									// Перемещение вверх по меню
									if (event.key.code == sf::Keyboard::Up) {
										fieldMenu.moveUp();
									}

									// Перемещение вниз по меню
									if (event.key.code == sf::Keyboard::Down) {
										fieldMenu.moveDown();
									}

									// Проверяем нажатие клавиши Enter
									if (event.key.code != sf::Keyboard::Enter)
										break;

									// Обработка выбранного пункта меню
									switch (fieldMenu.getSelected()) {
									case 0:  // Пункт "BIG"
										settings[8].second = "1";  // Устанавливаем размер поля "BIG"
										break;
									case 1:  // Пункт "MEDIUM"
										settings[8].second = "2";  // Устанавливаем размер поля "MEDIUM"
										break;
									case 2:  // Пункт "SMALL"
										settings[8].second = "3";  // Устанавливаем размер поля "SMALL"
										break;
									case 3:  // Пункт "GO BACK"
										setSettings(settings);  // Сохраняем настройки
										field = false;  // Выходим из цикла
										break;
									}
								}
							}

							// Очистка окна и отрисовка интерфейса
							window.clear();
							window.draw(background);
							window.draw(titul);
							fieldMenu.draw();
							window.display();
						}
						break;
					}

					case 4: {  // Пункт "SET DEFAULT SETTINGS"

						// Проверяем нажатие клавиши Enter
						if (event.key.code != sf::Keyboard::Enter) {
							break;
						}

						// Устанавливаем настройки по умолчанию
						setDefaultSettings();
						// Загружаем обновленные настройки
						settings = getSettings();
						break;
					}

					case 5: {  // Пункт "GO BACK" в меню настроек

						// Проверяем нажатие клавиши Enter
						if (event.key.code != sf::Keyboard::Enter) {
							break;
						}

						// Возвращаемся в главное меню
						gameMenu_.setPositionY(400);
						gameMenu_.pressButton(name, 0);
						break;
					}
					}
					break;  // Конец обработки меню настроек
				}

				// Меню "О программе" (режим 3)
				case 3: {
					// Обработка выбранного пункта в меню "О программе"
					switch (gameMenu_.getSelected()) {
					case 0:  // Пункт "GO BACK"
						// Проверяем нажатие Enter для возврата
						if (event.key.code == sf::Keyboard::Enter) {
							about = false;  // Сбрасываем флаг отображения информации о разработчиках
							gameMenu_.setPositionY(400);  // Восстанавливаем позицию меню
							gameMenu_.pressButton(name, 0);  // Возвращаемся в главное меню
						}
						break;
					}
					break;  // Конец обработки меню "О программе"
				}

					  // Меню выхода из игры (режим 4)
				case 4:
					// Проверяем нажатие клавиши Enter
					if (event.key.code != sf::Keyboard::Enter) {
						break;
					}

					// Обработка выбранного пункта в меню выхода
					switch (gameMenu_.getSelected()) {
					case 0:  // Пункт "GO BACK"
						gameMenu_.setPositionY(400);  // Восстанавливаем позицию меню
						gameMenu_.pressButton(name, 0);  // Возвращаемся в главное меню
						break;
					case 1:  // Пункт "EXIT"
						window.close();  // Закрываем окно
						return 0;  // Завершаем программу
					}
					break;  // Конец обработки меню выхода
				}
			}
		}

		// Отрисовка графического интерфейса
		window.clear();  // Очищаем окно
		window.draw(background);  // Отрисовываем фон

		// Если активен режим меню выхода (4), отрисовываем текст подтверждения
		if (gameMenu_.getMode() == 4) {
			window.draw(exit1);
			window.draw(exit2);
		}

		// Если активен режим "О программе", отрисовываем информацию о разработчиках
		if (about) {
			window.draw(developers);
			window.draw(owner);
			window.draw(developer);
			window.draw(kanye);
		}

		// Отрисовываем заголовок и главное меню
		window.draw(titul);
		gameMenu_.draw();
		window.display();  // Отображаем все нарисованное
	}

	// Завершение программы
	return 0;
}

// Реализация функции инициализации текста
void InitText(sf::Text& mtext, float xpos, float ypos, sf::String str, int size_font,
	sf::Color menu_text_color, int bord, sf::Color border_color)
{
	// Устанавливаем размер шрифта
	mtext.setCharacterSize(size_font);
	// Устанавливаем позицию текста
	mtext.setPosition(xpos, ypos);
	// Устанавливаем строку текста
	mtext.setString(str);
	// Устанавливаем цвет заполнения текста
	mtext.setFillColor(menu_text_color);
	// Устанавливаем толщину контура текста
	mtext.setOutlineThickness(bord);
	// Устанавливаем цвет контура текста
	mtext.setOutlineColor(border_color);
}

// Реализация функции установки текста управления
void setControl(sf::Text& key, sf::String keyName, float ypos)
{
	// Инициализируем текст с указанными параметрами
	InitText(key, 1000, ypos, keyName, 55, sf::Color::Yellow, 3, sf::Color(40, 40, 40));
}

// Реализация функции запуска одиночной игры
int gameSingleplayerStart(sf::RenderWindow& window, singleplayer game)
{
	// Запускаем одиночную игру и получаем результат
	int result = game.startSingleplayer(window);

	// Обрабатываем результат игры
	switch (result) {
	case 2:  // Рестарт игры
		gameSingleplayerStart(window, game);  // Рекурсивный вызов для перезапуска
		break;
	case 0:  // Возврат в меню
		return 1;  // Возвращаем 1 (успешное завершение с возвратом в меню)
	}

	return 1;  // Возвращаем 1 по умолчанию
}

// Реализация функции запуска многопользовательской игры
int gameMultyplayerStart(sf::RenderWindow& window, twoPlayaGame& game)
{
	// Запускаем многопользовательскую игру и получаем результат
	int result = game.startTwoPlayaGame(window);

	// Обрабатываем результат игры
	switch (result) {
	case 2:  // Рестарт игры
		gameMultyplayerStart(window, game);  // Рекурсивный вызов для перезапуска
		break;
	case 0:  // Возврат в меню
		return 1;  // Возвращаем 1 (успешное завершение с возвратом в меню)
	}

	return 1;  // Возвращаем 1 по умолчанию
}