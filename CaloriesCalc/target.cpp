#include "target.h"
#include <iostream>


bool Target::upsert_target(pqxx::connection& C, int user_id, const std::string& type,
    double target_weight, double target_calories) {
    try {
        pqxx::work W(C);

        pqxx::params p;
        p.append(user_id);
        p.append(target_weight);
        p.append(type);
        p.append(target_calories);

        W.exec(
            "INSERT INTO \"Target\" (user_id, target_weight, type, target_calories) "
            "VALUES ($1, $2, $3::target_type_enum, $4) "
            "ON CONFLICT (user_id) DO UPDATE SET "
            "target_weight = EXCLUDED.target_weight, "
            "type = EXCLUDED.type, "
            "target_calories = EXCLUDED.target_calories",p);
        W.commit();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Target::upsert_target error: " << e.what() << std::endl;
        return false;
    }
}