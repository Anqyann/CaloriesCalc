#include "view.h"
#include <iostream>
#include <limits>
#include <vector>

std::string get_string_input(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);

    while (input.empty()) {
        std::cout << "Input cannot be empty. Please try again: ";
        std::getline(std::cin, input);
    }
    return input;
}

int get_int_input(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            return value;
        }
        else {
            std::cout << "Error. Please enter a whole number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

double get_double_input(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            return value;
        }
        else {
            std::cout << "Error. Please enter a number (e.g., 55.5).\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::string get_enum_input(const std::string& prompt, const std::vector<std::string>& options) {
    std::cout << prompt << "\n";
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << options[i] << "\n";
    }
    while (true) {
        int choice = get_int_input("Your choice (1-" + std::to_string(options.size()) + "): ");
        if (choice > 0 && choice <= options.size()) {
            return options[choice - 1];
        }
        else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }
}
int Console_view::show_main_menu() {
    std::cout << "\nMAIN MENU\n";
    std::cout << "1. Login\n";
    std::cout << "2. Register\n";
    std::cout << "3. Exit\n";
    return get_int_input("Your choice: ");
}

int Console_view::show_user_menu(const std::string& user_name) {
    std::cout << "\nUSER MENU (Welcome, " << user_name << ")\n";
    std::cout << "1. Set/Update Target\n";
    std::cout << "2. Add Ingredient\n";
    std::cout << "3. Create Dish\n";
    std::cout << "4. Log Meal\n";
    std::cout << "5. Show Daily Summary\n";
    std::cout << "6. Logout\n";
    return get_int_input("Your choice: ");
}

//get data
Login_data Console_view::get_login_data() {
    std::cout << "LOGIN\n";
    std::string email = get_string_input("Email: ");
    std::string password = get_string_input("Password: ");
    return { email, password };
}

New_user_data Console_view::get_registration_data() {
    std::cout << "NEW USER REGISTRATION\n";
    New_user_data data;
    data.name = get_string_input("Name: ");
    data.email = get_string_input("Email: ");
    data.password = get_string_input("Password: ");
    data.age = get_int_input("Age: ");
    data.height = get_double_input("Height (cm): ");
    data.weight = get_double_input("Weight (kg): ");
    data.sex = get_enum_input("Sex:", { "Male", "Female" });
    data.activity = get_enum_input("Activity Level:", { "low", "medium", "high" });
    return data;
}

New_target_data Console_view::get_target_data() {
    std::cout << "SET YOUR DATA\n";
    New_target_data data;
    data.type = get_enum_input("Goal:", { "Lose weight", "Support weight", "Gain weight" });
    data.target_weight = get_double_input("Target Weight (kg): ");
    return data;
}

New_ingredient_data Console_view::get_new_ingredient_data() {
    std::cout << "ADD NEW INGREDIENT\n";
    New_ingredient_data data;
    data.name = get_string_input("Name: ");
    data.calories = get_double_input("Calories (per 100g): ");
    data.proteins = get_double_input("Proteins (per 100g): ");
    data.fats = get_double_input("Fats (per 100g): ");
    data.carbs = get_double_input("Carbs (per 100g): ");
    return data;
}

New_dish_data Console_view::get_new_dish_data() {
    std::cout << "CREATE NEW DISH RECIPE\n";
    New_dish_data data;
    data.dish_name = get_string_input("Dish Name: ");

    std::cout << "Now, add ingredients to the recipe:\n";
    while (true) {
        std::string name_query = get_string_input("Enter ingredient name (or 'done' to finish): ");
        if (name_query == "done" || name_query == "DONE") {
            break;
        }
        double grams = get_double_input("Enter grams: ");
        data.items.push_back({ name_query, grams });
    }
    return data;
}

std::string get_date_from_user(pqxx::connection& C) {
    std::string date = get_string_input("Enter date (YYYY-MM-DD, or 'today'): ");
    if (date == "today") {
        pqxx::nontransaction N(C);
        date = N.exec("SELECT CURRENT_DATE")[0][0].as<std::string>();
    }
    return date;
}

New_log_data Console_view::get_meal_log_data(pqxx::connection& C) {
    std::cout << "LOG A MEAL\n";
    New_log_data data;
    data.date = get_date_from_user(C);
    data.item_type = get_int_input("What did you eat?\n1. Ingredient\n2. Dish\nChoice: ");
    data.search_query = get_string_input("Search for item: ");
    data.grams = get_double_input("How many grams: ");
    return data;
}

std::string Console_view::get_date_for_summary(pqxx::connection& C) {
    std::cout << "Daily Summary\n";
    return get_date_from_user(C);
}


int Console_view::get_choice_from_ingredients(const std::vector<Ingredient_data>& ingredients) {
    std::cout << "Multiple ingredients found:\n";
    for (size_t i = 0; i < ingredients.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << ingredients[i].name << "\n";
    }
    int choice = get_int_input("Please select one (0 to cancel): ");
    if (choice > 0 && choice <= ingredients.size()) {
        return ingredients[choice - 1].id;
    }
    return -1;
}

int Console_view::get_choice_from_dishes(const std::vector<Dish_data>& dishes) {
    std::cout << "Multiple dishes found:\n";
    for (size_t i = 0; i < dishes.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << dishes[i].name << "\n";
    }
    int choice = get_int_input("Please select one (0 to cancel): ");
    if (choice > 0 && choice <= dishes.size()) {
        return dishes[choice - 1].id;
    }
    return -1; 
}


//show results
void Console_view::show_daily_summary(const Nutrition_info& total) {
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "---------------------------------\n";
    std::cout << "Total Consumed:\n";
    std::cout << "Calories:    " << total.kcal << " kcal\n";
    std::cout << "Proteins:    " << total.proteins << " g\n";
    std::cout << "Fats:         " << total.fats << " g\n";
    std::cout << "Carbs:        " << total.carbs << " g\n";
    std::cout << "---------------------------------\n";
}

void Console_view::show_target_calories(const std::string& type, double maintenance, double target) {
    std::cout << std::fixed << std::setprecision(0);
    std::cout << "Your maintenance calories: " << maintenance << " kcal.\n";
    std::cout << "Your new target (" << type << "): " << target << " kcal.\n";
}

//messages 

void Console_view::show_message(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

void Console_view::show_error(const std::string& error) {
    std::cerr << "[ERROR] " << error << std::endl;
}