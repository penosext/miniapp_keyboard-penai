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

#include "Table.hpp"
#include <stdexcept>

TABLE::TABLE(sqlite3 *conn, std::string tableName) : conn(conn), tableName(tableName)
{
    ASSERT(conn != nullptr);
    ASSERT(!tableName.empty());
}

TABLE &TABLE::column(std::string name, ColumnType type, int options, std::string defaultValue)
{
    ASSERT(!name.empty());
    std::string columnDefinition = name + " ";
    switch (type)
    {
    case TEXT:
        columnDefinition += "TEXT";
        break;
    case INTEGER:
        columnDefinition += "INTEGER";
        break;
    case REAL:
        columnDefinition += "REAL";
        break;
    case BLOB:
        columnDefinition += "BLOB";
        break;
    default:
        ASSERT(false);
    }
    if (options & PRIMARY_KEY)
        columnDefinition += " PRIMARY KEY";
    if (options & NOT_NULL)
        columnDefinition += " NOT NULL";
    if (options & UNIQUE)
        columnDefinition += " UNIQUE";
    if (options & AUTOINCREMENT)
        columnDefinition += " AUTOINCREMENT";
    if (options & DEFAULT)
        columnDefinition += " DEFAULT '" + std::string(defaultValue) + "'";

    columns.push_back(columnDefinition);
    return *this;
}

void TABLE::execute() const
{
    std::string sql = "CREATE TABLE IF NOT EXISTS " + std::string(tableName) + " (";
    for (size_t i = 0; i < columns.size(); ++i)
    {
        sql += columns[i];
        if (i < columns.size() - 1)
            sql += ", ";
    }
    sql += ")";

    ASSERT_DATABASE_OK(sqlite3_exec(conn, sql.c_str(), nullptr, nullptr, nullptr));
}
