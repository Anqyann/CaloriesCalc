#ifndef USER_H
#define USER_H

#include <string>
#include <optional>
#include <pqxx/pqxx>

struct User_data {
    int id;
    std::string name;
    std::string email;
    int age;
    double height;
    double weight;
    std::string sex;
    std::string activity_level;
};

class User {
public:
    static std::optional<User_data> login(pqxx::connection& C, const std::string& email, 
        const std::string& password);

    static std::optional<User_data> register_user(pqxx::connection& C,
        const std::string& name,
        const std::string& email,
        const std::string& password,
        int age, double height, double weight,
        const std::string& sex,
        const std::string& activity);

    static std::optional<User_data> get_by_id(pqxx::connection& C, int user_id);
};

#endif //USER_H