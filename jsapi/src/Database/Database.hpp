
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

#include <sqlite3/sqlite3.h>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include "Table.hpp"
#include "Select.hpp"
#include "Insert.hpp"
#include "Delete.hpp"
#include "Update.hpp"
#include "Size.hpp"
#include "Truncate.hpp"

class DATABASE
{
private:
    sqlite3 *conn;

public:
    DATABASE(const std::string &filePath);
    ~DATABASE();

    TABLE table(const std::string &tableName);
    SELECT select(const std::string &tableName);
    INSERT insert(const std::string &tableName);
    DELETE remove(const std::string &tableName);
    UPDATE update(const std::string &tableName);
    SIZE size(const std::string &tableName);
    TRUNCATE truncate(const std::string &tableName);
};
