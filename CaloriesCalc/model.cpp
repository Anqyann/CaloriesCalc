#include "model.h"
#include <iostream>

#include "user.h"
#include "dish.h"
#include "ingredient.h"
#include "target.h"
#include "meal_log.h"
#include <windows.h>


std::unique_ptr<pqxx::connection> connect_db() {
    try {
        std::string conn_string = "dbname=CaloriesCalc user=postgres "
            "password=3188 host=localhost port=5432";

        auto C = std::make_unique<pqxx::connection>(conn_string);
        if (C->is_open()) {
            std::cout << "Connected to database: " << C->dbname() << std::endl;
        }
        else {
            std::cerr << "Could not open database" << std::endl;
            return nullptr;
        }
        return C;
    }
    catch (const std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << std::endl;
        return nullptr;
    }
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
}