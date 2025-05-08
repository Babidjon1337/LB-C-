#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <string>

// === Базовый класс User ===
class User {
private:
    std::string name;
    int id;
    int accessLevel;

public:
    User(const std::string& name, int id, int accessLevel)
        : name(name), id(id), accessLevel(accessLevel) {
        if (name.empty()) throw std::invalid_argument("Имя пользователя не может быть пустым.");
        if (accessLevel < 0) throw std::invalid_argument("Уровень доступа не может быть отрицательным.");
    }

    virtual void displayInfo() const {
        std::cout << "Пользователь: " << name
                  << ", ID: " << id
                  << ", Уровень доступа: " << accessLevel << std::endl;
    }

    virtual void save(std::ostream& out) const {
        out << name << "," << id << "," << accessLevel << "\n";
    }

    virtual bool load(std::istream& in) {
        std::string line;
        if (!std::getline(in, line)) return false;

        std::istringstream ss(line);
        std::string token;

        std::getline(ss, name, ',');
        std::getline(ss, token, ','); id = std::stoi(token);
        std::getline(ss, token, ','); accessLevel = std::stoi(token);

        return true;
    }

    // Геттеры
    std::string getName() const { return name; }
    int getId() const { return id; }
    int getAccessLevel() const { return accessLevel; }

    virtual ~User() = default;
};

// === Класс Student ===
class Student : public User {
private:
    std::string group;

public:
    Student(const std::string& name, int id, int accessLevel, const std::string& group)
        : User(name, id, accessLevel), group(group) {}

    void displayInfo() const override {
        User::displayInfo();
        std::cout << "Группа: " << group << std::endl;
    }

    void save(std::ostream& out) const override {
        out << "Student," << getName() << "," << getId() << "," << getAccessLevel() << "," << group << "\n";
    }

    bool load(std::istream& in) override {
        std::string line;
        if (!std::getline(in, line)) return false;

        std::istringstream ss(line);
        std::string type, name, group;
        int id, accessLevel;

        std::getline(ss, type, ',');
        std::getline(ss, name, ',');
        std::getline(ss, group, ',');

        id = std::stoi(group); // Здесь нужно уточнить формат
        accessLevel = std::stoi(group);
        this->group = group;

        return true;
    }
};

// === Класс Teacher ===
class Teacher : public User {
private:
    std::string department;

public:
    Teacher(const std::string& name, int id, int accessLevel, const std::string& department)
        : User(name, id, accessLevel), department(department) {}

    void displayInfo() const override {
        User::displayInfo();
        std::cout << "Кафедра: " << department << std::endl;
    }

    void save(std::ostream& out) const override {
        out << "Teacher," << getName() << "," << getId() << "," << getAccessLevel() << "," << department << "\n";
    }
};

// === Класс Administrator ===
class Administrator : public User {
public:
    Administrator(const std::string& name, int id, int accessLevel)
        : User(name, id, accessLevel) {}

    void displayInfo() const override {
        User::displayInfo();
        std::cout << "Роль: Администратор\n";
    }

    void save(std::ostream& out) const override {
        out << "Administrator," << getName() << "," << getId() << "," << getAccessLevel() << "\n";
    }
};

// === Класс Resource ===
class Resource {
private:
    std::string name;
    int requiredAccessLevel;

public:
    Resource(const std::string& name, int requiredAccessLevel)
        : name(name), requiredAccessLevel(requiredAccessLevel) {}

    bool checkAccess(const User& user) const {
        return user.getAccessLevel() >= requiredAccessLevel;
    }

    void display() const {
        std::cout << "Ресурс: " << name
                  << ", Требуемый уровень доступа: " << requiredAccessLevel << std::endl;
    }

    std::string getName() const { return name; }

    void save(std::ostream& out) const {
        out << name << "," << requiredAccessLevel << "\n";
    }

    bool load(std::istream& in) {
        std::string line;
        if (!std::getline(in, line)) return false;

        std::istringstream ss(line);
        std::string token;

        std::getline(ss, name, ',');
        std::getline(ss, token, ',');
        requiredAccessLevel = std::stoi(token);

        return true;
    }
};

// === Шаблонный класс AccessControlSystem ===
template<typename T>
class AccessControlSystem {
private:
    std::vector<std::shared_ptr<T>> items;

public:
    void addItem(std::shared_ptr<T> item) {
        items.push_back(item);
    }

    void displayAll() const {
        for (const auto& item : items) {
            item->displayInfo();
            std::cout << "--------------------\n";
        }
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream out(filename);
        if (!out) throw std::runtime_error("Не удалось открыть файл для записи.");

        for (const auto& item : items) {
            item->save(out);
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream in(filename);
        if (!in) throw std::runtime_error("Не удалось открыть файл для чтения.");

        while (!in.eof()) {
            std::string line;
            if (!std::getline(in, line)) break;

            std::istringstream ss(line);
            std::string type;
            std::getline(ss, type, ',');

            std::shared_ptr<T> item = std::make_shared<T>();
            in.seekg(-(long long)(line.size() + 1), std::ios_base::cur); // вернуться к началу строки

            if (item->load(in)) {
                items.push_back(item);
            }
        }
    }

    template<typename Compare>
    void sortItems(Compare comp) {
        std::sort(items.begin(), items.end(), comp);
    }

    std::shared_ptr<T> findByName(const std::string& name) const {
        for (const auto& item : items) {
            if constexpr (std::is_same_v<T, User>) {
                if (item->getName() == name) return item;
            } else if constexpr (std::is_same_v<T, Resource>) {
                if (item->getName() == name) return item;
            }
        }
        return nullptr;
    }

    std::shared_ptr<T> findById(int id) const {
        for (const auto& item : items) {
            if constexpr (std::is_same_v<T, User>) {
                if (item->getId() == id) return item;
            }
        }
        return nullptr;
    }
};

// === Основная функция main ===
int main() {
    try {
        // Создаем систему контроля доступа
        AccessControlSystem<User> accessSystem;

        // Добавляем пользователей
        accessSystem.addItem(std::make_shared<Student>("Александр", 101, 2, "М-31"));
        accessSystem.addItem(std::make_shared<Teacher>("Елена Петрова", 202, 5, "Физика"));
        accessSystem.addItem(std::make_shared<Administrator>("Иван Иванов", 303, 10));

        // Добавляем ресурсы
        AccessControlSystem<Resource> resourceSystem;
        resourceSystem.addItem(std::make_shared<Resource>("Лаборатория по физике", 5));
        resourceSystem.addItem(std::make_shared<Resource>("Библиотека", 2));
        resourceSystem.addItem(std::make_shared<Resource>("Серверная комната", 9));

        // Проверяем доступ студенту
        auto student = accessSystem.findById(101);
        auto lab = resourceSystem.findByName("Лаборатория по физике");

        if (student && lab) {
            std::cout << "\nПроверка доступа студента к лаборатории:\n";
            if (lab->checkAccess(*student)) {
                std::cout << student->getName() << " имеет доступ к " << lab->getName() << ".\n";
            } else {
                std::cout << student->getName() << " НЕ имеет доступа к " << lab->getName() << ".\n";
            }
        }

        // Сохраняем данные в файл
        accessSystem.saveToFile("users.txt");
        resourceSystem.saveToFile("resources.txt");

        // Загружаем из файла
        AccessControlSystem<User> loadedUsers;
        loadedUsers.loadFromFile("users.txt");

        std::cout << "\nЗагруженные пользователи:\n";
        loadedUsers.displayAll();

        // Сортировка по уровню доступа
        loadedUsers.sortItems([](const std::shared_ptr<User>& a, const std::shared_ptr<User>& b) {
            return a->getAccessLevel() > b->getAccessLevel();
        });

        std::cout << "\nОтсортированные пользователи по уровню доступа:\n";
        loadedUsers.displayAll();

    } catch (const std::exception& ex) {
        std::cerr << "Ошибка: " << ex.what() << std::endl;
    }

    return 0;
}