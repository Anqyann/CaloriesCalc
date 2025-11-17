#include "user.h"
#include <iostream>


static User_data map_row_to_user(const pqxx::row& row) {
    return User_data{
        row["user_id"].as<int>(),
        row["name"].as<std::string>(),
        row["email"].as<std::string>(),
        row["age"].as<int>(),
        row["height"].as<double>(),
        row["weight"].as<double>(),
        row["sex"].as<std::string>(),
        row["activity_level"].as<std::string>()
    };
}

std::optional<User_data> User::login(pqxx::connection& C, const std::string& email, const std::string& password) {
    try {
        pqxx::nontransaction N(C);

        pqxx::params p;
        p.append(email);
        p.append(password);

        auto result = N.exec(
            "SELECT * FROM \"User\" WHERE email = $1 AND password = $2",p);

        if (result.empty()) {
            return std::nullopt;
        }
        return map_row_to_user(result[0]);
    }
    catch (const std::exception& e) {
        std::cerr << "User::login error: " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<User_data> User::register_user(pqxx::connection& C,
    const std::string& name, const std::string& email, const std::string& password,
    int age, double height, double weight,
    const std::string& sex, const std::string& activity) {
    try {
        pqxx::work W(C);

        pqxx::params p;
        p.append(name);
        p.append(email);
        p.append(password);
        p.append(age);
        p.append(height);
        p.append(weight);
        p.append(sex);
        p.append(activity);

        auto result = W.exec(
            "INSERT INTO \"User\" (name, email, password, age, height, weight, sex, activity_level) "
            "VALUES ($1, $2, $3, $4, $5, $6, $7::sex_enum, $8::activity_level_enum) "
            "RETURNING *",p);
        W.commit();

        return map_row_to_user(result[0]);
    }
    catch (const std::exception& e) {
        std::cerr << "User::register_user error: " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<User_data> User::get_by_id(pqxx::connection& C, int user_id) {
    try {
        pqxx::nontransaction N(C);

        pqxx::params p;
        p.append(user_id);

        auto result = N.exec(
            "SELECT * FROM \"User\" WHERE user_id = $1",p);

        return map_row_to_user(result[0]);
    }
    catch (const std::exception& e) {
        std::cerr << "User::get_by_id error: " << e.what() << std::endl;
        return std::nullopt;
    }
}