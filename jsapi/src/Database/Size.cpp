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

#include "Size.hpp"

SIZE::SIZE(sqlite3 *conn, std::string tableName) : conn(conn), tableName(tableName) {}
void SIZE::execute(std::function<void(int)> callback)
{
    if (conn == nullptr)
        throw std::runtime_error("Not connected");
    std::string query = "SELECT COUNT(*) FROM \"" + tableName + "\"";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(conn, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error("SQLite prepare failed: " + std::string(sqlite3_errmsg(conn)));
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        count = sqlite3_column_int(stmt, 0);
    if (callback)
        callback(count);
    sqlite3_finalize(stmt);
}
