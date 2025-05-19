#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <ctime>
#include <cstdlib>
#include <locale>

// ===== Инвентарь =====
template<typename T>
class Inventory {
private:
    std::vector<T> items;

public:
    void addItem(const T& item) {
        items.push_back(item);
        std::cout << "Предмет добавлен: " << item << "\n";
    }

    bool removeItem(const T& item) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (*it == item) {
                items.erase(it);
                std::cout << "Предмет удалён: " << item << "\n";
                return true;
            }
        }
        std::cout << "Предмет не найден в инвентаре: " << item << "\n";
        return false;
    }

    const std::vector<T>& getItems() const {
        return items;
    }

    void displayItems() const {
        std::cout << "Инвентарь:\n";
        for (const auto& item : items) {
            std::cout << "- " << item << "\n";
        }
    }

    void clear() {
        items.clear();
    }
};

// ===== Класс Персонаж =====
class Character {
private:
    std::string name;
    int health;
    int attack;
    int defense;
    int level;
    int experience;

public:
    Inventory<std::string> inventory;

    Character(const std::string& n = "Герой", int h = 100, int a = 10, int d = 5)
        : name(n), health(h), attack(a), defense(d), level(1), experience(0) {}

    void attackEnemy(class Monster& enemy);

    void heal(int amount) {
        health += amount;
        if (health > 100) health = 100;
        if (amount > 0) {
            std::cout << name << " восстанавливает " << amount << " HP!\n";
        }
        else {
            std::cout << name << " получает урон: " << -amount << " HP!\n";
        }
    }

    void gainExperience(int exp);

    void displayInfo() const {
        std::cout << "Имя: " << name << "\nHP: " << health
            << "\nАтака: " << attack << "\nЗащита: " << defense
            << "\nУровень: " << level << "\nОпыт: " << experience << "\n";
        inventory.displayItems();
    }

    bool isAlive() const { return health > 0; }
    int getHealth() const { return health; }
    const std::string& getName() const { return name; }
    int getDefense() const { return defense; }

    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);
};

// ===== Базовый класс Монстр =====
class Monster {
protected:
    std::string name;
    int health;
    int attackPower;
    int defense;

public:
    virtual ~Monster() = default;

    virtual void attack(Character& player) {
        int baseDamage = attackPower - player.getDefense();
        if (baseDamage > 0) {
            player.heal(-baseDamage);
            std::cout << name << " атакует " << player.getName() << " и наносит " << baseDamage << " урона!\n";
        }
        else {
            // Добавляем случайный урон от 1 до 3, чтобы гарантировать минимум 1 урон
            int randomDamage = rand() % 3 + 1;
            player.heal(-randomDamage);
            std::cout << name << " атакует " << player.getName() << " и наносит " << randomDamage << " урона!\n";
        }
    }

    virtual void takeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
    }

    virtual bool isAlive() const { return health > 0; }
    virtual const std::string& getName() const { return name; }
    virtual int getDefense() const { return defense; }

    virtual void displayInfo() const {
        std::cout << "Монстр: " << name << "\nHP: " << health
            << "\nАтака: " << attackPower << "\nЗащита: " << defense << "\n";
    }
};

// ===== Конкретные монстры =====
class Goblin : public Monster {
public:
    Goblin() {
        name = "Гоблин";
        health = 30;
        attackPower = 12; 
        defense = 3;
    }
};

class Skeleton : public Monster {
public:
    Skeleton() {
        name = "Скелет";
        health = 40;
        attackPower = 15;
        defense = 5;
    }
};

class Dragon : public Monster {
public:
    Dragon() {
        name = "Дракон";
        health = 100;
        attackPower = 20;
        defense = 10;
    }
};

// ===== Реализация методов класса Character =====
void Character::attackEnemy(Monster& enemy) {
    int damage = attack - enemy.getDefense();
    if (damage > 0) {
        enemy.takeDamage(damage);
        std::cout << name << " атакует " << enemy.getName() << " и наносит " << damage << " урона!\n";
    }
    else {
        std::cout << name << " атакует " << enemy.getName() << ", но без эффекта.\n";
    }
}

void Character::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Не удалось открыть файл для сохранения.");

    file << name << '\n' << health << '\n' << attack << '\n'
        << defense << '\n' << level << '\n' << experience << '\n';

    for (const auto& item : inventory.getItems()) {
        file << item << '\n';
    }
    file << "END_ITEMS\n";

    file.close();
    std::cout << "Прогресс успешно сохранён.\n";
}

void Character::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Не удалось открыть файл для загрузки.");

    std::getline(file, name);
    file >> health >> attack >> defense >> level >> experience;
    file.ignore();

    std::string line;
    inventory.clear();
    while (std::getline(file, line)) {
        if (line == "END_ITEMS") break;
        inventory.addItem(line);
    }

    file.close();
    std::cout << "Прогресс успешно загружен.\n";
}

void Character::gainExperience(int exp) {
    experience += exp;
    while (experience >= level * 100) {
        experience -= level * 100;
        level++;
        attack += 2;
        defense += 1;
        std::cout << name << " повысил уровень! Теперь уровень: " << level << "\n";
    }
}

// ===== Игровая логика =====
class Game {
private:
    Character player;
    std::vector<std::unique_ptr<Monster>> monsters;

public:
    void start();
    void battle(Monster& monster);
};

void Game::start() {
    srand(static_cast<unsigned int>(time(0)));

    try {
        player.loadFromFile("save.txt");
    }
    catch (...) {
        std::cout << "Файл сохранения не найден. Начинаем новую игру.\n";
    }

    player.displayInfo();

    monsters.push_back(std::make_unique<Goblin>());
    monsters.push_back(std::make_unique<Skeleton>());
    monsters.push_back(std::make_unique<Dragon>());

    for (auto& mon : monsters) {
        battle(*mon);
        if (!player.isAlive()) break;
    }

    if (player.isAlive()) {
        std::cout << "Вы победили всех врагов! Победа!\n";
        player.saveToFile("save.txt");
    }
}

void Game::battle(Monster& monster) {
    std::cout << "На вас напал " << monster.getName() << "!\n";
    while (player.isAlive() && monster.isAlive()) {
        // Меню действий
        int choice;
        do {
            std::cout << "\nВыберите действие:\n";
            std::cout << "1. Атаковать\n";
            std::cout << "2. Лечиться\n";
            std::cout << "3. Сохранить игру\n";
            std::cout << "0. Выйти из игры\n";
            std::cout << "Ваш выбор: ";
            std::cin >> choice;

            switch (choice) {
            case 1:
                player.attackEnemy(monster);
                break;
            case 2:
                if (player.inventory.removeItem("Зелье здоровья")) {
                    player.heal(25);
                }
                else {
                    std::cout << "У вас нет зелий здоровья.\n";
                }
                break;
            case 3:
                player.saveToFile("save.txt");
                std::cout << "Игра сохранена.\n";
                break;
            case 0:
                std::cout << "Выход из игры...\n";
                exit(0);
                break;
            default:
                std::cout << "Неверный выбор. Попробуйте ещё раз.\n";
            }
        } while (choice != 1 && choice != 2 && choice != 3 && choice != 0);

        if (monster.isAlive()) {
            monster.attack(player);
        }
        player.displayInfo();
        std::cout << "--------------------\n";
    }

    if (player.isAlive()) {
        std::cout << "Вы победили " << monster.getName() << "!\n";
        player.gainExperience(50);
        player.inventory.addItem("Зелье здоровья");
    }
    else {
        std::cout << "Вы были побеждены...\n";
    }
}

// ===== Точка входа =====
int main() {
    setlocale(LC_ALL, "");

    try {
        Game game;
        game.start();
    }
    catch (const std::exception & e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }

    return 0;
}
