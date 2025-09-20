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

#include "ScanInput.hpp"
#include <unistd.h>

ScanInput::ScanInput() : database("/userdisk/database/history.db") {}

void ScanInput::initialize(ScanInputCallback callback)
{
    if (initialized)
        return;

    this->scanListenThread = std::make_unique<std::thread>(
        [this, callback]()
        {
            std::string lastString = "";
            while (this->initialized)
            {
                auto historyData = database.select("table_history")
                                       .select("word")
                                       .order("timestamp", false)
                                       .limit(1)
                                       .execute();
                if (historyData.size())
                {
                    std::string currentString = historyData[0]["word"];
                    if (currentString != lastString)
                    {
                        if (lastString != "")
                        {
                            system("miniapp_cli start 8001749644971193 softKeyboard");
                            callback(currentString);
                        }
                        lastString = currentString;
                    }
                }
                sleep(1);
            }
        });

    initialized = true;
}
void ScanInput::deinitialize()
{
    if (!initialized)
        return;

    initialized = false;
    this->scanListenThread->join();
}
