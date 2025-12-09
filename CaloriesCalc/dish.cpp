#include "dish.h"
#include "ingredient.h" 
#include <iostream>
#include <pqxx/pqxx> 

bool Dish::create_dish(pqxx::work& W, int user_id, const std::string& dish_name,
    const std::vector<Ingredient_data_>& ingredients) {
    try {
        pqxx::params p_dish; 
        p_dish.append(user_id);
        p_dish.append(dish_name);

        auto dish_res = W.exec( 
            "INSERT INTO \"Dish\" (user_id, name) VALUES ($1, $2) RETURNING dish_id", p_dish);

        int dish_id = dish_res[0]["dish_id"].as<int>(); 

        for (const auto& item : ingredients) {
            if (item.grams <= 0) {
                std::cerr << "Error: ingredient grams must be positive." << std::endl;
                return false; 
            }

            pqxx::params p_item;
            p_item.append(dish_id);
            p_item.append(item.id);
            p_item.append(item.grams);

            W.exec("INSERT INTO \"Dish_Ingredient\" (dish_id, ing_id, grams) VALUES ($1, $2, $3)", p_item);
        }
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Dish::create_dish error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Dish_data> Dish::find_by_name(pqxx::work& W, int user_id, const std::string& name_query) {
    std::vector<Dish_data> results;
    pqxx::params p;
    p.append(user_id);
    p.append("%" + name_query + "%"); 

    auto db_result = W.exec(
        "SELECT dish_id, name FROM \"Dish\" WHERE user_id = $1 AND name ILIKE $2",
        p 
    );

    for (auto row : db_result) {
        results.push_back({
            row["dish_id"].as<int>(),
            row["name"].as<std::string>()
            });
    }
    return results;
}

Nutrition_info Dish::get_nutrition_per_gram(pqxx::work& W, int dish_id) {

    pqxx::params p;
    p.append(dish_id);

    auto recipe_items = W.exec(
        "SELECT ing_id, grams FROM \"Dish_Ingredient\" WHERE dish_id = $1",
        p 
    );

    Nutrition_info total_recipe_nutrition;
    double total_recipe_weight = 0.0;

    for (auto item : recipe_items) {
        int ing_id = item["ing_id"].as<int>();
        double ing_grams = item["grams"].as<double>();

        Nutrition_info ing_nutrition_per_gram = Ingredient::get_nutrition_per_gram(W, ing_id);

        total_recipe_nutrition.kcal += ing_nutrition_per_gram.kcal * ing_grams;
        total_recipe_nutrition.proteins += ing_nutrition_per_gram.proteins * ing_grams;
        total_recipe_nutrition.fats += ing_nutrition_per_gram.fats * ing_grams;
        total_recipe_nutrition.carbs += ing_nutrition_per_gram.carbs * ing_grams;

        total_recipe_weight += ing_grams;
    }

    if (total_recipe_weight == 0) {
        return { 0, 0, 0, 0 };
    }

    return {
        total_recipe_nutrition.kcal / total_recipe_weight,
        total_recipe_nutrition.proteins / total_recipe_weight,
        total_recipe_nutrition.fats / total_recipe_weight,
        total_recipe_nutrition.carbs / total_recipe_weight
    };
}