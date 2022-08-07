//
// Created by Jakub Górski on 07/08/2022.
//

#ifndef DATABASE_DBHANDLER_H
#define DATABASE_DBHANDLER_H

#include <string>
#include <sstream>

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
//    std::stringstream Update(){
//        std::stringstream log;
//        if (!open){
//            log << "Unable to open db.\n";
//            return log;
//        }
//        return log;
//    }
    ~DBHandler() = default;
private:
    sqlite3 *db;
    sqlite3_stmt *stmt{};
    const char* filename;
    bool open{};

    std::stringstream ExecQuery(const std::string& statement){
        std::stringstream log;
        int err{};
        if (!open){
            log << "Unable to open db.\n";
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
        return log;
    }
    static std::string sqlquote(const std::string &value){
        return "'" + value + "'";
    }
};

#endif //DATABASE_DBHANDLER_H
