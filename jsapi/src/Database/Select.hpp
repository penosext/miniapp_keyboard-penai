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

#include <sqlite3.h>
#include <string>
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
    int limits = 0;
    int offsets = 0;

public:
    SELECT(sqlite3 *conn, std::string tableName);
    SELECT &select(std::string column);
    SELECT &where(std::string column, std::string value);
    SELECT &where(std::string column, int value);
    SELECT &order(std::string column, bool ascending);
    SELECT &limit(int limits);
    SELECT &offset(int offsets);
    void execute(std::function<void(std::vector<std::unordered_map<std::string, std::string>>)> callback = nullptr);
};
