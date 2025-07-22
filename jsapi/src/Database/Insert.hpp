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

class INSERT
{
private:
    sqlite3 *conn;
    std::string tableName;
    std::vector<std::string> columns;
    std::vector<std::string> values;

public:
    INSERT(sqlite3 *conn, std::string tableName);
    [[nodiscard]] INSERT &value(std::string column, std::string data);
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
    [[nodiscard]] INSERT &value(std::string column, T data)
    {
        return value(column, std::to_string(data));
    }
    int64_t execute() const;
};
