#include "controller.h"
#include "user.h"
#include "target.h"
#include "ingredient.h"
#include "dish.h"
#include "meal_log.h"

#include <iostream>

//constructor
App_controller::App_controller(pqxx::connection& C) : m_conn(C) {}

void App_controller::run() {
    
    handle_main_menu();
}

//menu handlers
void App_controller::handle_main_menu() {
    while (1) {
        int choice = m_view.show_main_menu(); 
        switch (choice) {
        case 1: handle_login(); break;
        case 2: handle_register(); break;
        case 3: break;
        default: m_view.show_error("Invalid choice.");
        }
    }
}

void App_controller::handle_user_menu() {
    if (!m_current_user) {
        m_view.show_error("Not logged in!");
        return;
    }
    while (1) {
        int choice = m_view.show_user_menu(m_current_user->name);
        switch (choice) {
        case 1: handle_set_target(); break;
        case 2: handle_add_ingredient(); break;
        case 3: handle_create_dish(); break;
        case 4: handle_log_meal(); break;
        case 5: handle_show_summary(); break;
        case 6:
            m_current_user.reset(); 
            m_view.show_message("Logged out.");
            break;
        default: m_view.show_error("Invalid choice.");
        }
    }
}

//action handlers 
void App_controller::handle_login() {
    Login_data data = m_view.get_login_data();

    m_current_user = User::login(m_conn, data.email, data.password);

    if (m_current_user) {
        m_view.show_message("Login successful!");
        handle_user_menu(); 
    }
    else {
        m_view.show_error("Invalid email or password.");
    }
}

void App_controller::handle_register() {
    New_user_data data = m_view.get_registration_data();

    m_current_user = User::register_user(m_conn, data.name, data.email, data.password,
        data.age, data.height, data.weight,
        data.sex, data.activity);

    if (m_current_user) {
        m_view.show_message("Registration successful!");
        handle_set_target(); 
        handle_user_menu();  
    }
    else {
        m_view.show_error("Registration failed (email might be taken).");
    }
}

void App_controller::handle_set_target() {
    New_target_data data = m_view.get_target_data();

    double maintenance_cals = calculate_maintenance_calories(*m_current_user);
    double target_cals = adjust_calories_for_goal(maintenance_cals, data.type);

    bool success = Target::upsert_target(m_conn, m_current_user->id, data.type,
        data.target_weight, target_cals);

    if (success) {
        m_view.show_target_calories(data.type, maintenance_cals, target_cals);
        m_view.show_message("Target saved successfully.");
    }
    else {
        m_view.show_error("Failed to save target.");
    }
}

void App_controller::handle_add_ingredient() {
    New_ingredient_data data = m_view.get_new_ingredient_data();

    bool success = Ingredient::add(m_conn, m_current_user->id, data.name,
        data.calories, data.proteins, data.fats, data.carbs);

    if (success) {
        m_view.show_message("Ingredient '" + data.name + "' added.");
    }
    else {
        m_view.show_error("Failed to add ingredient.");
    }
}

void App_controller::handle_create_dish() {
    New_dish_data data = m_view.get_new_dish_data();
    if (data.items.empty()) {
        m_view.show_message("No ingredients added. Dish creation cancelled.");
        return;
    }

    std::vector<Ingredient_data_> valid_ingredients;
    bool validation_ok = true;

    pqxx::work W(m_conn); 
    try {
        for (const auto& item : data.items) {
            std::vector<Ingredient_data> found_ings = Ingredient::find_by_name(W, m_current_user->id, item.name_query);

            if (found_ings.empty()) {
                m_view.show_error("Ingredient matching '" + item.name_query + "' not found.");
                validation_ok = false;
                break;
            }

            int chosen_id = -1;
            if (found_ings.size() > 1) {
                chosen_id = m_view.get_choice_from_ingredients(found_ings);
            }
            else {
                chosen_id = found_ings[0].id;
            }

            if (chosen_id == -1) { 
                m_view.show_message("Ambiguous ingredient. Dish creation cancelled.");
                validation_ok = false;
                break;
            }

            valid_ingredients.push_back({ chosen_id, item.grams });
        }
        if (validation_ok) {
            bool success = Dish::create_dish(W, m_current_user->id, data.dish_name, valid_ingredients);
            if (success) {
                W.commit();
                m_view.show_message("Dish '" + data.dish_name + "' created successfully!");
            }
            else {
                
                m_view.show_error("Failed to create dish. Rolling back.");
                W.abort(); 
            }
        }
        else {
            m_view.show_message("Validation failed. Rolling back.");
            W.abort();
        }

    }
    catch (const std::exception& e) {
        m_view.show_error("A database error occurred: " + std::string(e.what()));
        W.abort();
    }
}

void App_controller::handle_log_meal() {
    New_log_data data = m_view.get_meal_log_data(m_conn);

    std::optional<int> ing_id = std::nullopt;
    std::optional<int> dish_id = std::nullopt;
    bool found = false;

    pqxx::work W(m_conn); 
    try {
        if (data.item_type == 1) { 
            auto found_ings = Ingredient::find_by_name(W, m_current_user->id, data.search_query);
            if (found_ings.empty()) { m_view.show_error("Ingredient not found."); }
            else if (found_ings.size() > 1) {
                ing_id = m_view.get_choice_from_ingredients(found_ings);
            }
            else {
                ing_id = found_ings[0].id;
            }
            if (ing_id.has_value() && ing_id.value() != -1) found = true;

        }
        else if (data.item_type == 2) {
            auto found_dishes = Dish::find_by_name(W, m_current_user->id, data.search_query);
            if (found_dishes.empty()) { m_view.show_error("Dish not found."); }
            else if (found_dishes.size() > 1) {
                dish_id = m_view.get_choice_from_dishes(found_dishes);
            }
            else {
                dish_id = found_dishes[0].id;
            }
            if (dish_id.has_value() && dish_id.value() != -1) found = true;
        }
        else {
            m_view.show_error("Invalid item type.");
            return;
        }

        W.commit(); 

    }
    catch (const std::exception& e) {
        m_view.show_error("Search failed: " + std::string(e.what()));
        W.abort();
        return;
    }

    if (found) {
        bool success = Meal_log::add_log_item(m_conn, m_current_user->id, data.date, data.grams, ing_id, dish_id);
        if (success) {
            m_view.show_message("Meal logged successfully.");
        }
        else {
            m_view.show_error("Failed to log meal.");
        }
    }
    else {
        m_view.show_message("Logging cancelled.");
    }
}

void App_controller::handle_show_summary() {
    std::string date = m_view.get_date_for_summary(m_conn);

    Nutrition_info summary = Meal_log::get_summary_for_date(m_conn, m_current_user->id, date);

    m_view.show_daily_summary(summary);
}


//controller helpers
double App_controller::calculate_maintenance_calories(const User_data& user) {
    double bmr = (10 * user.weight) + (6.25 * user.height) - (5 * user.age);
    bmr += (user.sex == "Male" ? 5 : -161);

    double activity_mult = 1.2; 
    if (user.activity_level == "medium") activity_mult = 1.55;
    if (user.activity_level == "high") activity_mult = 1.9;

    return bmr * activity_mult;
}

double App_controller::adjust_calories_for_goal(double maintenance, const std::string& type) {
    if (type == "Lose weight") return maintenance - 500;
    if (type == "Gain weight") return maintenance + 500;
    return maintenance; 
}