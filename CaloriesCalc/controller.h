#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <pqxx/pqxx>
#include <optional>
#include "view.h"
#include "user.h" 

class App_controller {
public:

    App_controller(pqxx::connection& C);

    void run();

private:
    double calculate_maintenance_calories(const User_data& user);
    double adjust_calories_for_goal(double maintenance, const std::string& type);

    void handle_main_menu();
    void handle_user_menu();

    void handle_login();
    void handle_register();
    void handle_set_target();
    void handle_add_ingredient();
    void handle_create_dish();
    void handle_log_meal();
    void handle_show_summary();

    pqxx::connection& m_conn;       
    Console_view m_view;             
    std::optional<User_data> m_current_user; 
};

#endif //CONTROLLER_H