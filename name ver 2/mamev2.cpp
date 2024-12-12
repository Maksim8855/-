#include <iostream>
#include <string>
#include <functional>
#include <sstream>
#include <map>
#include <vector>
#include <memory>
#include <random>
#include <algorithm>

using namespace std;

class User {
protected:
    string username;
    string hashedPassword;
    bool isBuyer;

public:
    User(string name, string password, bool isBuyer) : username(name), isBuyer(isBuyer) {

        hash<string> hasher;
        size_t hash = hasher(password);
        stringstream ss;
        ss << hex << hash;
        hashedPassword = ss.str();
    }

    virtual void displayInfo() const { cout << "Пользователь: " << username << endl; }

    string getUsername() const {
        return username;
    }

    bool checkPassword(const string& password) const {
        hash<string> hasher;
        size_t hash = hasher(password);
        stringstream ss;
        ss << hex << hash;
        return ss.str() == hashedPassword;
    }

    bool isBuyerUser() const { return isBuyer; }

    virtual ~User() = default;
};

class Auction {
private:
    vector<shared_ptr<User>> users;
    vector<shared_ptr<User>> loggedUsers;
    vector<shared_ptr<User>> sellers;
    vector<shared_ptr<User>> buyers;
    vector<shared_ptr<User>> itemsOwners;
    vector<shared_ptr<Item>> items;

public:
    void addUser(shared_ptr<User> user) { users.push_back(user); }

    void addSeller(shared_ptr<User> seller) { sellers.push_back(seller); }

    void addBuyer(shared_ptr<User> buyer) { buyers.push_back(buyer); }

    void addItem(shared_ptr<Item> item) { items.push_back(item); }

    void addItemOwner(shared_ptr<User> owner) { itemsOwners.push_back(owner); }

    void displayUsers() const {
        if (users.empty()) {
            cout << "Нет зарегистрированных польхователей." << endl;
            return;
        }
        for (const auto& user : users) {
            user->displayInfo();
        }
    }

    shared_ptr<User> login(const string& username, const string& password) {
        for (auto& user : users) {
            if (user->getUsername() == username && user->checkPassword(password)) {
                loggedUsers.push_back(user);
                return user;
            }
        }
        return nullptr;
    }

    void displayItems() const {
        if (items.empty()) {
            cout << "Нет доступных товаров." << endl;
            return;
        }
        for (const auto& item : items) {
            item->displayInfo();
        }
    }

    void buyItem(const string& itemName, User* buyer) {
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

    void bidItem(const string& itemName, double bidPrice, User* buyer) {
        for (auto& item : items) {
            if (item->getName() == itemName) {
                if (bidPrice > item->getPrice()) {
                    item->setPrice(bidPrice);
                    cout << "Покупатель " << buyer->getUsername() << " повысил цену на " << itemName
                        << " до " << bidPrice << endl;
                    return;
                }
                else {
                    cout << "Ставка слишком низкая. Текущая цена: " << item->getPrice() << endl;
                    return;
                }
            }
        }
        cout << "Товар " << itemName << " не найден." << endl;
    }
};

class Item {
private:
    string name;
    double price;
    shared_ptr<User> owner;

public:
    Item(string itemName, double itemPrice, shared_ptr<User> itemOwner)
        : name(itemName), price(itemPrice), owner(itemOwner) {
    }

    string getName() const { return name; }
    double getPrice() const { return price; }
    void setPrice(double newPrice) { price = newPrice; }

    void displayInfo() const {
        cout << "Товар: " << name << ", Цена: " << price << endl;
    }

    User* getOwner() const { return owner.get(); }
};

int main() {
    Auction auction;

    while (true) {
        cout << "\nМеню:\n";
        cout << "1. Зарегистрироваться\n";
        cout << "2. Войти в систему\n";
        cout << "3. Выход\n";
        cout << "Выберите действие: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            string username, password;
            cout << "Введите имя пользователя: ";
            cin >> username;
            cout << "Введите пароль: ";
            cin >> password;
            cout << "Вы хотите быть покупателем? (1 - да, 0 - нет): ";
            int isBuyer;
            cin >> isBuyer;
            auction.addUser(make_shared<User>(username, password, isBuyer));
            cout << "Аккаунт создан!" << endl;
        }
        else if (choice == 2) {
            string username, password;
            cout << "Введите имя пользователя: ";
            cin >> username;
            cout << "Введите пароль: ";
            cin >> password;
            shared_ptr<User> user = auction.login(username, password);
            if (user != nullptr) {
                cout << "Добро пожаловать, пользователь " << user->getUsername() << "!" << endl;
                while (true) {
                    cout << "\nМеню:\n";
                    cout << "1. Добавить товар\n";
                    cout << "2. Просмотреть товары\n";
                    cout << "3. Сделать ставку на товар\n";
                    cout << "4. Купить товар\n";
                    cout << "5. Выход\n";
                    cout << "Выберите действие: ";

                    int choice;
                    cin >> choice;

                    if (choice == 1) {
                        string itemName;
                        double itemPrice;
                        cout << "Введите название товара: ";
                        cin >> itemName;
                        cout << "Введите начальную цену товара: ";
                        cin >> itemPrice;

                        auction.addItem(make_shared<Item>(itemName, itemPrice, user));
                        cout << "Товар добавлен!" << endl;
                    }
                    else if (choice == 2) {
                        auction.displayItems();
                    }
                    else if (choice == 3) {
                        if (!user->isBuyerUser()) {
                            cout << "Вы не покупатель." << endl;
                            continue;
                        }
                        string itemName;
                        double bidPrice;
                        cout << "Введите название товара для ставки: ";
                        cin >> itemName;
                        cout << "Введите вашу ставку: ";
                        cin >> bidPrice;

                        auction.bidItem(itemName, bidPrice, user.get());
                    }
                    else if (choice == 4) {
                        if (!user->isBuyerUser()) {
                            cout << "Вы не покупатель." << endl;
                            continue;
                        }
                        string itemName;
                        cout << "Введите название товара для покупки: ";
                        cin >> itemName;

                        auction.buyItem(itemName, user.get());
                    }
                    else if (choice == 5) {
                        cout << "Выход из системы." << endl;
                        break;
                    }
                    else {
                        cout << "Неверный выбор. Попробуйте снова." << endl;
                    }
                }
            }
            else {
                cout << "Неправильный логин или пароль." << endl;
            }
        }
        else if (choice == 3) {
            cout << "Выход из программы." << endl;
            break;
        }
        else {
            cout << "Неверный выбор. Попробуйте снова." << endl;
        }
    }

    return 0;
}
