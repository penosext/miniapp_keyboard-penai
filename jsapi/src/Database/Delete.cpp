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

#include "Delete.hpp"
#include <stdexcept>

DELETE::DELETE(sqlite3 *conn, std::string tableName) : conn(conn), tableName(tableName) {}
DELETE &DELETE::where(std::string column, std::string value)
{
    this->conditions.push_back({column, value});
    return *this;
}
DELETE &DELETE::where(std::string column, int value)
{
    return where(column, std::to_string(value));
}
void DELETE::execute()
{
    if (conn == nullptr)
        throw std::runtime_error("Not connected");
    std::string query = "DELETE FROM \"" + tableName + "\"";
    if (!conditions.empty())
    {
        query += " WHERE ";
        for (auto &condition : conditions)
            query += "\"" + condition.first + "\"=? AND ";
        query.erase(query.end() - 5, query.end());
    }
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(conn, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error("SQLite prepare failed: " + std::string(sqlite3_errmsg(conn)));
    int idx = 1;
    for (auto &condition : conditions)
        sqlite3_bind_text(stmt, idx++, condition.second.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("SQLite delete failed: " + std::string(sqlite3_errmsg(conn)));
    }
    sqlite3_finalize(stmt);
}
