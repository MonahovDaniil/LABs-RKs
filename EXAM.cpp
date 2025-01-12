#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <pqxx/pqxx>
#include <windows.h>
#include <memory>

std::string formatPrice(double price) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << price;  // Устанавливаем фиксированную точность
    return oss.str();
}

template <typename T>
class DatabaseConnection {
private:
    std::unique_ptr<pqxx::connection> conn; // Умный указатель на соединение

public:
    // Конструктор: инициализирует подключение к базе данных
    DatabaseConnection(const std::string& connectionString) {
        try {
            conn = std::make_unique<pqxx::connection>(connectionString);
            if (conn->is_open()) {
                std::cout << "Соединение с базой данных успешно установлено!" << std::endl;
            }
            else {
                std::cerr << "Не удалось подключиться к базе данных." << std::endl;
                throw std::runtime_error("Connection failed");
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка подключения: " << e.what() << std::endl;
            throw;
        }
    }

    // Деструктор: закрывает соединение и освобождает ресурсы
    ~DatabaseConnection() {
            std::cout << "Соединение с базой данных закрыто" << std::endl;
        }

    // Метод для выполнения SQL-запросов, которые возвращают данные
    std::vector<std::vector<std::string>> executeQuery(const std::string& query) {
        std::vector<std::vector<std::string>> resultData;
        try {
            pqxx::work txn(*conn);
            pqxx::result result = txn.exec(query);

            for (auto row : result) {
                std::vector<std::string> rowData;
                for (auto field : row) {
                    rowData.push_back(field.as<std::string>());
                }
                resultData.push_back(rowData);
            }
            txn.commit();
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка выполнения запроса: " << e.what() << std::endl;
        }
        return resultData;
    }

    // Метод для выполнения SQL-запросов, которые не возвращают данные
    void executeNonQuery(const std::string& query) {
        try {
            pqxx::work txn(*conn);
            txn.exec(query);
            txn.commit();
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка выполнения запроса: " << e.what() << std::endl;
        }
    }

    // Методы для управления транзакциями
    void beginTransaction() {
        try {
            pqxx::work txn(*conn);
            txn.exec("BEGIN;");
            txn.commit();
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка начала транзакции: " << e.what() << std::endl;
        }
    }

    void commitTransaction() {
        try {
            pqxx::work txn(*conn);
            txn.exec("COMMIT;");
            txn.commit();
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка фиксации транзакции: " << e.what() << std::endl;
        }
    }

    void rollbackTransaction() {
        try {
            pqxx::work txn(*conn);
            txn.exec("ROLLBACK;");
            txn.commit();
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка отката транзакции: " << e.what() << std::endl;
        }
    }

    // Метод для создания хранимой процедуры
    void createFunction(const std::string& functionDefinition) {
        try {
            pqxx::work txn(*conn);
            txn.exec(functionDefinition);
            txn.commit();
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка создания функции: " << e.what() << std::endl;
        }
    }

    // Метод для создания триггера
    void createTrigger(const std::string& triggerDefinition) {
        try {
            pqxx::work txn(*conn);
            txn.exec(triggerDefinition);
            txn.commit();
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка создания триггера: " << e.what() << std::endl;
        }
    }

    // Метод для проверки состояния транзакции
    std::string getTransactionStatus(int id) {
        try {
            pqxx::work txn(*conn);
            pqxx::result result = txn.exec("select txid_status(" + id + ")");
            txn.commit();
            if (!result.empty()) {
                return result[0][0].as<std::string>();
            }
            else {
                return "Не удалось получить статус транзакции.";
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка получения статуса транзакции: " << e.what() << std::endl;
            return "Ошибка.";
        }
    }
};


void logaction(const std::string& action) {
    std::ofstream logfile("log.txt", std::ios::app);
    if (logfile.is_open()) {
        logfile << action << std::endl;
    }
}

class user {
protected:
    std::shared_ptr<DatabaseConnection<int>> dbConnection;

public:
    user(const std::string& connectionString)
        : dbConnection(std::make_shared<DatabaseConnection<int>>(connectionString)) {}

    virtual ~user() = default;

    virtual void createorder(int userid, const std::vector<std::pair<int, int>>& products) {
        try {
            dbConnection->beginTransaction();
            std::string orderquery = "insert into orders (user_id, status, total_price, order_date) values (" +
                std::to_string(userid) + ", 'pending', 0, now()) returning order_id";
            auto result = dbConnection->executeQuery(orderquery);
            int orderid = std::stoi(result[0][0]);

            double totalprice = 0;
            for (const auto& product : products) {
                int productid = product.first;
                int quantity = product.second;

                std::string productquery = "select price, stock_quantity from products where product_id = " + std::to_string(productid);
                auto productresult = dbConnection->executeQuery(productquery);
                if (productresult.empty() || std::stoi(productresult[0][1]) < quantity) {
                    throw std::runtime_error("insufficient stock for product id: " + std::to_string(productid));
                }

                double price = std::stod(productresult[0][0]);
                totalprice += price * quantity;

                std::string orderitemquery = "insert into order_items (order_id, product_id, quantity, price) values (" +
                    std::to_string(orderid) + ", " + std::to_string(productid) + ", " +
                    std::to_string(quantity) + ", " + std::to_string(price) + ")";
                dbConnection->executeNonQuery(orderitemquery);

                std::string updatestockquery = "update products set stock_quantity = stock_quantity - " +
                    std::to_string(quantity) + " where product_id = " + std::to_string(productid);
                dbConnection->executeNonQuery(updatestockquery);
            }

            std::string updateorderquery = "update orders set total_price = " + std::to_string(totalprice) +
                " where order_id = " + std::to_string(orderid);
            dbConnection->executeNonQuery(updateorderquery);

            dbConnection->commitTransaction();
            logaction("user created order with id: " + std::to_string(orderid));
            std::cout << "order created successfully with id: " << orderid << "\n";
        }
        catch (const std::exception& e) {
            dbConnection->rollbackTransaction();
            std::cerr << "failed to create order: " << e.what() << "\n";
        }
    }
    virtual void vieworderstatus(int orderid) {
        std::string query = "select status from orders where order_id = " + std::to_string(orderid);
        auto result = dbConnection->executeQuery(query);
        if (!result.empty()) {
            std::cout << "order status: " << result[0][0] << "\n";
        }
        else {
            std::cout << "order not found.\n";
        }
    }
    virtual void updateorderstatus(int orderid, const std::string& newstatus) {
        try {
            dbConnection->beginTransaction();
            std::string query = "update orders set status = '" + newstatus + "' where order_id = " + std::to_string(orderid);
            dbConnection->executeNonQuery(query);
            dbConnection->commitTransaction();
            std::cout << "order status updated successfully." << std::endl;
        }
        catch (const std::exception& ex) {
            dbConnection->rollbackTransaction();
            std::cerr << "failed to update order status: " << ex.what() << std::endl;
        }
    }
};

class admin : public user {
public:
    std::shared_ptr<DatabaseConnection<pqxx::connection>> dbConn =
        std::make_shared<DatabaseConnection<pqxx::connection>>("host=localhost dbname=exam_orders user=postgres password=123456");
    admin(const std::string& connectionString) : user(connectionString) {}

    void viewallorders() {
        try {
            dbConnection->beginTransaction();
            std::string query = "select order_id, user_id, status, total_price, order_date from orders";
            auto result = dbConnection->executeQuery(query);
            dbConnection->commitTransaction();

            std::cout << "all orders:" << std::endl;
            for (const auto& row : result) {
                std::cout << "order id: " << row[0]
                    << ", user id: " << row[1]
                    << ", status: " << row[2]
                    << ", total price: " << row[3]
                    << ", order date: " << row[4] << std::endl;
                logaction("order id: " + row[0] + ", user id: " + row[1] + ", status: " + row[2] + ", total price: " + row[3] + ", order date: " + row[4]);
            }
        }
        catch (const std::exception& ex) {
            dbConnection->rollbackTransaction();
            std::cerr << "failed to view all orders: " << ex.what() << std::endl;
            logaction("orders failed to view");
        }
    }

    void updateorderstatus(int orderid, const std::string& newstatus) override {
        try {
            dbConnection->beginTransaction();
            std::string query = "update orders set status = '" + newstatus + "' where order_id = " + std::to_string(orderid);
            dbConnection->executeNonQuery(query);
            dbConnection->commitTransaction();
            std::cout << "order status updated successfully." << std::endl;
            logaction("order" + std::to_string(orderid) + "status updated successfully.");
        }
        catch (const std::exception& ex) {
            dbConnection->rollbackTransaction();
            std::cerr << "failed to update order status: " << ex.what() << std::endl;
            logaction("order" + std::to_string(orderid) + "status failed to update");
        }
    }

    void addproduct(const std::string& productname, double price, int stockquantity) {
        try {
            dbConnection->beginTransaction();
            std::string query = "insert into products (name, price, stock_quantity) values ('" + productname + "', " + formatPrice(price) + ", " + std::to_string(stockquantity) + ")";
            dbConnection->executeNonQuery(query);
            dbConnection->commitTransaction();
            logaction("admin added product: " + productname);
            std::cout << "product added successfully.\n";
        }
        catch (const std::exception& ex) {
            dbConnection->rollbackTransaction();
            std::cerr << "failed to add product: " << ex.what() << std::endl;
            logaction("failed to add product " + productname);
        }
    }

    void updateproduct(int productid, double newprice, int newstockquantity) {
        try {
            dbConnection->beginTransaction();
            std::string query = "update products set price = " + formatPrice(newprice) + ", stock_quantity = " + std::to_string(newstockquantity) + " where product_id = " + std::to_string(productid);
            dbConnection->executeNonQuery(query);
            dbConnection->commitTransaction();
            logaction("admin updated product with id: " + std::to_string(productid));
            std::cout << "product updated successfully.\n";
        }
        catch (const std::exception& ex) {
            dbConnection->rollbackTransaction();
            std::cerr << "failed to update product: " << ex.what() << std::endl;
            logaction("failed to update product with ID: " + std::to_string(productid));
        }
    }

    void deleteproduct(int productid) {
        try {
            dbConnection->beginTransaction();
            std::string query = "delete from products where product_id = " + std::to_string(productid);
            dbConnection->executeNonQuery(query);
            dbConnection->commitTransaction();
            logaction("admin deleted product with id: " + std::to_string(productid));
            std::cout << "product deleted successfully.\n";
        }
        catch (const std::exception& ex) {
            dbConnection->rollbackTransaction();
            std::cerr << "failed to delete product: " << ex.what() << std::endl;
            logaction("failed to delete product with ID: " + std::to_string(productid));
        }
    }

    void adduser(const std::string& name, const std::string &email, const std::string &role) {
        try {
            dbConnection->beginTransaction();
            std::string query = "insert into users (name, email, role) values ('" + name + "', '" + email + "', '" + role + "')";
            dbConnection->executeNonQuery(query);
            dbConnection->commitTransaction();
            logaction("user " + name + " added to the database.");
        }
        catch (const std::exception& ex) {
            dbConnection->rollbackTransaction();
            std::cerr << "failed to add user: " << ex.what() << std::endl;
            logaction("failed to add user with name: " + name);
        }
    }



    void getAllUsers() {
        try {
            dbConnection->beginTransaction();
            std::string query = "SELECT user_id, name, email, role, loyalty_level FROM users";
            auto result = dbConnection->executeQuery(query);  // Выполнение запроса
            dbConnection->commitTransaction();
            if (result.empty()) {
                std::cout << "Пользователей в базе данных нет." << std::endl;
                logaction("Пользователей в базе данных нет.");
                return;
            }

            // Заголовок таблицы
            std::cout << "Список всех пользователей:" << std::endl;
            std::cout << "ID\tИмя\t\tEmail\t\tРоль\t\tУровень лояльности" << std::endl;
            std::cout << "-----------------------------------------------------------" << std::endl;

            // Вывод данных построчно
            for (const auto& row : result) {
                std::cout << row[0] << "\t" << row[1] << "\t\t" << row[2] << "\t\t"
                    << row[3] << "\t\t" << row[4] << std::endl;
                logaction("ID: " + row[0] + ", Имя: " + row[1] + ", Email: " + row[2] + ", Роль: " + row[3] + ", Уровень лояльности: " + row[4]);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка при получении списка пользователей: " << e.what() << std::endl;
            dbConnection->rollbackTransaction();
            logaction("Ошибка при получении списка пользователей");
        }
    }
};

class manager : public user {
public:
    std::shared_ptr<DatabaseConnection<pqxx::connection>> dbConn =
        std::make_shared<DatabaseConnection<pqxx::connection>>("host=localhost dbname=exam_orders user=postgres password=123456");
    manager(const std::string& connectionString) : user(connectionString) {}

    void approveorder(int orderid) {
        try {
            dbConnection->beginTransaction();
            std::string query = "update orders set status = 'completed' where order_id = " + std::to_string(orderid);
            dbConnection->executeNonQuery(query);
            dbConnection->commitTransaction();
            logaction("manager approved order with id: " + std::to_string(orderid));
            std::cout << "order approved successfully.\n";
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка при утверждении заказа: " << e.what() << std::endl;
            dbConnection->rollbackTransaction();
            logaction("Ошибка при утверждении заказа с ID: " + std::to_string(orderid));
        }
    }

    void updatestock(int productid, int newstockquantity) {
        try {
            dbConnection->beginTransaction();
            std::string query = "update products set stock_quantity = " + std::to_string(newstockquantity) + " where product_id = " + std::to_string(productid);
            dbConnection->executeNonQuery(query);
            dbConnection->commitTransaction();
            logaction("manager updated stock for product with id: " + std::to_string(productid));
            std::cout << "stock updated successfully.\n";
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка при обновлении товаров на складе: " << e.what() << std::endl;
            dbConnection->rollbackTransaction();
            logaction("Ошибка при обновлении на складе товара с ID: " + std::to_string(productid));
        }
    }

    void addProductsToOrder(int orderId, const std::vector<std::pair<int, int>>& products) {
        try {
            dbConnection->beginTransaction();  // Начинаем транзакцию

            double totalPrice = 0.0;

            for (const auto& product : products) {
                int productId = product.first;
                int quantity = product.second;

                // Проверка наличия товара на складе
                std::string checkStockQuery =
                    "SELECT stock_quantity, price FROM products WHERE product_id = " + std::to_string(productId) + ";";
                auto result = dbConnection->executeQuery(checkStockQuery);

                if (result.empty()) {
                    throw std::runtime_error("Товар с ID " + std::to_string(productId) + " не найден.");
                }

                int stockQuantity = std::stoi(result[0][0]);
                double price = std::stod(result[0][1]);

                if (stockQuantity < quantity) {
                    throw std::runtime_error("Недостаточное количество товара на складе для ID " + std::to_string(productId));
                }

                // Добавление товара в таблицу order_items
                std::string addItemQuery =
                    "INSERT INTO order_items (order_id, product_id, quantity, price) VALUES (" +
                    std::to_string(orderId) + ", " + std::to_string(productId) + ", " + std::to_string(quantity) +
                    ", " + formatPrice(price) + ");";
                dbConnection->executeNonQuery(addItemQuery);
                logaction("Added product with ID: " + std::to_string(productId) + ", quantity: " + std::to_string(quantity) +
                    ", price: " + formatPrice(price) + " at order with ID: " + std::to_string(orderId));

                // Обновление остатка товара на складе
                std::string updateStockQuery =
                    "UPDATE products SET stock_quantity = stock_quantity - " + std::to_string(quantity) +
                    " WHERE product_id = " + std::to_string(productId) + ";";
                dbConnection->executeNonQuery(updateStockQuery);

                // Увеличение итоговой цены заказа
                totalPrice += price * quantity;
            }

            // Обновление итоговой цены в таблице orders
            std::string updateOrderQuery =
                "UPDATE orders SET total_price = total_price + " + formatPrice(totalPrice) +
                " WHERE order_id = " + std::to_string(orderId) + ";";
            dbConnection->executeNonQuery(updateOrderQuery);

            dbConnection->commitTransaction();  // Фиксируем транзакцию
            std::cout << "Товары успешно добавлены в заказ с ID " << orderId << std::endl;
        }
        catch (const std::exception& e) {
            dbConnection->rollbackTransaction();  // Откатываем транзакцию в случае ошибки
            std::cerr << "Ошибка при добавлении товаров в заказ: " << e.what() << std::endl;
            logaction("Ошибка при добавлении товаров в заказ с ID: " + std::to_string(orderId));
        }
    }

    void cancelOrder(int userid, int orderId) {
        try {
            dbConnection->beginTransaction();
            std::string query = "UPDATE orders SET status = 'canceled' WHERE order_id = " + std::to_string(orderId) + " AND user_id = " + std::to_string(userid);
            dbConnection->executeNonQuery(query);
            dbConnection->commitTransaction();
            std::cout << "Заказ #" << orderId << " отменен." << std::endl;
            logaction("Заказ #" + std::to_string(orderId) + " отменен.");
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка при отмене заказа: " << e.what() << std::endl;
            dbConnection->rollbackTransaction();
            logaction("Ошибка при чтении заказа");
        }
    }

    void returnOrder(int userid, int orderId) {
        try {
            // Проверка возможности возврата заказа с помощью функции can_return_order
            dbConnection->beginTransaction();
            std::string query = "SELECT can_return_order(" + std::to_string(orderId) + ")";
            auto result = dbConnection->executeQuery(query);
            dbConnection->commitTransaction();

            if (!result.empty() && result[0][0] == "t") { // Если возврат возможен
                dbConnection->beginTransaction();
                std::string updateQuery = "UPDATE orders SET status = 'returned' WHERE order_id = " + std::to_string(orderId) + " AND user_id = " + std::to_string(userid);
                dbConnection->executeNonQuery(updateQuery);
                dbConnection->commitTransaction();
                std::cout << "Заказ #" << orderId << " возвращен." << std::endl;
                logaction("Заказ #" + std::to_string(orderId) + " возвращен.");
            }
            else {
                std::cerr << "Ошибка: возврат невозможен, срок возврата истек." << std::endl;
                dbConnection->rollbackTransaction();
                logaction("Ошибка: возврат заказа с ID: " + std::to_string(orderId) + "невозможен, срок возврата истек.");
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка при возврате заказа: " << e.what() << std::endl;
            logaction("Ошибка при возврате заказа с ID: " + std::to_string(orderId));
            dbConnection->rollbackTransaction();
        }
    }
};

class customer : public user {
public:
    std::shared_ptr<DatabaseConnection<pqxx::connection>> dbConn =
        std::make_shared<DatabaseConnection<pqxx::connection>>("host=localhost dbname=exam_orders user=postgres password=123456");
    customer(const std::string& connectionString) : user(connectionString) {}

    void addtoorder(int orderid, int productid, int quantity) {
        try {
            dbConnection->beginTransaction();

            // Получаем цену товара из таблицы products
            std::string query = "SELECT price FROM products WHERE product_id = " + std::to_string(productid);
            auto result = dbConnection->executeQuery(query);

            // Проверяем, что товар найден
            if (result.empty()) {
                throw std::runtime_error("Товар с ID " + std::to_string(productid) + " не найден.");
            }

            // Извлекаем цену товара из результата
            double price = std::stod(result[0][0]);

            dbConnection->beginTransaction();
            std::string insertquery = "insert into order_items (order_id, product_id, quantity, price) values (" + std::to_string(orderid) + ", " + std::to_string(productid) + ", " + std::to_string(quantity) + ", " + formatPrice(price) + ")";
            dbConnection->executeNonQuery(insertquery);
            dbConnection->commitTransaction();
            logaction("customer added product to order with id: " + std::to_string(orderid));

            // пересчитываем итоговую стоимость заказа
            dbConnection->beginTransaction();
            std::string updatequery = "update orders set total_price = (select sum(price * quantity) from order_items where order_id = " +
                std::to_string(orderid) + ") where order_id = " + std::to_string(orderid);
            dbConnection->executeNonQuery(updatequery);
            dbConnection->commitTransaction();
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка при добавлении товара в заказ: " << e.what() << std::endl;
            logaction("Ошибка при добавлении в заказ с ID: " + std::to_string(orderid) + " товара с ID: " + std::to_string(productid));
            dbConnection->rollbackTransaction();
        }
    }

    void removefromorder(int orderid, int productid) {
        try {
            dbConnection->beginTransaction();

            // удаляем товар из заказа
            std::string query = "delete from order_items where order_id = " + std::to_string(orderid) +
                " and product_id = " + std::to_string(productid);
            dbConnection->executeNonQuery(query);

            // пересчитываем итоговую стоимость заказа
            query = "update orders set total_price = (select sum(price * quantity) from order_items where order_id = " +
                std::to_string(orderid) + ") where order_id = " + std::to_string(orderid);
            dbConnection->executeNonQuery(query);

            dbConnection->commitTransaction();
            std::cout << "product removed from order successfully." << std::endl;
            logaction("product with ID: " + std::to_string(productid) + " removed from order with ID: " + std::to_string(orderid) + " successfully");
        }
        catch (const std::exception& ex) {
            dbConnection->rollbackTransaction();
            std::cerr << "failed to remove product from order: " << ex.what() << std::endl;
            logaction("failed to remove product with ID: " + std::to_string(productid) + "from order wit ID: " + std::to_string(orderid));
        }
    }

    void makepayment(int orderid) {
        try {
            dbConnection->beginTransaction();

            // проверяем наличие заказа и его текущий статус
            std::string query = "select status from orders where order_id = " + std::to_string(orderid);
            auto result = dbConnection->executeQuery(query);

            if (result.empty()) {
                std::cerr << "order not found." << std::endl;
                dbConnection->rollbackTransaction();
                return;
            }

            if (result[0][0] == "completed") {
                std::cerr << "order is already completed." << std::endl;
                dbConnection->rollbackTransaction();
                return;
            }

            // обновляем статус заказа на 'completed'
            query = "update orders set status = 'completed' where order_id = " + std::to_string(orderid);
            dbConnection->executeNonQuery(query);

            dbConnection->commitTransaction();
            std::cout << "order payment completed successfully." << std::endl;
            logaction("order with ID: " + std::to_string(orderid) + " payment completed successfuly");
        }
        catch (const std::exception& ex) {
            dbConnection->rollbackTransaction();
            std::cerr << "failed to complete payment: " << ex.what() << std::endl;
            logaction("failed to complete payment for order with ID: " + std::to_string(orderid));
        }
    }

    void createorder(int userid, const std::vector<std::pair<int, int>>& products) override {
        try {
            dbConnection->beginTransaction();
            std::string orderquery = "insert into orders (user_id, status, total_price, order_date) values (" +
                std::to_string(userid) + ", 'pending', 0, now()) returning order_id";
            auto result = dbConnection->executeQuery(orderquery);
            int orderid = std::stoi(result[0][0]);

            double totalprice = 0;
            for (const auto& product : products) {
                int productid = product.first;
                int quantity = product.second;

                std::string productquery = "select price, stock_quantity from products where product_id = " + std::to_string(productid);
                auto productresult = dbConnection->executeQuery(productquery);
                if (productresult.empty() || std::stoi(productresult[0][1]) < quantity) {
                    throw std::runtime_error("insufficient stock for product id: " + std::to_string(productid));
                }

                double price = std::stod(productresult[0][0]);
                totalprice += price * quantity;

                std::string orderitemquery = "insert into order_items (order_id, product_id, quantity, price) values (" +
                    std::to_string(orderid) + ", " + std::to_string(productid) + ", " +
                    std::to_string(quantity) + ", " + formatPrice(price) + ")";
                dbConnection->executeNonQuery(orderitemquery);
                logaction("New product ID at created order: " + std::to_string(productid) + ", in quantity: " + std::to_string(quantity) + ", with price: " + formatPrice(price));

                std::string updatestockquery = "update products set stock_quantity = stock_quantity - " +
                    std::to_string(quantity) + " where product_id = " + std::to_string(productid);
                dbConnection->executeNonQuery(updatestockquery);
            }

            std::string updateorderquery = "update orders set total_price = " + formatPrice(totalprice) +
                " where order_id = " + std::to_string(orderid);
            dbConnection->executeNonQuery(updateorderquery);

            dbConnection->commitTransaction();
            logaction("user created order with id: " + std::to_string(orderid));
            std::cout << "order created successfully with id: " << orderid << "\n";
        }
        catch (const std::exception& e) {
            dbConnection->rollbackTransaction();
            std::cerr << "failed to create order: " << e.what() << "\n";
            logaction("failed to create order");
        }
    }

    void vieworderstatus(int orderid) override {
        try {
            dbConnection->beginTransaction();
            std::string query = "select status from orders where order_id = " + std::to_string(orderid);
            auto result = dbConnection->executeQuery(query);
            dbConnection->commitTransaction();
            if (!result.empty()) {
                std::cout << "order status: " << result[0][0] << "\n";
                logaction("order with ID: " + std::to_string(orderid) + " is " + result[0][0]);
            }
            else {
                std::cout << "order not found.\n";
                logaction("order with ID: " + std::to_string(orderid) + " not found");
            }
        }
        catch (const std::exception& e) {
            dbConnection->rollbackTransaction();
            std::cerr << "failed to view order status: " << e.what() << "\n";
            logaction("failed to view order status with ID: " + std::to_string(orderid));
        }
    }

};


void showAdminMenu(const std::string &connectionString, admin admin) {
    while (true) {
        std::cout << "\n====== Меню администратора ======" << std::endl;
        std::cout << "1. Добавить новый продукт" << std::endl;
        std::cout << "2. Обновить информацию о продукте" << std::endl;
        std::cout << "3. Удалить продукт" << std::endl;
        std::cout << "4. Просмотр всех заказов" << std::endl;
        std::cout << "5. Просмотр всех пользователей" << std::endl;
        std::cout << "6. Изменить статус заказа" << std::endl;
        std::cout << "7. Выход" << std::endl;
        std::cout << "Выберите действие: ";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            std::string productname;
            double price;
            int stockquantity;
            std::cout << "Введите название продукта: ";
            std::cin.ignore();
            std::getline(std::cin, productname);
            std::cout << "Введите цену продукта: ";
            std::cin >> price;
            std::cout << "Введите количество товара на складе: ";
            std::cin >> stockquantity;
            admin.addproduct(productname, price, stockquantity);
        }

        else if (choice == 2) {
            int productid, newstockquantity;
            double newprice;
            std::cout << "Введите ID продукта: ";
            std::cin >> productid;
            std::cout << "Введите новую цену продукта: ";
            std::cin >> newprice;
            std::cout << "Введите новое количество товара на складе: ";
            std::cin >> newstockquantity;
            admin.updateproduct(productid, newprice, newstockquantity);
        }
        else if (choice == 3) {
            int productid;
            std::cout << "Введите ID продукта: ";
            std::cin >> productid;
            admin.deleteproduct(productid);
        }
        else if (choice == 4) {
            admin.viewallorders();
        }
        else if (choice == 5) {
            admin.getAllUsers();
        }
        else if (choice == 6) {
            int orderId;
            std::string newStatus;
            std::cout << "Введите ID заказа: ";
            std::cin >> orderId;
            std::cout << "Введите новый статус: ";
            std::cin >> newStatus;
            admin.updateorderstatus(orderId, newStatus);
        }
        else if (choice == 7) {
            std::cout << "Выход из меню администратора." << std::endl;
            break;
        }
        else {
            std::cout << "Неверный выбор. Попробуйте снова." << std::endl;
        }
    }
}

void showManagerMenu(const std::string& connectionString, manager manager) {
    while (true) {
        std::cout << "\n====== Меню менеджера ======" << std::endl;
        std::cout << "1. Утвердить заказ" << std::endl;
        std::cout << "2. Обновить количество товара" << std::endl;
        std::cout << "3. Добавить товары в заказ" << std::endl;
        std::cout << "4. Отменить заказ" << std::endl;
        std::cout << "5. Вернуть заказ" << std::endl;
        std::cout << "6. Выход" << std::endl;
        std::cout << "Выберите действие: ";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            int orderId;
            std::cout << "Введите ID заказа: ";
            std::cin >> orderId;
            manager.approveorder(orderId);
        }
        else if (choice == 2) {
            int productId, quantity;
            std::cout << "Введите ID продукта: ";
            std::cin >> productId;
            std::cout << "Введите новое количество: ";
            std::cin >> quantity;
            manager.updatestock(productId, quantity);
        }
        else if (choice == 3) {
            int orderid, productid, quantity;
            char ch;
            std::vector<std::pair<int, int>> products;
            std::cout << "Введите ID заказа: ";
            std::cin >> orderid;
            do {
                std::cout << "Введите ID продукта: ";
                std::cin >> productid;

                std::cout << "Введите количество: ";
                std::cin >> quantity;

                // Добавляем пару {productId, quantity} в контейнер
                products.emplace_back(productid, quantity);

                std::cout << "Хотите добавить еще один продукт? (y/n): ";
                std::cin >> ch;

            } while (ch == 'y' || ch == 'Y');
            manager.addProductsToOrder(orderid, products);
        }
        else if (choice == 4) {
            int userid, orderid;
            std::cout << "Введите ID покупателя: ";
            std::cin >> userid;
            std::cout << "Введите ID заказа: ";
            std::cin >> orderid;
            manager.cancelOrder(userid, orderid);
        }
        else if (choice == 5) {
            int userid, orderid;
            std::cout << "Введите ID покупателя: ";
            std::cin >> userid;
            std::cout << "Введите ID заказа: ";
            std::cin >> orderid;
            manager.returnOrder(userid, orderid);
        }
        else if (choice == 6) {
            std::cout << "Выход из меню менеджера." << std::endl;
            break;
        }
        else {
            std::cout << "Неверный выбор. Попробуйте снова." << std::endl;
        }
    }
}

void showCustomerMenu(const std::string& connectionString, customer customer) {
    while (true) {
        std::cout << "\n====== Меню покупателя ======" << std::endl;
        std::cout << "1. Создать новый заказ" << std::endl;
        std::cout << "2. Добавить товар в заказ" << std::endl;
        std::cout << "3. Удалить товар из заказа" << std::endl;
        std::cout << "4. Оплатить заказ" << std::endl;
        std::cout << "5. Просмотреть статус заказа" << std::endl;
        std::cout << "6. Выход" << std::endl;
        std::cout << "Выберите действие: ";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            int userid, productid, quantity;
            char ch;
            std::vector<std::pair<int, int>> products;
            std::cout << "Введите ID пользователя: ";
            std::cin >> userid;
            do {
                std::cout << "Введите ID продукта: ";
                std::cin >> productid;

                std::cout << "Введите количество: ";
                std::cin >> quantity;

                // Добавляем пару {productId, quantity} в контейнер
                products.emplace_back(productid, quantity);

                std::cout << "Хотите добавить еще один продукт? (y/n): ";
                std::cin >> ch;

            } while (ch == 'y' || ch == 'Y');
            customer.createorder(userid, products);
        }
        else if (choice == 2) {
            int orderId, productId, quantity;
            std::cout << "Введите ID заказа: ";
            std::cin >> orderId;
            std::cout << "Введите ID продукта: ";
            std::cin >> productId;
            std::cout << "Введите количество: ";
            std::cin >> quantity;
            customer.addtoorder(orderId, productId, quantity);
        }
        else if (choice == 3) {
            int orderId, productId;
            std::cout << "Введите ID заказа: ";
            std::cin >> orderId;
            std::cout << "Введите ID продукта: ";
            std::cin >> productId;
            customer.removefromorder(orderId, productId);
        }
        else if (choice == 4) {
            int orderId;
            std::cout << "Введите ID заказа: ";
            std::cin >> orderId;
            customer.makepayment(orderId);
        }
        else if (choice == 5) {
            int orderId;
            std::cout << "Введите ID заказа: ";
            std::cin >> orderId;
            customer.vieworderstatus(orderId);
        }
        else if (choice == 6) {
            std::cout << "Выход из меню покупателя." << std::endl;
            break;
        }
        else {
            std::cout << "Неверный выбор. Попробуйте снова." << std::endl;
        }
    }
}
int main() {
    setlocale(LC_ALL, "rus");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::string connectionString = "host=localhost dbname=exam_orders user=postgres password=123456";

    while (true) {
        std::cout << "====== Internet Shop System ======" << std::endl;
        std::cout << "1. Войти как Администратор" << std::endl;
        std::cout << "2. Войти как Менеджер" << std::endl;
        std::cout << "3. Войти как Покупатель" << std::endl;
        std::cout << "4. Выход" << std::endl;
        std::cout << "Выберите роль: ";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            admin admin(connectionString);
            showAdminMenu(connectionString, admin);
        }
        else if (choice == 2) {
            manager manager(connectionString);
            showManagerMenu(connectionString, manager);
        }
        else if (choice == 3) {
            customer customer(connectionString);
            showCustomerMenu(connectionString, customer);
        }
        else if (choice == 4) {
            std::cout << "Выход из программы." << std::endl;
            break;
        }
        else {
            std::cout << "Неверный выбор. Попробуйте снова." << std::endl;
        }
    }

    return 0;
}