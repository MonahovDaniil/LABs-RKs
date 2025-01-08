#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <pqxx/pqxx>
#include <windows.h>

void logOperation(const std::string& operation) {
    std::ofstream logFile("log.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << operation << std::endl;
        logFile.close();
    }
}

class Category {
public:
    void addCategory(pqxx::connection& conn, const std::string& category_name) {
        try {
            pqxx::work txn(conn);
            txn.exec("INSERT INTO categories (category_name) VALUES (" + txn.quote(category_name) + ")");
            txn.commit();
            logOperation("Added category: " + category_name);
            std::cout << "Added category" + category_name << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
};

class Product {
public:
    virtual void addProduct(pqxx::connection& conn, const std::string& name,
        float price, int quantity, int category_id) {
        try {
            pqxx::work txn(conn);
            txn.exec("INSERT INTO products (name, price, quantity, category_id) VALUES (" +
                txn.quote(name) + ", " + txn.quote(price) + ", " + txn.quote(quantity) + ", " + txn.quote(category_id) + ")");
            txn.commit();
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    virtual ~Product() = default;
};

class PhysicalProduct : public Product {
private:
    float weight;
public:
    PhysicalProduct(float weight)
        : weight(weight) {}

    void addProduct(pqxx::connection& conn, const std::string& name,
        float price, int quantity, int category_id) override {
        Product::addProduct(conn, name, price, quantity, category_id);
        std::cout << "Added physical product: Weight = " << weight
            << "kg" << std::endl;
        logOperation("\nAdded product: " + name + ", price: " + std::to_string(price) + ", quantity: " + std::to_string(quantity) + ", category ID: " + std::to_string(category_id) + ", weight: " + std::to_string(weight) + "kg");

    }

    void displayProductDetails() const {
        std::cout << "Physical Product Details: Weight = " << weight
            << "kg" << std::endl;
    }
};

class DigitalProduct : public Product {
private:
    float fileSize;
public:
    DigitalProduct(float fileSize)
        : fileSize(fileSize) {}

    void addProduct(pqxx::connection& conn, const std::string& name,
        float price, int quantity, int category_id) override {
        Product::addProduct(conn, name, price, quantity, category_id);
        std::cout << "Added digital product: File Size = " << fileSize
            << "MB" << std::endl;
        logOperation("\nAdded product: " + name + ", price: " + std::to_string(price) + ", quantity: " + std::to_string(quantity) + ", category ID: " + std::to_string(category_id) + ", file size: " + std::to_string(fileSize) + "MB");
    }

    void displayProductDetails() const {
        std::cout << "Digital Product Details: File Size = " << fileSize
            << "MB" << std::endl;
    }
};

class Sale {
public:
    void addSale(pqxx::connection& conn, int product_id, const std::string& sale_date, int quantity_sold) {
        try {
            pqxx::work txn(conn);
            txn.exec("INSERT INTO sales (product_id, sale_date, quantity_sold) VALUES (" +
                txn.quote(product_id) + ", " + txn.quote(sale_date) + ", " + txn.quote(quantity_sold) + ")");
            txn.commit();
            logOperation("\nAdded sale for product: " + std::to_string(product_id) + ", on " + sale_date + ", quantity sold: " + std::to_string(quantity_sold));
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
};

template<typename T1, typename T2>
class Pair {
public:
    T1 first;
    T2 second;

    Pair(T1 f, T2 s) : first(f), second(s) {}
};

void listProductsByCategory(pqxx::connection& conn) {
    try {
        std::string category_name;
        std::cout << "Enter the category name: ";
        std::cin.ignore();
        std::getline(std::cin, category_name);

        pqxx::work txn(conn);
        pqxx::result res = txn.exec(
            "SELECT products.id, products.name, products.price, products.quantity "
            "FROM products "
            "JOIN categories ON products.category_id = categories.id "
            "WHERE categories.category_name = " + txn.quote(category_name)
        );

        if (res.empty()) {
            std::cout << "No products found in category '" << category_name << "'.\n";
        }
        else {
            std::cout << "Products in category '" << category_name << "':\n";
            for (const auto& row : res) {
                std::cout << "ID: " << row["id"].as<int>()
                    << ", Name: " << row["name"].c_str()
                    << ", Price: " << row["price"].as<float>()
                    << ", Quantity: " << row["quantity"].as<int>() << std::endl;
                logOperation("\nProducts in " + category_name + ":\n" + "ID: " + to_string(row["id"]) + ", Name : " + to_string(row["name"]) + ", Price : " + to_string(row["price"]) + ", Quantity : " + to_string(row["quantity"]));
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void getTotalRevenue(pqxx::connection& conn) {
    try {
        pqxx::work txn(conn);
        pqxx::result res = txn.exec(
            "SELECT products.name, SUM(sales.quantity_sold * products.price) AS total_revenue "
            "FROM products "
            "JOIN sales ON products.id = sales.product_id "
            "GROUP BY products.name"
        );
        logOperation("\nTotal revenue for each product: ");
        for (const auto& row : res) {
            std::cout << "Product: " << row["name"].c_str()
                << ", Total Revenue: " << row["total_revenue"].as<float>() << std::endl;
            logOperation("Product: " + to_string(row["name"]) + ", Total Revenue: " + to_string(row["total_revenue"]));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void getTotalUnitsSold(pqxx::connection& conn) {
    try {
        pqxx::work txn(conn);
        pqxx::result res = txn.exec(
            "SELECT products.name, SUM(sales.quantity_sold) AS total_units "
            "FROM products "
            "JOIN sales ON products.id = sales.product_id "
            "GROUP BY products.name"
        );
        logOperation("\nTotal Units Sold: ");
        for (const auto& row : res) {
            std::cout << "Product: " << row["name"].c_str()
                << ", Total Units Sold: " << row["total_units"].as<int>() << std::endl;
            logOperation("Product: " + to_string(row["name"]) + ", Total Units Sold: " + to_string(row["total_units"]));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void getTop5Products(pqxx::connection& conn) {
    try {
        pqxx::work txn(conn);
        pqxx::result res = txn.exec(
            "SELECT products.name, SUM(sales.quantity_sold) AS total_sold "
            "FROM products "
            "JOIN sales ON products.id = sales.product_id "
            "GROUP BY products.name "
            "ORDER BY total_sold DESC LIMIT 5"
        );
        logOperation("\nTop 5 products: ");
        for (const auto& row : res) {
            std::cout << "Product: " << row["name"].c_str()
                << ", Total Sold: " << row["total_sold"].as<int>() << std::endl;
            logOperation("Product: " + to_string(row["name"]) + ", Total Units Sold: " + to_string(row["total_units"]));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void viewAllProducts(pqxx::connection& conn) {
    try {
        pqxx::work txn(conn);
        pqxx::result res = txn.exec(
            "SELECT products.id, products.name, products.price, products.quantity, "
            "categories.category_name "
            "FROM products "
            "JOIN categories ON products.category_id = categories.id"
        );

        std::cout << "All Products:\n";
        logOperation("\nAll products: ");
        for (const auto& row : res) {
            std::cout << "ID: " << row["id"].as<int>()
                << ", Name: " << row["name"].c_str()
                << ", Price: " << row["price"].as<float>()
                << ", Quantity: " << row["quantity"].as<int>()
                << ", Category: " << row["category_name"].c_str() << std::endl;
            logOperation("ID: " + to_string(row["id"]) + ", Name: " + to_string(row["name"]) + ", Price: " + to_string(row["price"]) + ", Quantity: " + to_string(row["quantity"]) + ", Category: " + to_string(row["category_name"]));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void viewAllSales(pqxx::connection& conn) {
    try {
        pqxx::work txn(conn);
        pqxx::result res = txn.exec(
            "SELECT sales.id, products.name AS product_name, sales.sale_date, "
            "sales.quantity_sold "
            "FROM sales "
            "JOIN products ON sales.product_id = products.id"
        );

        std::cout << "All Sales:\n";
        logOperation("\nAll sales: ");
        for (const auto& row : res) {
            std::cout << "Sale ID: " << row["id"].as<int>()
                << ", Product: " << row["product_name"].c_str()
                << ", Date: " << row["sale_date"].c_str()
                << ", Quantity Sold: " << row["quantity_sold"].as<int>() << std::endl;
            logOperation("Sale ID: " + to_string(row["id"]) + ", Product: " + to_string(row["product_name"]) + ", Date: " + to_string(row["sale_date"]) + ", Quantity Sold: " + to_string(row["quantity_sold"]));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void menu() {
    std::cout << "\nMain Menu:\n";
    std::cout << "1. Add Category\n";
    std::cout << "2. Add Physical Product\n";
    std::cout << "3. Add Digital Product\n";
    std::cout << "4. Add Sale\n";
    std::cout << "5. View All Products\n";
    std::cout << "6. View All Sales\n";
    std::cout << "7. Analytics\n";
    std::cout << "8. Exit\n";
    std::cout << "Choose an option: ";
}

void analyticsMenu() {
    std::cout << "\nAnalytics Menu:\n";
    std::cout << "1. List all products in a category\n";
    std::cout << "2. Total revenue per product\n";
    std::cout << "3. Quantity sold per product\n";
    std::cout << "4. Top 5 selling products\n";
    std::cout << "5. Back to main menu\n";
    std::cout << "Choose an option: ";
}

int main() {
    setlocale(LC_ALL, "rus");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    try {
        pqxx::connection conn("dbname=products_db user=postgres password=123456");

        while (true) {
            menu();
            int choice;
            std::cin >> choice;

            switch (choice) {
            case 1: {
                std::string category_name;
                std::cout << "Enter category name: ";
                std::cin.ignore();
                std::getline(std::cin, category_name);
                Category category;
                category.addCategory(conn, category_name);
                break;
            }
            case 2: {
                float weight;
                std::string name;
                float price;
                int quantity, category_id;
                std::cout << "Enter product name: ";
                std::cin >> name;
                std::cout << "Enter product price: ";
                std::cin >> price;
                std::cout << "Enter product quantity: ";
                std::cin >> quantity;
                std::cout << "Enter product category_id: ";
                std::cin >> category_id;
                std::cout << "Enter product weight: ";
                std::cin >> weight;
                PhysicalProduct physicalProduct(weight);
                physicalProduct.addProduct(conn, name, price, quantity, category_id);
                break;
            }
            case 3: {
                float fileSize;
                std::string name;
                float price;
                int quantity, category_id;
                std::cout << "Enter product name: ";
                std::cin >> name;
                std::cout << "Enter product price: ";
                std::cin >> price;
                std::cout << "Enter product quantity: ";
                std::cin >> quantity;
                std::cout << "Enter product category_id: ";
                std::cin >> category_id;
                std::cout << "Enter product fileSize: ";
                std::cin >> fileSize;
                DigitalProduct digitalProduct(fileSize);
                digitalProduct.addProduct(conn, name, price, quantity, category_id);
                break;
            }
            case 4: {
                int product_id, quantity_sold;
                std::string sale_date;
                std::cout << "Enter product ID: ";
                std::cin >> product_id;
                std::cout << "Enter sale date (YYYY-MM-DD): ";
                std::cin >> sale_date;
                std::cout << "Enter quantity sold: ";
                std::cin >> quantity_sold;
                Sale sale;
                sale.addSale(conn, product_id, sale_date, quantity_sold);
                break;
            }
            case 5:
                viewAllProducts(conn);
                break;
            case 6:
                viewAllSales(conn);
                break;
            case 7: {
                while (true) {
                    analyticsMenu();
                    int analyticsChoice;
                    std::cin >> analyticsChoice;

                    switch (analyticsChoice) {
                    case 1:
                        listProductsByCategory(conn);
                        break;
                    case 2:
                        getTotalRevenue(conn);
                        break;
                    case 3:
                        getTotalUnitsSold(conn);
                        break;
                    case 4:
                        getTop5Products(conn);
                        break;
                    case 5:
                        goto endAnalytics;
                    default:
                        std::cout << "Invalid choice. Try again.\n";
                    }
                }
            endAnalytics:
                break;
            }
            case 8:
                std::cout << "Exiting the program.\n";
                return 0;
            default:
                std::cout << "Invalid choice. Try again.\n";
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Connection error: " << e.what() << std::endl;
    }

    return 0;
}