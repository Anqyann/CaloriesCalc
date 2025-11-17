#ifndef MODEL_H
#define MODEL_H

#include <pqxx/pqxx>
#include <memory>

std::unique_ptr<pqxx::connection> connect_db();

struct Nutrition_info {
    double kcal = 0.0;
    double proteins = 0.0;
    double fats = 0.0;
    double carbs = 0.0;
};

#endif //MODEL_H