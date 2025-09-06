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

#include <jqutil_v2/jqutil.h>
#include "Database/Database.hpp"
#include "ScanInputCallback.hpp"
#include <string>
#include <thread>

class ScanInput
{
private:
    DATABASE database;

    std::unique_ptr<std::thread> scanListenThread;
    std::string lastString;

public:
    bool initialized = false;

    ScanInput();
    void initialize(ScanInputCallback callback);
    void deinitialize();
};
