#include "ingredient.h"
#include <iostream>
#include <pqxx/pqxx> 

bool Ingredient::add(pqxx::connection& C, int user_id, const std::string& name,
    double cals, double p, double f, double c) {
    try {
        pqxx::work W(C);

        pqxx::params params;
        params.append(user_id);
        params.append(name);
        params.append(cals);
        params.append(p);
        params.append(f);
        params.append(c);

        W.exec(
            "INSERT INTO \"Ingredient\" (user_id, name, calories, proteins, fats, carbs) "
            "VALUES ($1, $2, $3, $4, $5, $6)",params);
        W.commit();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Ingredient::add error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Ingredient_data> Ingredient::find_by_name(pqxx::work& W, int user_id, const std::string& name_query) {
    std::vector<Ingredient_data> results;

    pqxx::params p;
    p.append(user_id);
    p.append("%" + name_query + "%");

    auto db_result = W.exec(
        "SELECT * FROM \"Ingredient\" WHERE user_id = $1 AND name ILIKE $2",p);

    for (auto row : db_result) {
        results.push_back({
            row["ing_id"].as<int>(),
            row["name"].as<std::string>(),
            row["calories"].as<double>(),
            row["proteins"].as<double>(),
            row["fats"].as<double>(),
            row["carbs"].as<double>()
            });
    }
    return results;
}

Nutrition_info Ingredient::get_nutrition_per_gram(pqxx::work& W, int ing_id) {
    try {
        pqxx::params p;
        p.append(ing_id);

        auto result_collection = W.exec( 
            "SELECT calories, proteins, fats, carbs FROM \"Ingredient\" WHERE ing_id = $1", p);

        if (result_collection.empty()) {
            return { 0,0,0,0 };
        }
        auto row = result_collection[0];

        return {
            row["calories"].as<double>(0.0) / 100.0,
            row["proteins"].as<double>(0.0) / 100.0,
            row["fats"].as<double>(0.0) / 100.0,
            row["carbs"].as<double>(0.0) / 100.0
        };
    }
    catch (...) {
        return { 0,0,0,0 };
    }
}