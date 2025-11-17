#ifndef VIEW_H
#define VIEW_H

#include <string>

#include "model.h"
#include "ingredient.h" 
#include "dish.h"      

std::string get_string_input(const std::string& prompt);
int get_int_input(const std::string& prompt);
double get_double_input(const std::string& prompt);
std::string get_enum_input(const std::string& prompt, const std::vector<std::string>& options);


struct Login_data {
    std::string email;
    std::string password;
};

struct New_user_data {
    std::string name, email, password, sex, activity;
    int age;
    double height, weight;
};

struct New_target_data {
    std::string type;
    double target_weight;
};

struct New_ingredient_data {
    std::string name;
    double calories, proteins, fats, carbs;
};

struct Recipe_item_input {
    std::string name_query;
    double grams;
};
struct New_dish_data {
    std::string dish_name;
    std::vector<Recipe_item_input> items;
};

struct New_log_data {
    std::string date;
    int item_type; 
    std::string search_query;
    double grams;
};

class Console_view {
public:
    int show_main_menu();
    int show_user_menu(const std::string& user_name);

    Login_data get_login_data();
    New_user_data get_registration_data();
    New_target_data get_target_data();
    New_ingredient_data get_new_ingredient_data();
    New_dish_data get_new_dish_data();
    New_log_data get_meal_log_data(pqxx::connection& C);
    std::string get_date_for_summary(pqxx::connection& C);

    int get_choice_from_ingredients(const std::vector<Ingredient_data>& ingredients);
    int get_choice_from_dishes(const std::vector<Dish_data>& dishes);

    void show_daily_summary(const Nutrition_info& total);
    void show_target_calories(const std::string& type, double maintenance, double target);

    void show_message(const std::string& message);
    void show_error(const std::string& error);
};
#endif //VIEW_H