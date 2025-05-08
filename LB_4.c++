#include <iostream>
#include <memory>
#include <vector>

class Inventory {
private:
    std::vector<std::unique_ptr<std::string>> items; // Вектор умных указателей на строки

public:
    // Метод для добавления предмета
    void addItem(const std::string& item) {
        items.push_back(std::make_unique<std::string>(item));
    }

    // Метод для отображения инвентаря
    void displayInventory() const {
        std::cout << "Inventory:\n";
        for (const auto& item : items) {
            std::cout << "- " << *item << std::endl; // Разыменовываем указатель для получения строки
        }
    }
};

int main() {
    Inventory inventory;

    // Добавляем предметы в инвентарь
    inventory.addItem("Sword");
    inventory.addItem("Health Potion");
    inventory.addItem("Bow");

    // Отображаем инвентарь
    inventory.displayInventory();

    return 0;
}