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

class UPDATE
{
private:
    sqlite3 *conn;
    std::string tableName;
    std::vector<std::pair<std::string, std::string>> columns;
    std::vector<std::pair<std::string, std::string>> conditions;

public:
    UPDATE(sqlite3 *conn, std::string tableName);
    [[nodiscard]] UPDATE &set(std::string col, std::string value);
    [[nodiscard]] UPDATE &set(std::string column, int value);
    [[nodiscard]] UPDATE &where(std::string col, std::string value);
    [[nodiscard]] UPDATE &where(std::string column, int value);
    void execute() const;
};
