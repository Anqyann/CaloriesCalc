#ifndef TARGET_H
#define TARGET_H

#include <string>
#include <pqxx/pqxx>

class Target {
public:
    static bool upsert_target(pqxx::connection& C,
        int user_id,
        const std::string& type,
        double target_weight,
        double target_calories);
};

#endif //TARGET_H