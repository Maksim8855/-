#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>
#include <map>
#include <utility>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;
using std::map;
using std::pair;

class User {
protected:
    string username;
    size_t passwordHash;

public:
    User(string name, const string &pass) : username(name) {
        passwordHash = hashPassword(pass);
    }

    User(string name, size_t hash) : username(name), passwordHash(hash) {}

    virtual void displayInfo() const {
        cout << "Пользователь: " << username << endl;
    }

    string getUsername() const { return username; }
    size_t getPasswordHash() const { return passwordHash; }

    bool checkPassword(const string &pass) const {
        return passwordHash == hashPassword(pass);
    }

    static size_t hashPassword(const string &password) {
        std::hash<string> hasher;
        return hasher(password);
    }

    virtual ~User  () = default;
};

class Buyer : public User {
public:
    Buyer(string name, const string &pass) : User(name, pass) {}

    Buyer(string name, size_t hash) : User(name, hash) {}

    void displayInfo() const override {
        cout << "Покупатель: " << username << endl;
    }
};

class Seller : public User {
public:
    Seller(string name, const string &pass) : User(name, pass) {}

    void displayInfo() const override {
        cout << "Продавец: " << username << endl;
    }
};

class Item {
private:
    static int idCounter;
    int id;
    string name;
    double price;
    string owner;

public:
    Item(string itemName, double itemPrice, const string &itemOwner)
        : id(idCounter++), name(itemName), price(itemPrice), owner(itemOwner) {}

    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    void setPrice(double newPrice) { price = newPrice; }
    string getOwner() const { return owner; }

    void setId(int newId) { id = newId; }

    void displayInfo() const {
        cout << "ID: " << id << ", Товар: " << name << ", Цена: " << price << ", Продавец: " << owner << endl;
    }
};

int Item::idCounter = 1;

class Message {
private:
    string fromUser ;
    string content;
    int itemId;

public:
    Message(const string &from, const string &msg, int id)
        : fromUser (from), content(msg), itemId(id) {}

    void display() const {
        cout << fromUser  << ": " << content << " (Товар ID: " << itemId << ")" << endl;
    }

    string getFromUser () const { return fromUser ; }
    string getContent() const { return content; }
    int getItemId() const { return itemId; }
};

class Auction {
private:
    vector<unique_ptr<Item>> items;
    map<pair<string, int>, vector<Message>> messages;

public:
    void addItem(unique_ptr<Item> item) { items.push_back(move(item)); }

    void displayItems() const {
        if (items.empty()) {
            cout << "Нет доступных товаров." << endl;
            return;
        }
        for (const auto &item : items) {
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
                    cout << "Покупатель " << buyer->getUsername() << " повысил цену на "
                         << itemName << " до " << bidPrice << endl;
                    return;
                } else {
                    cout << "Ставка слишком низкая. Текущая цена: " << item->getPrice() << endl;
                    return;
                }
            }
        }
        cout << "Товар " << itemName << " не найден." << endl;
    }

    void sendMessage(const string &from, const string &content, int itemId) {
        messages[{from, itemId}].emplace_back(from, content, itemId);
    }

    void displayMessages() const {
        cout << "Доступные чаты:\n";
        for (const auto &pair : messages) {
            cout << "Товар ID " << pair.first.second << " - " << pair.second.size() << " сообщений от " << pair.first.first << "\n";
        }
    }

    void displayChat(int itemId, const string &username) const {
        cout << "Чат для товара ID " << itemId << ":\n";
        for (const auto &pair : messages) {
            if (pair.first.second == itemId) {
                for (const auto &message : pair.second) {
                    message.display();
                }
            }
        }
    }

    void saveItemsToFile(const string &filename) const {
        std::ofstream outFile(filename);
        if (outFile.is_open()) {
            for (const auto &item : items) {
                outFile << item->getId() << "," << item->getName() << "," << item->getPrice() << ","
                        << item->getOwner() << "\n";
            }
            outFile.close();
        } else {
            cout << "Не удалось открыть файл для записи." << endl;
        }
    }

    void loadItemsFromFile(const string &filename) {
        std::ifstream inFile(filename);
        if (inFile.is_open()) {
            string line;
            while (std::getline(inFile, line)) {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                size_t pos3 = line.find(',', pos2 + 1);
                if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos) {
                    int itemId = std::stoi(line.substr(0, pos1));
                    string itemName = line.substr(pos1 + 1, pos2 - pos1 - 1);
                    double itemPrice = std::stod(line.substr(pos2 + 1, pos3 - pos2 - 1));
                    string itemOwner = line.substr(pos3 + 1);
                    addItem(make_unique<Item>(itemName, itemPrice, itemOwner));
                    items.back()->setId(itemId);
                }
            }
            inFile.close();
        } else {
            cout << "Не удалось открыть файл для чтения." << endl;
        }
    }

    void saveMessagesToFile(const string &filename) const {
        std::ofstream outFile(filename);
        if (outFile.is_open()) {
            for (const auto &pair : messages) {
                for (const auto &message : pair.second) {
                    outFile << message.getFromUser () << ","
                            << message.getContent() << ","
                            << message.getItemId() << "\n";
                }
            }
            outFile.close();
        } else {
            cout << "Не удалось открыть файл для записи сообщений." << endl;
        }
    }

    void loadMessagesFromFile(const string &filename) {
        std::ifstream inFile(filename);
        if (inFile.is_open()) {
            string line;
            while (std::getline(inFile, line)) {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                if (pos1 != string::npos && pos2 != string::npos) {
                    string fromUser  = line.substr(0, pos1);
                    string content = line.substr(pos1 + 1, pos2 - pos1 - 1);
                    int itemId = std::stoi(line.substr(pos2 + 1));
                    sendMessage(fromUser , content, itemId);
                }
            }
            inFile.close();
        } else {
            cout << "Не удалось открыть файл для чтения сообщений." << endl;
        }
    }
};

void saveUsersToFile(const vector<unique_ptr<Buyer>> &buyers, const string &filename) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        for (const auto &buyer : buyers) {
            outFile << buyer->getUsername() << "," << buyer->getPasswordHash() << "\n";
        }
        outFile.close();
    } else {
        cout << "Не удалось открыть файл для записи пользователей." << endl;
    }
}

void loadUsersFromFile(vector<unique_ptr<Buyer>> &buyers, const string &filename) {
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        string line;
        while (std::getline(inFile, line)) {
            size_t pos = line.find(',');
            if (pos != string::npos) {
                string username = line.substr(0, pos);
                size_t passwordHash = std::stoull(line.substr(pos + 1));
                buyers.push_back(make_unique<Buyer>(username, passwordHash));
            }
        }
        inFile.close();
    } else {
        cout << "Не удалось открыть файл для чтения пользователей." << endl;
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    Auction auction;
    vector<unique_ptr<Buyer>> buyers;

    loadUsersFromFile(buyers, "users.txt");
    auction.loadItemsFromFile("items.txt");
    auction.loadMessagesFromFile("messages.txt");

    while (true) {
        cout << "\nМеню:\n";
        cout << "1. Войти\n";
        cout << "2. Зарегистрироваться\n";
        cout << "3. Выход\n";
        cout << "Выберите действие: ";

        int mainChoice;
        cin >> mainChoice;

        if (mainChoice == 1) {
            string username, password;
            cout << "Введите имя пользователя: ";
            cin >> username;
            cout << "Введите пароль: ";
            cin >> password;

            bool loggedIn = false;
            for (const auto &buyer : buyers) {
                if (buyer->getUsername() == username && buyer->checkPassword(password)) {
                    loggedIn = true;
                    cout << "Вход выполнен!" << endl;

                    while (loggedIn) {
                        cout << "\nМеню:\n";
                        cout << "1. Добавить товар\n";
                        cout << "2. Просмотреть товары\n";
                        cout << "3. Сделать ставку на товар\n";
                        cout << "4. Купить товар\n";
                        cout << "5. Отправить сообщение\n";
                        cout << "6. Просмотреть чаты\n";
                        cout << "7. Выход\n";
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

                            auction.addItem(make_unique<Item>(itemName, itemPrice, username));
                            cout << "Товар добавлен!" << endl;

                        } else if (choice == 2) {
                            auction.displayItems();

                        } else if (choice == 3) {
                            string itemName;
                            double bidPrice;
                            cout << "Введите название товара для ставки: ";
                            cin >> itemName;
                            cout << "Введите вашу ставку: ";
                            cin >> bidPrice;

                            auction.bidItem(itemName, bidPrice, buyer.get());

                        } else if (choice == 4) {
                            string itemName;
                            cout << "Введите название товара для покупки: ";
                            cin >> itemName;

                            auction.buyItem(itemName, buyer.get());

                        } else if (choice == 5) {
                            int itemId;
                            string messageContent;
                            cout << "Введите ID товара, к которому хотите отправить сообщение: ";
                            cin >> itemId;
                            cout << "Введите ваше сообщение: ";
                            cin.ignore();
                            std::getline(cin, messageContent);

                            auction.sendMessage(buyer->getUsername(), messageContent, itemId);
                            cout << "Сообщение отправлено!" << endl;

                        } else if (choice == 6) {
                            auction.displayMessages();

                            int chatChoice;
                            cout << "Введите номер ID товара для открытия чата: ";
                            cin >> chatChoice;

                            auction.displayChat(chatChoice, buyer->getUsername());

                        } else if (choice == 7) {
                            loggedIn = false;
                        } else {
                            cout << "Неверный выбор. Попробуйте снова." << endl;
                        }
                    }
                    break;
                }
            }
            if (!loggedIn) {
                cout << "Неверное имя пользователя или пароль." << endl;
            }

        } else if (mainChoice == 2) {
            string username, password;
            cout << "Введите имя пользователя: ";
            cin >> username;
            cout << "Введите пароль: ";
            cin >> password;

            buyers.push_back(make_unique<Buyer>(username, password));
            cout << "Аккаунт создан!" << endl;

            saveUsersToFile(buyers, "users.txt");

        } else if (mainChoice == 3) {
            saveUsersToFile(buyers, "users.txt");
            auction.saveItemsToFile("items.txt");
            auction.saveMessagesToFile("messages.txt");
            cout << "Выход из программы." << endl;
            break;

        } else {
            cout << "Неверный выбор. Попробуйте снова." << endl;
        }
    }

    return 0;
}
