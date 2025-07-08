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

#pragma once

#include "Includes.hpp"
#include <vector>
#include <functional>
#include <unordered_map>

class TABLE
{

private:
    sqlite3 *conn;
    std::string tableName;
    std::vector<std::string> columns;

public:
    enum ColumnType
    {
        TEXT,
        INTEGER,
        REAL,
        BLOB
    };
    enum ColumnOptions
    {
        PRIMARY_KEY = 1 << 0,
        NOT_NULL = 1 << 1,
        UNIQUE = 1 << 2,
        AUTOINCREMENT = 1 << 3,
        DEFAULT = 1 << 4
    };

    TABLE(sqlite3 *conn, std::string tableName);
    TABLE &column(std::string name, ColumnType type = TEXT, int options = 0, std::string defaultValue = "");
    void execute() const;
};
