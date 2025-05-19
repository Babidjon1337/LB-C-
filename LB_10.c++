#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <typeinfo>

class Resource;

// === Вспомогательные функции ===

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) ++start;

    auto end = s.end();
    do {
        --end;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

// === Базовый класс User ===
class User {
protected:
    std::string name;
    int id;
    int accessLevel;

public:
    User(const std::string& name, int id, int accessLevel);

    virtual void displayInfo() const = 0;

    bool checkAccessToResource(const Resource& resource) const;

    const std::string& getName() const { return name; }
    int getId() const { return id; }
    int getAccessLevel() const { return accessLevel; }

    void setName(const std::string& name);
    void setId(int id);
    void setAccessLevel(int level);
};

// === Класс Student ===
class Student : public User {
private:
    std::string group;

public:
    Student(const std::string& name, int id, int accessLevel, const std::string& group);

    void displayInfo() const override;

    const std::string& getGroup() const { return group; }
};

// === Класс Teacher ===
class Teacher : public User {
private:
    std::string department;

public:
    Teacher(const std::string& name, int id, int accessLevel, const std::string& department);

    void displayInfo() const override;

    const std::string& getDepartment() const { return department; }
};

// === Класс Administrator ===
class Administrator : public User {
public:
    Administrator(const std::string& name, int id, int accessLevel);

    void displayInfo() const override;
};

// === Класс Resource ===
class Resource {
private:
    std::string name;
    int requiredAccessLevel;

public:
    Resource(const std::string& name, int requiredAccessLevel);

    const std::string& getName() const { return name; }
    int getRequiredAccessLevel() const { return requiredAccessLevel; }

    void setName(const std::string& name);
    void setRequiredAccessLevel(int level);
};

// === Шаблонный класс AccessControlSystem ===
template<typename T>
class AccessControlSystem {
private:
    std::vector<std::shared_ptr<T>> users;
    std::vector<Resource> resources;

public:
    void addUser(std::shared_ptr<T> user) {
        users.push_back(user);
    }

    void addResource(const Resource& resource) {
        resources.push_back(resource);
    }

    void displayAllUsers() const {
        std::cout << "=== Все пользователи ===\n";
        for (const auto& user : users) {
            user->displayInfo();
        }
    }

    void displayResources() const {
        std::cout << "\n=== Ресурсы ===\n";
        for (const auto& res : resources) {
            std::cout << "Ресурс: " << res.getName()
                << ", Требуемый уровень доступа: " << res.getRequiredAccessLevel() << "\n";
        }
    }

    bool checkAccess(const T& user, const std::string& resourceName) const {
        for (const auto& res : resources) {
            if (res.getName() == resourceName)
                return user.checkAccessToResource(res);
        }
        return false;
    }

    std::vector<std::shared_ptr<T>> findUserExact(const std::string& name) const {
        std::vector<std::shared_ptr<T>> result;
        for (const auto& user : users) {
            if (user->getName() == name)
                result.push_back(user);
        }
        return result;
    }

    void checkAccessForResource(const T& user) const {
        std::cout << "\n=== Доступ для пользователя \"" << user.getName() << "\" ===\n";
        for (const auto& res : resources) {
            bool hasAccess = user.checkAccessToResource(res);
            std::cout << "Ресурс: " << res.getName()
                << " -> " << (hasAccess ? "Разрешён" : "Запрещён") << "\n";
        }
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream out(filename);
        if (!out) {
            throw std::runtime_error("Не удалось открыть файл для записи: " + filename);
        }

        out << users.size() << "\n";
        for (const auto& user : users) {
            out << typeid(*user).name() << " "
                << user->getName() << " "
                << user->getId() << " "
                << user->getAccessLevel() << " ";

            if (dynamic_cast<Student*>(user.get())) {
                out << dynamic_cast<Student*>(user.get())->getGroup() << "\n";
            }
            else if (dynamic_cast<Teacher*>(user.get())) {
                out << dynamic_cast<Teacher*>(user.get())->getDepartment() << "\n";
            }
            else {
                out << "\n"; // администратор
            }
        }

        out << resources.size() << "\n";
        for (const auto& res : resources) {
            out << res.getName() << " " << res.getRequiredAccessLevel() << "\n";
        }

        std::cout << "Данные успешно сохранены в файл: " << filename << "\n";
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream in(filename);
        if (!in) {
            throw std::runtime_error("Не удалось открыть файл для чтения: " + filename);
        }

        users.clear();
        resources.clear();

        int count;
        in >> count;
        in.ignore(); // пропустить символ новой строки

        for (int i = 0; i < count; ++i) {
            std::string type, name, extra;
            int id, level;

            std::getline(in, type, ' ');
            in >> id >> level;
            in.ignore();
            std::getline(in, name, ' ');

            if (type.find("Student") != std::string::npos) {
                std::getline(in, extra);
                auto student = std::make_shared<Student>(name, id, level, extra);
                users.push_back(student);
            }
            else if (type.find("Teacher") != std::string::npos) {
                std::getline(in, extra);
                auto teacher = std::make_shared<Teacher>(name, id, level, extra);
                users.push_back(teacher);
            }
            else if (type.find("Administrator") != std::string::npos) {
                auto admin = std::make_shared<Administrator>(name, id, level);
                users.push_back(admin);
            }
        }

        in >> count;
        in.ignore();
        for (int i = 0; i < count; ++i) {
            std::string name;
            int level;
            in >> name >> level;
            resources.emplace_back(name, level);
        }

        std::cout << "Данные успешно загружены из файла: " << filename << "\n";
    }
};

// === Реализация методов класса User ===

User::User(const std::string& name, int id, int accessLevel)
    : name(name), id(id), accessLevel(accessLevel)
{
    if (name.empty()) throw std::invalid_argument("Имя пользователя не может быть пустым.");
    if (accessLevel < 0) throw std::invalid_argument("Уровень доступа не может быть отрицательным.");
}

bool User::checkAccessToResource(const Resource& resource) const {
    return accessLevel >= resource.getRequiredAccessLevel();
}

void User::setName(const std::string& name) {
    this->name = name;
}

void User::setId(int id) {
    this->id = id;
}

void User::setAccessLevel(int level) {
    if (level < 0) throw std::invalid_argument("Уровень доступа не может быть отрицательным.");
    this->accessLevel = level;
}

// === Реализация методов класса Student ===

Student::Student(const std::string& name, int id, int accessLevel, const std::string& group)
    : User(name, id, accessLevel), group(group) {}

void Student::displayInfo() const {
    std::cout << "Студент: " << name << ", ID: " << id
        << ", Группа: " << group
        << ", Уровень доступа: " << getAccessLevel() << "\n";
}

// === Реализация методов класса Teacher ===

Teacher::Teacher(const std::string& name, int id, int accessLevel, const std::string& department)
    : User(name, id, accessLevel), department(department) {}

void Teacher::displayInfo() const {
    std::cout << "Преподаватель: " << name << ", ID: " << id
        << ", Кафедра: " << department
        << ", Уровень доступа: " << getAccessLevel() << "\n";
}

// === Реализация класса Administrator ===

Administrator::Administrator(const std::string& name, int id, int accessLevel)
    : User(name, id, accessLevel) {}

void Administrator::displayInfo() const {
    std::cout << "Администратор: " << name << ", ID: " << id
        << ", Уровень доступа: " << getAccessLevel() << "\n";
}

// === Реализация методов класса Resource ===

Resource::Resource(const std::string& name, int requiredAccessLevel)
    : name(name), requiredAccessLevel(requiredAccessLevel)
{
    if (name.empty()) throw std::invalid_argument("Название ресурса не может быть пустым.");
    if (requiredAccessLevel < 0) throw std::invalid_argument("Требуемый уровень доступа не может быть отрицательным.");
}

void Resource::setName(const std::string& name) {
    if (name.empty()) throw std::invalid_argument("Название ресурса не может быть пустым.");
    this->name = name;
}

void Resource::setRequiredAccessLevel(int level) {
    if (level < 0) throw std::invalid_argument("Требуемый уровень доступа не может быть отрицательным.");
    this->requiredAccessLevel = level;
}

// === Основная функция main с меню ===

int main() {
    setlocale(LC_ALL, "");

    AccessControlSystem<User> system;

    // Пример начальных данных
    system.addUser(std::make_shared<Student>("Алиса", 1, 2, "CS-101"));
    system.addUser(std::make_shared<Teacher>("Доктор Смит", 2, 5, "Информатика"));
    system.addUser(std::make_shared<Administrator>("Админ", 3, 10));

    system.addResource(Resource("Библиотека", 3));
    system.addResource(Resource("Лаборатория 101", 4));
    system.addResource(Resource("Конференц-зал", 6));

    int choice;
    do {
        std::cout << "\n=== Меню ===\n";
        std::cout << "1. Вывести всех пользователей\n";
        std::cout << "2. Вывести все ресурсы\n";
        std::cout << "3. Проверить доступ пользователя ко всем ресурсам\n";
        std::cout << "4. Сохранить данные в файл\n";
        std::cout << "5. Загрузить данные из файла\n";
        std::cout << "0. Выход\n";
        std::cout << "Выберите действие: ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            system.displayAllUsers();
            break;
        case 2:
            system.displayResources();
            break;
        case 3: {
            std::cin.ignore();
            std::string name;
            std::cout << "Введите полное имя пользователя: ";
            std::getline(std::cin, name);

            auto results = system.findUserExact(name);
            if (results.empty()) {
                std::cout << "Пользователь не найден.\n";
            }
            else {
                for (const auto& user : results) {
                    system.checkAccessForResource(*user);
                }
            }
            break;
        }
        case 4:
            try {
                system.saveToFile("university_data.txt");
            }
            catch (const std::exception & e) {
                std::cerr << "Ошибка при сохранении: " << e.what() << "\n";
            }
            break;
        case 5:
            try {
                system.loadFromFile("university_data.txt");
            }
            catch (const std::exception & e) {
                std::cerr << "Ошибка при загрузке: " << e.what() << "\n";
            }
            break;
        case 0:
            std::cout << "Выход из программы.\n";
            break;
        default:
            std::cout << "Неверный выбор. Попробуйте снова.\n";
        }
    } while (choice != 0);

    return 0;
}
