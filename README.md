# Texas Hold'em Poker (C++)

Консольная реализация покера Texas Hold'em.

## Возможности

- Полные правила Texas Hold'em: префлоп, флоп, тёрн, ривер, блайнды, олл-ин, сплит-пот
- ИИ-боты с оценкой силы руки, pot odds, рейзами и блефами
- Таблица рекордов с сохранением в файл
- Логирование событий в `poker.log` (INFO / WARNING / ERROR)
- Конфигурационный файл `config.ini` для настройки параметров
- Валидация всего пользовательского ввода
- Unit-тесты (Google Test)

## Сборка

### Требования

- CMake 3.15+
- Компилятор с поддержкой C++17
- Google Test (для тестов): `sudo apt install libgtest-dev`

### Linux / macOS

```bash
git clone https://github.com/raitar/poker.git
cd poker
mkdir build && cd build
cmake ..
cmake --build .
./poker
```

### Windows

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
Release\poker.exe
```

### Запуск тестов

```bash
cd build
cmake --build . --target run_tests
ctest --output-on-failure
```

## Конфигурация

При запуске программа читает `config.ini` из рабочей директории. Если файл отсутствует — используются значения по умолчанию.

```ini
# config.ini
STARTING_CHIPS = 2000   # стартовые фишки (100..1000000)
SMALL_BLIND    = 10     # малый блайнд (1..10000)
LARGE_BLIND    = 20     # большой блайнд (2..20000)
LOG_FILE       = poker.log
```

## Структура проекта

```
poker/
├── main.cpp
├── CMakeLists.txt
├── config.ini
├── core/
│   ├── types.h          # Card, Player, HandEvaluation, enum
│   ├── utils.h/.cpp     # Утилиты ввода/вывода
│   ├── logger.h/.cpp    # Логирование
│   └── config.h/.cpp    # Парсер конфига
├── engine/
│   ├── poker_engine.h/.cpp   # Оценка комбинаций
│   ├── ai_player.h/.cpp      # Логика ботов
│   └── texas_holdem.h/.cpp   # Игровой движок
├── ui/
│   ├── leaderboard.h/.cpp    # Таблица рекордов
│   └── menu.h/.cpp           # Меню
└── tests/
    └── test_poker.cpp        # Unit-тесты (15 кейсов)
```

## Правила игры

Каждый игрок получает 2 закрытые карты. Затем поэтапно открываются 5 общих карт:

| Стадия | Карты |
|--------|-------|
| Флоп   | 3 карты |
| Тёрн   | 1 карта |
| Ривер  | 1 карта |

На каждой стадии — раунд ставок. Доступные действия: **Check / Call / Raise / Fold**.  
Побеждает лучшая 5-карточная комбинация из любых 7 доступных.

| Комбинация | Пример |
|------------|--------|
| Стрит-флеш | 9♠ 10♠ J♠ Q♠ K♠ |
| Каре | A♣ A♦ A♥ A♠ K♣ |
| Фулл-хаус | K♣ K♦ K♥ Q♠ Q♣ |
| Флеш | 2♥ 5♥ 7♥ J♥ A♥ |
| Стрит | 5♣ 6♦ 7♥ 8♠ 9♣ |
| Тройка | 8♣ 8♦ 8♥ 2♠ 5♣ |
| Две пары | K♣ K♦ J♥ J♠ 2♣ |
| Пара | A♣ A♦ 3♥ 7♠ K♣ |
| Старшая карта | 2♣ 5♦ 7♥ 9♠ J♣ |
