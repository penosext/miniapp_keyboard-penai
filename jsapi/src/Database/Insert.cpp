// Copyright (C) 2025 Langning Chen
//
// This file is part of miniapp.
//
// miniapp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// miniapp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with miniapp.  If not, see <https://www.gnu.org/licenses/>.

#include "Insert.hpp"

INSERT::INSERT(sqlite3 *conn, std::string tableName) : conn(conn), tableName(tableName) {}
INSERT &INSERT::insert(std::string column, std::string value)
{
    this->columns.push_back(column);
    this->values.push_back(value);
    return *this;
}
INSERT &INSERT::insert(std::string column, int value) { return insert(column, std::to_string(value)); }
void INSERT::execute(std::function<void(int)> callback)
{
    if (conn == nullptr)
        throw std::runtime_error("Not connected");
    std::string query = "INSERT INTO \"" + tableName + "\" (";
    for (auto &column : columns)
        query += "\"" + column + "\", ";
    query.erase(query.end() - 2, query.end());
    query += ") VALUES (";
    for (size_t i = 0; i < values.size(); i++)
        query += "?, ";
    query.erase(query.end() - 2, query.end());
    query += ")";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(conn, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error("SQLite prepare failed: " + std::string(sqlite3_errmsg(conn)));
    int idx = 1;
    for (auto &value : values)
        sqlite3_bind_text(stmt, idx++, value.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("SQLite insert failed: " + std::string(sqlite3_errmsg(conn)));
    }
    int lastId = (int)sqlite3_last_insert_rowid(conn);
    if (callback)
        callback(lastId);
    sqlite3_finalize(stmt);
}
