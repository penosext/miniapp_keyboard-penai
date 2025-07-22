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

class DELETE
{
private:
    sqlite3 *conn;
    std::string tableName;
    std::vector<std::pair<std::string, std::string>> conditions;

public:
    DELETE(sqlite3 *conn, std::string tableName);
    [[nodiscard]] DELETE &where(std::string column, std::string value);
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
    [[nodiscard]] DELETE &where(std::string column, T data)
    {
        return where(column, std::to_string(data));
    }
    void execute() const;
};
