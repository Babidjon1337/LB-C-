#include <iostream>
#include <vector>
#include <stdexcept>

template <typename T>
class Queue {
private:
    std::vector<T> items; // Вектор для хранения элементов очереди

public:
    // Метод для добавления элемента в очередь
    void push(const T& item) {
        items.push_back(item);
    }

    // Метод для удаления элемента из очереди
    void pop() {
        if (items.empty()) {
            throw std::out_of_range("Queue is empty, cannot pop.");
        }
        items.erase(items.begin()); // Удаляем первый элемент
    }

    // Метод для получения первого элемента
    T front() const {
        if (items.empty()) {
            throw std::out_of_range("Queue is empty, cannot access front.");
        }
        return items.front(); // Возвращаем первый элемент
    }

    // Метод для проверки, пуста ли очередь
    bool isEmpty() const {
        return items.empty();
    }

    // Метод для получения размера очереди
    size_t size() const {
        return items.size();
    }
};

int main() {
    // Пример использования очереди для строк
    Queue<std::string> stringQueue;
    stringQueue.push("First");
    stringQueue.push("Second");
    stringQueue.push("Third");

    std::cout << "String Queue:\n";
    while (!stringQueue.isEmpty()) {
        std::cout << stringQueue.front() << std::endl;
        stringQueue.pop();
    }

    // Пример использования очереди для целых чисел
    Queue<int> intQueue;
    intQueue.push(10);
    intQueue.push(20);
    intQueue.push(30);

    std::cout << "\nInteger Queue:\n";
    while (!intQueue.isEmpty()) {
        std::cout << intQueue.front() << std::endl;
        intQueue.pop();
    }

    return 0;
}