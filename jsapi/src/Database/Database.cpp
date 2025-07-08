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

#include "Database.hpp"

DATABASE::DATABASE(const std::string &filePath)
{
    if (sqlite3_open(filePath.c_str(), &conn) != SQLITE_OK && conn)
        sqlite3_close(conn);
}
DATABASE::~DATABASE()
{
    if (conn)
        sqlite3_close(conn);
}

TABLE DATABASE::table(const std::string &tableName) { return TABLE(conn, tableName); }
SELECT DATABASE::select(const std::string &tableName) { return SELECT(conn, tableName); }
INSERT DATABASE::insert(const std::string &tableName) { return INSERT(conn, tableName); }
DELETE DATABASE::remove(const std::string &tableName) { return DELETE(conn, tableName); }
UPDATE DATABASE::update(const std::string &tableName) { return UPDATE(conn, tableName); }
SIZE DATABASE::size(const std::string &tableName) { return SIZE(conn, tableName); }
