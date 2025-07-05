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

#include "Truncate.hpp"
#include <stdexcept>

TRUNCATE::TRUNCATE(sqlite3 *conn, std::string tableName) : conn(conn), tableName(tableName) {}
void TRUNCATE::execute()
{
    if (conn == nullptr)
        throw std::runtime_error("Not connected");
    std::string query = "DELETE FROM \"" + tableName + "\"";
    char *errMsg = nullptr;
    if (sqlite3_exec(conn, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::string err = errMsg ? errMsg : "Unknown error";
        sqlite3_free(errMsg);
        throw std::runtime_error("SQLite truncate failed: " + err);
    }
}
