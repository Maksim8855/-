#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

class User {
protected:
    string username;

public:
    User(string name) : username(name) {}

    virtual void displayInfo() const { cout << "Пользователь: " << username << endl; }

    string getUsername() const { return username; }

    virtual ~User () = default;
};

class Buyer : public User {
public:
    Buyer(string name) : User(name) {}

    void displayInfo() const override { cout << "Покупатель: " << username << endl; }
};

class Seller : public User {
public:
    Seller(string name) : User(name) {}

    void displayInfo() const override { cout << "Продавец: " << username << endl; }
};

class Item {
private:
    string name;
    double price;
    unique_ptr<Seller> owner;

public:
    Item(string itemName, double itemPrice, unique_ptr<Seller> itemOwner)
        : name(itemName), price(itemPrice), owner(move(itemOwner)) {}

    string getName() const { return name; }
    double getPrice() const { return price; }
    void setPrice(double newPrice) { price = newPrice; }

    void displayInfo() const {
        cout << "Товар: " << name << ", Цена: " << price << endl;
    }

    Seller* getOwner() const { return owner.get(); }
};

class Auction {
private:
    vector<unique_ptr<Item>> items;

public:
    void addItem(unique_ptr<Item> item) { items.push_back(move(item)); }

    void displayItems() const {
        if (items.empty()) {
            cout << "Нет доступных товаров." << endl;
            return;
        }
        for (const auto& item : items) {
            item->displayInfo();
        }
    }

    void buyItem(const string &itemName, Buyer *buyer) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if ((*it)->getName() == itemName) {
                cout << "Покупатель " << buyer->getUsername() << " купил " << itemName
                     << " за " << (*it)->getPrice() << endl;
                items.erase(it);
                return;
            }
        }
        cout << "Товар " << itemName << " не найден." << endl;
    }

    void bidItem(const string &itemName, double bidPrice, Buyer *buyer) {
        for (auto &item : items) {
            if (item->getName() == itemName) {
                if (bidPrice > item->getPrice()) {
                    item->setPrice(bidPrice);
                    cout << "Покупатель " << buyer->getUsername() << " повысил цену на " << itemName
                         << " до " << bidPrice << endl;
                    return;
                } else {
                    cout << "Ставка слишком низкая. Текущая цена: " << item->getPrice() << endl;
                    return;
                }
            }
        }
        cout << "Товар " << itemName << " не найден." << endl;
    }
};

string generateRandomUsername() {
    static const string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    string username;
    for (int i = 0; i < 5; ++i) {
        username += characters[rand() % characters.size()];
    }
    return username;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    Auction auction;
    vector<unique_ptr<Buyer>> buyers;

    while (true) {
        cout << "\nМеню:\n";
        cout << "1. Создать аккаунт покупателя\n";
        cout << "2. Добавить товар\n";
        cout << "3. Просмотреть товары\n";
        cout << "4. Сделать ставку на товар\n";
        cout << "5. Купить товар\n";
        cout << "6. Выход\n";
        cout << "Выберите действие: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            string username;
            cout << "Введите имя покупателя: ";
            cin >> username;
            buyers.push_back(make_unique<Buyer>(username));
            cout << "Аккаунт покупателя создан!" << endl;

        } else if (choice == 2) {
            string itemName;
            double itemPrice;
            cout << "Введите название товара: ";
            cin >> itemName;
            cout << "Введите начальную цену товара: ";
            cin >> itemPrice;

            auction.addItem(make_unique<Item>(itemName, itemPrice, nullptr));
            cout << "Товар добавлен!" << endl;

        } else if (choice == 3) {
            auction.displayItems();

        } else if (choice == 4) {
            string itemName;
            double bidPrice;
            cout << "Введите название товара для ставки: ";
            cin >> itemName;
            cout << "Введите вашу ставку: ";
            cin >> bidPrice;

            if (!buyers.empty()) {
                int randomIndex = rand() % buyers.size();
                auction.bidItem(itemName, bidPrice, buyers[randomIndex].get());
            } else {
                cout << "Нет доступных покупателей для ставок." << endl;
            }

        } else if (choice == 5) {
            string itemName;
            cout << "Введите название товара для покупки: ";
            cin >> itemName;

            if (!buyers.empty()) {
                auction.buyItem(itemName, buyers.back().get());
            } else {
                cout << "Нет доступных покупателей для покупки." << endl;
            }

        } else if (choice == 6) {
            cout << "Выход из программы." << endl;
            break;

        } else {
            cout << "Неверный выбор. Попробуйте снова." << endl;
        }
    }

    return 0;
}
