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

#include "Select.hpp"
#include <stdexcept>

SELECT::SELECT(sqlite3 *conn, std::string tableName) : conn(conn), tableName(tableName)
{
    ASSERT(conn != nullptr);
    ASSERT(!tableName.empty());
}
SELECT &SELECT::select(std::string column)
{
    ASSERT(!column.empty());
    this->columns.push_back(column);
    return *this;
}
SELECT &SELECT::where(std::string column, std::string value)
{
    ASSERT(!column.empty());
    ASSERT(!value.empty());
    this->conditions.push_back({column, value});
    return *this;
}
SELECT &SELECT::where(std::string column, int value) { return where(column, std::to_string(value)); }
SELECT &SELECT::order(std::string column, bool ascending)
{
    ASSERT(!column.empty());
    this->orders.push_back({column, ascending});
    return *this;
}
SELECT &SELECT::limit(size_t limits)
{
    ASSERT(limits > 0);
    this->limits = limits;
    return *this;
}
SELECT &SELECT::offset(size_t offsets)
{
    this->offsets = offsets;
    return *this;
}
void SELECT::execute(std::function<void(std::vector<std::unordered_map<std::string, std::string>>)> callback) const
{
    std::string query = "SELECT ";
    if (columns.empty())
        query += "*";
    else
    {
        for (auto &column : columns)
            query += "\"" + column + "\", ";
        query.erase(query.end() - 2, query.end());
    }
    query += " FROM \"" + tableName + "\"";
    if (!conditions.empty())
    {
        query += " WHERE ";
        for (auto &condition : conditions)
            query += "\"" + condition.first + "\"=? AND ";
        query.erase(query.end() - 5, query.end());
    }
    if (!orders.empty())
    {
        query += " ORDER BY ";
        for (auto &Order : orders)
            query += "\"" + Order.first + "\" " + (Order.second ? "ASC" : "DESC") + ", ";
        query.erase(query.end() - 2, query.end());
    }
    if (limits)
        query += " LIMIT " + std::to_string(limits);
    if (offsets)
        query += " OFFSET " + std::to_string(offsets);

    sqlite3_stmt *stmt = nullptr;
    ASSERT_DATABASE_OK(sqlite3_prepare_v2(conn, query.c_str(), -1, &stmt, nullptr));
    int idx = 1;
    for (auto &condition : conditions)
        ASSERT_DATABASE_OK(sqlite3_bind_text(stmt, idx++, condition.second.c_str(), -1, SQLITE_TRANSIENT));
    std::vector<std::unordered_map<std::string, std::string>> Data;
    int colCount = columns.empty() ? sqlite3_column_count(stmt) : columns.size();
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        std::unordered_map<std::string, std::string> Row;
        for (int i = 0; i < colCount; ++i)
        {
            std::string colName = columns.empty() ? sqlite3_column_name(stmt, i) : columns[i];
            const unsigned char *val = sqlite3_column_text(stmt, i);
            Row[colName] = val ? std::string(reinterpret_cast<const char *>(val)) : "";
        }
        Data.push_back(Row);
    }
    if (callback)
        callback(Data);
    ASSERT_DATABASE_OK(sqlite3_finalize(stmt));
}
