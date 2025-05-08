#include <iostream>
#include <string>

class Weapon {
private:
    std::string name;
    int damage;
    float weight;

public:
    // Конструктор
    Weapon(const std::string& n, int d, float w)
        : name(n), damage(d), weight(w) {}

    // Перегрузка оператора +
    Weapon operator+(const Weapon& other) const {
        return Weapon(name + " & " + other.name, damage + other.damage, weight + other.weight);
    }

    // Перегрузка оператора >
    bool operator>(const Weapon& other) const {
        return damage > other.damage;
    }

    // Метод для отображения информации об оружии
    friend std::ostream& operator<<(std::ostream& os, const Weapon& weapon) {
        os << "Weapon: " << weapon.name << ", Damage: " << weapon.damage
           << ", Weight: " << weapon.weight << " kg";
        return os;
    }
};

int main() {
    Weapon sword("Sword", 50, 3.5);
    Weapon bow("Bow", 35, 1.5);
    Weapon axe("Axe", 65, 4.0);

    // Демонстрация перегрузки оператора +
    Weapon combinedWeapon = sword + bow;
    std::cout << combinedWeapon << std::endl; // Вывод нового оружия

    // Демонстрация перегрузки оператора >
    if (axe > sword) {
        std::cout << "Axe is stronger than Sword!" << std::endl;
    } else {
        std::cout << "Sword is stronger than or equal to Axe!" << std::endl;
    }

    return 0;
}
