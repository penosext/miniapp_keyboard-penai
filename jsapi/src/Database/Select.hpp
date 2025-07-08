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

class SELECT
{
private:
    sqlite3 *conn;
    std::string tableName;
    std::vector<std::string> columns;
    std::vector<std::pair<std::string, std::string>> conditions;
    std::vector<std::pair<std::string, bool>> orders;
    size_t limits = 0;
    size_t offsets = 0;

public:
    SELECT(sqlite3 *conn, std::string tableName);
    [[nodiscard]] SELECT &select(std::string column);
    [[nodiscard]] SELECT &where(std::string column, std::string value);
    [[nodiscard]] SELECT &where(std::string column, int value);
    [[nodiscard]] SELECT &order(std::string column, bool ascending);
    [[nodiscard]] SELECT &limit(size_t limits);
    [[nodiscard]] SELECT &offset(size_t offsets);
    [[nodiscard]] std::vector<std::unordered_map<std::string, std::string>> execute() const;
};
