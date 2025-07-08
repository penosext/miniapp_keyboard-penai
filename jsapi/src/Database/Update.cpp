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

#include "Update.hpp"
#include <stdexcept>

UPDATE::UPDATE(sqlite3 *conn, std::string tableName) : conn(conn), tableName(tableName)
{
    ASSERT(conn != nullptr);
    ASSERT(!tableName.empty());
}
UPDATE &UPDATE::set(std::string column, std::string value)
{
    ASSERT(!column.empty());
    ASSERT(!value.empty());
    this->columns.push_back({column, value});
    return *this;
}
UPDATE &UPDATE::set(std::string column, int value) { return set(column, std::to_string(value)); }
UPDATE &UPDATE::where(std::string column, std::string value)
{
    this->conditions.push_back({column, value});
    return *this;
}
UPDATE &UPDATE::where(std::string column, int value) { return where(column, std::to_string(value)); }
void UPDATE::execute() const
{
    std::string query = "UPDATE \"" + tableName + "\" SET ";
    for (auto &column : columns)
        query += "\"" + column.first + "\"=?, ";
    query.erase(query.end() - 2, query.end());
    if (!conditions.empty())
    {
        query += " WHERE ";
        for (auto &condition : conditions)
            query += "\"" + condition.first + "\"=? AND ";
        query.erase(query.end() - 5, query.end());
    }
    sqlite3_stmt *stmt = nullptr;
    ASSERT_DATABASE_OK(sqlite3_prepare_v2(conn, query.c_str(), -1, &stmt, nullptr));
    int idx = 1;
    for (auto &column : columns)
        ASSERT_DATABASE_OK(sqlite3_bind_text(stmt, idx++, column.second.c_str(), -1, SQLITE_TRANSIENT));
    for (auto &condition : conditions)
        ASSERT_DATABASE_OK(sqlite3_bind_text(stmt, idx++, condition.second.c_str(), -1, SQLITE_TRANSIENT));
    ASSERT_DATABASE_OK(sqlite3_step(stmt));
    ASSERT_DATABASE_OK(sqlite3_finalize(stmt));
}
