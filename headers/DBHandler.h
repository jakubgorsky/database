//
// Created by Jakub Górski on 07/08/2022.
//

#ifndef DATABASE_DBHANDLER_H
#define DATABASE_DBHANDLER_H

#include <string>
#include <sstream>
#include <iostream>

#include "sqlite3/sqlite3.h"

class DBHandler {
public:
    DBHandler(sqlite3 *db, const char* filename)
        :db(db), filename(filename){

        if(sqlite3_open(filename, &db) != SQLITE_OK){
            std::cout << "Error opening DB file\n";
            open = false;
        }
        else{
            sqlite3_close_v2(db);
            open = true;
        }
    };

    std::stringstream Insert(const std::string &ProductName, const double &Price, const int &Stock){
        std::string statement = "INSERT INTO Products (Name, Price, Qty) VALUES (" + sqlquote(ProductName) + ", " + std::to_string(Price) + ", " + std::to_string(Stock) + ");";
        return ExecQuery(statement);
    }

    std::stringstream Select(int ID){
        std::string statement = "SELECT * FROM Products WHERE ID = " + std::to_string(ID) + ";";
        return ExecQuery(statement);
    }

    std::stringstream Select(const std::string& name){
        std::string statement = "SELECT * FROM Products WHERE Name = " + sqlquote(name) + ";";
        return ExecQuery(statement);
    }

    std::stringstream Select(double Price){
        std::string statement = "SELECT * FROM Products WHERE Price = " + std::to_string(Price) + ";";
        return ExecQuery(statement);
    }

    std::stringstream Select(long Qty){
        std::string statement = "SELECT * FROM Products WHERE Qty = " + std::to_string(Qty) + ";";
        return ExecQuery(statement);
    }

    std::stringstream SelectAll(){
        std::string statement = "SELECT * FROM Products;";
        return ExecQuery(statement);
    }

    std::stringstream Update(){
        return ExecQuery(UpdateInterface());
    }
    ~DBHandler() = default;
private:
    sqlite3 *db;
    sqlite3_stmt *stmt{};
    const char* filename;
    bool open{};

    struct Product {
        int ID{};
        std::string Name;
        double Price{};
        long Qty{};
        enum memberType{
            name = 1,
            price = 2,
            qty = 3
        };
        bool isUpdated(const int &type = 0) const{
            switch(type){
                case 1:
                    return !(Name == "-");
                    break;
                case 2:
                    return !(std::to_string(Price) == "-");
                    break;
                case 3:
                    return !(std::to_string(Qty) == "-");
                    break;
                default:
                    return !(Name == "-" && std::to_string(Price) == "-" && std::to_string(Qty) == "-");
            }
        }
//        inline bool nameUpdated() const{
//            return !(Name == "-");
//        }
//        inline bool priceUpdated() const{
//            return !(std::to_string(Price) == "-");
//        }
//        inline bool qtyUpdated() const{
//            return !(std::to_string(Qty) == "-");
//        }

        ~Product() = default;
    };

    std::string UpdateInterface(){
        std::string input{};
        Product temp;
        std::cout << "\n\tDo you want to search for the items?\n"
                     "\t\t(Y) Yes\n"
                     "\t\t(N) No\n";
        std::cin >> input;
        if (input == "Y" || input == "y"){
            std::cout << "\n\tDo you want to search by ID or the name?\n"
                         "\t\t(1) ID\n"
                         "\t\t(2) Name\n";
            std::cin >> input;
            if (input == "1"){
                std::cout << "\n\tProvide the ID of the product you want to search for:\n\t\t";
                std::cin >> input;
                std::cout << Select(std::stoi(input)).rdbuf();
            }
            else if (input == "2"){
                std::cout << "\n\tProvide the Name of the product you want to search for:\n\t\t";
                std::cin >> input;
                std::cout << Select(input).rdbuf();
            }
            goto updating;
        }
        else if (input == "N" || input == "n"){
            updating:
            std::cout << "\n\tPlease, provide the ID of the product you wish to update:\n\t";
            std::cin >> temp.ID;
            std::cout << "\n\tIf you do not wish to update a field put a '-' in."
                         "\n\n\tProvide new name: \n";
            std::cin >> temp.Name;
            std::cout << "\n\tProvide new price: \n";
            std::cin >> temp.Price;
            std::cout << "\n\tProvide new quantity in stock: \n";
            std::cin >> temp.Qty;
            std::cout << "\n\tPlease, review the changes: \n"
                         "\t\tName: " + temp.Name + "\n" +
                         "\t\tPrice: " + std::to_string(temp.Price) + "\n" +
                         "\t\tQuantity: " + std::to_string(temp.Qty) + "\n" +
                         "\n\tDo you want to commit the changes? (Y/N):";
            std::cin >> input;
            if (input == "N" || input == "n")
                return "-";
            else if (input == "Y" || input == "y"){
                if (!temp.isUpdated())
                    return "-";
                std::string query = "UPDATE Products SET ";
                if(temp.isUpdated(temp.memberType::name))
                    query += "Name = " + sqlquote(temp.Name);
                if (temp.isUpdated(temp.memberType::price)){
                    query += appendComma(query) + "Price = " + std::to_string(temp.Price);
                }
                if (temp.isUpdated(temp.memberType::qty)){
                    query += appendComma(query) + "Qty = " + std::to_string(temp.Qty);
                }
                query += " WHERE ID = " + std::to_string(temp.ID) + ";";
                return query;
            }
        }
        return "-";
    }

    std::string appendComma(std::string str){
        if(!std::isspace(str[str.length()-1]))
            return ", ";
        else
            return "";
    }

    std::stringstream ExecQuery(const std::string& statement){
        std::stringstream log;
        int err{};
        if (!open){
            log << "Unable to open db.\n";
            return log;
        }
        if (statement == "-"){
            log << "[DBHANDLER] Error processing statement.";
            return log;
        }
        if(sqlite3_open_v2(filename, &db, SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK){
            log << "[SQLITE3 ERROR (" << sqlite3_errcode(db) << ")]: " << sqlite3_errmsg(db) << "\n";
            return log;
        }
        if(sqlite3_prepare(db, statement.c_str(), statement.size() - 1, &stmt, nullptr) != SQLITE_OK){
            log << "[SQLITE3 ERROR (" << sqlite3_errcode(db) << ")]: " << sqlite3_errmsg(db) << "\n";
            return log;
        }
        while(( err = sqlite3_step(stmt)) == SQLITE_ROW){
            log << sqlite3_column_int(stmt, 0) << ", "
                << sqlite3_column_text(stmt, 1) << ", $"
                << sqlite3_column_double(stmt, 2) << ", "
                << sqlite3_column_int(stmt, 3) << ";\n";
        }
        if(err != SQLITE_DONE){
            log << "[SQLITE3 ERROR (" << sqlite3_errcode(db) << ")]: " << sqlite3_errmsg(db) << "\n";
            return log;
        }
        if (sqlite3_finalize(stmt) != SQLITE_OK){
            log << "[SQLITE3 ERROR (" << sqlite3_errcode(db) << ")]: " << sqlite3_errmsg(db) << "\n";
            return log;
        }
        sqlite3_close_v2(db);
        log << "[SQLITE3 Query] Query performed successfully.\n";
        return log;
    }
    static std::string sqlquote(const std::string &value){
        return "'" + value + "'";
    }
};

#endif //DATABASE_DBHANDLER_H
