#ifndef INGREDIENT_H
#define INGREDIENT_H

#include <string>
#include <vector>
#include <pqxx/pqxx>
#include "model.h"

struct Ingredient_data {
    int id;
    std::string name;
    double calories, proteins, fats, carbs;
};

class Ingredient {
public:
    static bool add(pqxx::connection& C, int user_id, const std::string& name,
        double cals, double p, double f, double c);

    static std::vector<Ingredient_data> find_by_name(pqxx::work& W, int user_id, const std::string& name_query);

    static Nutrition_info get_nutrition_per_gram(pqxx::work& W, int ing_id);
};

#endif //INGREDIENT_H