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

#include "Exception.hpp"

#define THROW_NETWORK_ERROR(statusCode) throw NetworkError(__FILE__, __LINE__, statusCode)

class NetworkError : public Exception
{
public:
    NetworkError(const char *file, int line,
                 int statusCode)
        : Exception(file, line,
                    "Network error " + std::to_string(statusCode)) {}
};
