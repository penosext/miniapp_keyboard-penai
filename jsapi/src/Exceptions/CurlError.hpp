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
#include <curl/curl.h>

#define THROW_CURL_ERROR(errorCode) throw CurlError(__FILE__, __LINE__, errorCode)

class CurlError : public Exception
{
public:
    CurlError(const char *file, int line,
              CURLcode errorCode)
        : Exception(file, line,
                    "CURL error " + std::to_string(errorCode) + ": " +
                        std::string(curl_easy_strerror(errorCode))) {}
};
