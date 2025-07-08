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
#include <stdexcept>

SIZE::SIZE(sqlite3 *conn, std::string tableName) : conn(conn), tableName(tableName)
{
    ASSERT(conn != nullptr);
    ASSERT(!tableName.empty());
}
int SIZE::execute() const
{
    std::string query = "SELECT COUNT(*) FROM \"" + tableName + "\"";
    sqlite3_stmt *stmt = nullptr;
    ASSERT_DATABASE_OK(sqlite3_prepare_v2(conn, query.c_str(), -1, &stmt, nullptr));
    ASSERT_DATABASE_OK(sqlite3_step(stmt));
    int count = sqlite3_column_int(stmt, 0);
    ASSERT_DATABASE_OK(sqlite3_finalize(stmt));
    return count;
}
