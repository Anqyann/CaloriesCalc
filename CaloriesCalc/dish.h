#ifndef DISH_H
#define DISH_H

#include <string>
#include <vector>
#include <pqxx/pqxx>
#include "model.h"

struct Ingredient_data_ {
    int id;
    double grams;
};

struct Dish_data {
    int id;
    std::string name;
};

class Dish {
public:
    static bool create_dish(pqxx::work& W, int user_id, const std::string& dish_name, 
        const std::vector<Ingredient_data_>& ingredients);

    static std::vector<Dish_data> find_by_name(pqxx::work& W, int user_id, const std::string& name_query);

    static Nutrition_info get_nutrition_per_gram(pqxx::work& W, int dish_id);
};

#endif //DISH_H