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

INSERT::INSERT(sqlite3 *conn, std::string tableName) : conn(conn), tableName(tableName)
{
    ASSERT(conn != nullptr);
    ASSERT(!tableName.empty());
}
INSERT &INSERT::value(std::string column, std::string data)
{
    ASSERT(!column.empty());
    this->columns.push_back(column);
    this->values.push_back(data);
    return *this;
}
INSERT &INSERT::value(std::string column, int data) { return value(column, std::to_string(data)); }
int64_t INSERT::execute() const
{
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
    ASSERT_DATABASE_OK(sqlite3_prepare_v2(conn, query.c_str(), -1, &stmt, nullptr));
    int idx = 1;
    for (auto &value : values)
        ASSERT_DATABASE_OK(sqlite3_bind_text(stmt, idx++, value.c_str(), -1, SQLITE_TRANSIENT));
    ASSERT_DATABASE_OK(sqlite3_step(stmt));
    int64_t lastId = sqlite3_last_insert_rowid(conn);
    ASSERT_DATABASE_OK(sqlite3_finalize(stmt));
    return lastId;
}
