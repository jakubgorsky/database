#include <iostream>
#include <sqlite3/sqlite3.h>
#include "DBHandler.h"

int main() {
    sqlite3 *db = nullptr;
    std::string name, price, qty;

    DBHandler DB(db, "../DB/identifier.sqlite");
//    std::cout << "Provide data: \n";
//    std::cin >> name >> price >> qty;
//    std::cout << DB.Insert(name, std::stod(price), std::stoi(qty)).str();
    std::cout << DB.SelectAll().rdbuf();
    return 0;
}
