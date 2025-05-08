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
        if (isEmpty()) {
            throw std::out_of_range("Queue is empty, cannot pop.");
        }
        items.erase(items.begin()); // Удаляем первый элемент
    }

    // Метод для получения первого элемента
    T front() const {
        if (isEmpty()) {
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
    Queue<int> intQueue;

    try {
        // Попробуем вызвать pop() на пустой очереди
        intQueue.pop(); // Это вызовет исключение
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: " << e.what() << std::endl; // Обработка исключения
    }

    // Добавление элементов в очередь
    intQueue.push(10);
    intQueue.push(20);

    try {
        // Попробуем получить первый элемент
        std::cout << "Front element: " << intQueue.front() << std::endl;

        // Теперь вызовем pop() для удаления элемента
        intQueue.pop();
        
        // Проверим, что осталось в очереди
        std::cout << "Front element after pop: " << intQueue.front() << std::endl;

        // Удалим оставшийся элемент
        intQueue.pop();

        // Попробуем снова вызвать pop() на пустой очереди
        intQueue.pop(); // Это снова вызовет исключение
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: " << e.what() << std::endl; // Обработка исключения
    }

    return 0;
}
