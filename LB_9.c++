#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <memory>

// Шаблонный класс Logger
template<typename T>
class Logger {
private:
    std::string filename;

public:
    Logger(const std::string& fname) : filename(fname) {}

    void log(const T& message) {
        std::ofstream file(filename, std::ios::app);
        if (!file) throw std::runtime_error("Failed to open log file");
        file << message << std::endl;
    }
};

// Базовый класс для всех существ
class Entity {
protected:
    std::string name;
    int health;
    int attack;
    int defense;

public:
    Entity(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {}

    virtual ~Entity() = default;

    virtual void takeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
    }

    int getHealth() const { return health; }
    std::string getName() const { return name; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }

    virtual void attackEntity(Entity& target) {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        } else {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!\n";
        }
        
        if (target.getHealth() <= 0) {
            throw std::runtime_error(target.getName() + " has been defeated!");
        }
    }
};

// Класс инвентаря
class Inventory {
private:
    std::vector<std::string> items;

public:
    void addItem(const std::string& item) { items.push_back(item); }
    void removeItem(const std::string& item) {
        auto it = std::find(items.begin(), items.end(), item);
        if (it != items.end()) items.erase(it);
    }
    void display() const {
        std::cout << "Inventory: ";
        for (const auto& item : items) std::cout << item << " ";
        std::cout << "\n";
    }
    const std::vector<std::string>& getItems() const { return items; }
    void setItems(const std::vector<std::string>& newItems) { items = newItems; }
};

// Класс персонажа
class Character : public Entity {
    int level;
    int experience;
    Inventory inventory;

public:
    Character(const std::string& n, int h, int a, int d)
        : Entity(n, h, a, d), level(1), experience(0) {}

    void heal(int amount) {
        health += amount;
        if (health > 100) health = 100;
        std::cout << name << " heals for " << amount << " HP!\n";
    }

    void gainExperience(int exp) {
        experience += exp;
        if (experience >= 100) {
            level++;
            experience -= 100;
            std::cout << name << " leveled up to level " << level << "!\n";
        }
    }

    void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health
                  << ", Attack: " << attack << ", Defense: " << defense
                  << ", Level: " << level << ", Experience: " << experience << "\n";
    }

    Inventory& getInventory() { return inventory; }
    int getLevel() const { return level; }
    int getExperience() const { return experience; }
    void setLevel(int lv) { level = lv; }
    void setExperience(int exp) { experience = exp; }
};

// Классы монстров
class Monster : public Entity {
public:
    Monster(const std::string& n, int h, int a, int d) : Entity(n, h, a, d) {}
};

class Goblin : public Monster {
public:
    Goblin() : Monster("Goblin", 30, 10, 5) {}
};

class Dragon : public Monster {
public:
    Dragon() : Monster("Dragon", 100, 20, 15) {}
};

class Skeleton : public Monster {
public:
    Skeleton() : Monster("Skeleton", 40, 12, 8) {}
};

// Класс игры
class Game {
    std::unique_ptr<Character> player;
    Logger<std::string> logger{"game_log.txt"};

public:
    void createCharacter() {
        std::string name;
        std::cout << "Enter character name: ";
        std::cin >> name;
        player = std::make_unique<Character>(name, 100, 15, 10);
        logger.log("Character created: " + name);
    }

    void battle() {
        if (!player) throw std::runtime_error("No character created!");
        
        Monster* monster = nullptr;
        int choice = rand() % 3;
        switch(choice) {
            case 0: monster = new Goblin(); break;
            case 1: monster = new Dragon(); break;
            case 2: monster = new Skeleton(); break;
        }

        logger.log(player->getName() + " encountered a " + monster->getName());
        std::cout << "A wild " << monster->getName() << " appears!\n";

        try {
            while (true) {
                player->attackEntity(*monster);
                if (monster->getHealth() <= 0) {
                    player->gainExperience(50);
                    logger.log(player->getName() + " defeated " + monster->getName());
                    break;
                }
                monster->attackEntity(*player);
            }
        } catch (const std::exception& e) {
            std::cout << e.what() << "\n";
        }

        delete monster;
    }

    void saveGame(const std::string& filename) {
        std::ofstream file(filename);
        if (!file) throw std::runtime_error("Failed to save game");

        file << player->getName() << "\n"
             << player->getHealth() << "\n"
             << player->getAttack() << "\n"
             << player->getDefense() << "\n"
             << player->getLevel() << "\n"
             << player->getExperience() << "\n";

        for (const auto& item : player->getInventory().getItems()) {
            file << item << "\n";
        }
        logger.log("Game saved to " + filename);
    }

    void loadGame(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) throw std::runtime_error("Failed to load game");

        std::string name;
        int health, attack, defense, level, experience;
        file >> name >> health >> attack >> defense >> level >> experience;

        player = std::make_unique<Character>(name, health, attack, defense);
        player->setLevel(level);
        player->setExperience(experience);

        std::vector<std::string> items;
        std::string item;
        while (file >> item) {
            items.push_back(item);
        }
        player->getInventory().setItems(items);
        logger.log("Game loaded from " + filename);
    }

    void showMenu() {
        while (true) {
            std::cout << "\n1. Battle\n2. Save\n3. Load\n4. Exit\nChoice: ";
            int choice;
            std::cin >> choice;

            try {
                switch(choice) {
                    case 1: battle(); break;
                    case 2: saveGame("save.txt"); break;
                    case 3: loadGame("save.txt"); break;
                    case 4: return;
                    default: std::cout << "Invalid choice!\n";
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << "\n";
            }
        }
    }
};

int main() {
    Game game;
    game.createCharacter();
    game.showMenu();
    return 0;
}