#include "meal_log.h"
#include "ingredient.h"
#include "dish.h"
#include <iostream>
#include <pqxx/pqxx>

bool Meal_log::add_log_item(pqxx::connection& C, int user_id, const std::string& date,
    double grams, std::optional<int> ing_id, std::optional<int> dish_id) {
    try {
        pqxx::work W(C);

        pqxx::params p;
        p.append(user_id);
        p.append(ing_id);  
        p.append(dish_id); 
        p.append(date);
        p.append(grams);

        W.exec(
            "INSERT INTO \"Meal_log\" (user_id, ing_id, dish_id, date, grams) "
            "VALUES ($1, $2, $3, $4, $5)",p);
        W.commit();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "MealLog::add_log_item error: " << e.what() << std::endl;
        return false;
    }
}

Nutrition_info Meal_log::get_summary_for_date(pqxx::connection& C, int user_id, const std::string& date) {
    Nutrition_info daily_total;
    pqxx::work W(C);
    try {
        pqxx::params p;
        p.append(user_id);
        p.append(date);


        auto logs = W.exec(
            "SELECT ing_id, dish_id, grams FROM \"Meal_log\" WHERE user_id = $1 AND date = $2",p);

        for (auto row : logs) {
            double grams_eaten = row["grams"].as<double>();
            Nutrition_info item_nutrition_per_gram;

            if (!row["ing_id"].is_null()) {
                int ing_id = row["ing_id"].as<int>();
                item_nutrition_per_gram = Ingredient::get_nutrition_per_gram(W, ing_id);
            }
            else if (!row["dish_id"].is_null()) {
                int dish_id = row["dish_id"].as<int>();
                item_nutrition_per_gram = Dish::get_nutrition_per_gram(W, dish_id);
            }

            daily_total.kcal += item_nutrition_per_gram.kcal * grams_eaten;
            daily_total.proteins += item_nutrition_per_gram.proteins * grams_eaten;
            daily_total.fats += item_nutrition_per_gram.fats * grams_eaten;
            daily_total.carbs += item_nutrition_per_gram.carbs * grams_eaten;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "MealLog::get_summary_for_date error: " << e.what() << std::endl;
    }
    return daily_total;
}