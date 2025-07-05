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

#include <string>
#include <vector>

class strUtils
{
public:
    static std::string trim(const std::string &str);
    static std::string trimEnd(const std::string &str);
    static std::string trimStart(const std::string &str);

    static std::string randomId();

    static std::vector<std::string> split(const std::string &str, const std::string &delimiter);
    static std::string join(const std::vector<std::string> &vec, const std::string &delimiter);
};
