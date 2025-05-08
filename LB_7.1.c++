#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <string>

// Определение базового класса Entity
class Entity {
public:
    virtual ~Entity() = default;
    virtual std::string getName() const = 0;
    virtual int getHealth() const = 0;
    virtual int getLevel() const = 0;
    virtual std::string serialize() const = 0; // Метод для сериализации
};

// Определение класса Player
class Player : public Entity {
private:
    std::string name;
    int health;
    int level;

public:
    Player(const std::string& name, int health, int level)
        : name(name), health(health), level(level) {}

    std::string getName() const override { return name; }
    int getHealth() const override { return health; }
    int getLevel() const override { return level; }

    std::string serialize() const override {
        return name + " " + std::to_string(health) + " " + std::to_string(level);
    }
};

// Определение класса GameManager
template <typename T>
class GameManager {
private:
    std::vector<T> entities;

public:
    void addEntity(T entity) {
        entities.push_back(entity);
    }

    const std::vector<T>& getEntities() const {
        return entities;
    }

    void displayAll() const {
        for (const auto& entity : entities) {
            std::cout << entity->getName() << " (Health: " << entity->getHealth()
                      << ", Level: " << entity->getLevel() << ")\n";
        }
    }
};

// Метод для сохранения данных в файл
void saveToFile(const GameManager<Entity*>& manager, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file for writing.");
    }

    for (const auto& entity : manager.getEntities()) {
        file << entity->serialize() << "\n"; // Сохраняем данные о каждом персонаже
    }
}

// Метод для загрузки данных из файла
void loadFromFile(GameManager<Entity*>& manager, const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading.");
    }

    std::string name;
    int health, level;
    while (file >> name >> health >> level) {
        manager.addEntity(new Player(name, health, level)); // Загружаем данные о персонаже
    }
}

int main() {
    GameManager<Entity*> manager;
    manager.addEntity(new Player("Hero", 100, 1));
    manager.addEntity(new Player("Mage", 80, 2));
    manager.addEntity(new Player("Warrior", 120, 3));

    // Сохранение данных в файл
    try {
        saveToFile(manager, "game_save.txt");
        std::cout << "Game data saved to game_save.txt" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Загрузка данных из файла
    GameManager<Entity*> loadedManager;
    try {
        loadFromFile(loadedManager, "game_save.txt");
        std::cout << "Game data loaded from game_save.txt" << std::endl;
        loadedManager.displayAll();
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Освобождение памяти
    for (auto entity : manager.getEntities()) {
        delete entity;
    }
    for (auto entity : loadedManager.getEntities()) {
        delete entity;
    }

    return 0;
}