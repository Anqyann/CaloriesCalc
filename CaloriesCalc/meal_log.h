#ifndef MEALLOG_H
#define MEALLOG_H

#include <string>
#include <optional>
#include <pqxx/pqxx>
#include "model.h"

class Meal_log {
public:
    static bool add_log_item(pqxx::connection& C, int user_id, const std::string& date,
        double grams, std::optional<int> ing_id, std::optional<int> dish_id);

    static Nutrition_info get_summary_for_date(pqxx::connection& C, int user_id, const std::string& date);
};

#endif //MEALLOG_H