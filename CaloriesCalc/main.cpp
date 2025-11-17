#include <iostream>
#include <memory>
#include <pqxx/pqxx>
#include "model.h"       
#include "controller.h" 

int main() {
    auto C_ptr = connect_db();

    if (!C_ptr) {
        std::cerr << "Database connection failed. Exiting.\n";
        return 1;
    }

    try {
        App_controller app(*C_ptr);
        app.run();

    }
    catch (const std::exception& e) {
        std::cerr << "Critical error: " << e.what() << std::endl;
    }

    std::cout << "Exiting application.\n";
    return 0;
}