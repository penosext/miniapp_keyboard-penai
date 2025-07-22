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
#include <nlohmann/json.hpp>
#include <chrono>

struct ConversationNode
{
    std::string id;
    enum ROLE
    {
        ROLE_USER = 0,
        ROLE_ASSISTANT = 1,
        ROLE_SYSTEM = 2,
    } role;

    enum STOP_REASON
    {
        STOP_REASON_DONE = 0,
        STOP_REASON_STOP = 1,
        STOP_REASON_LENGTH = 2,
        STOP_REASON_ERROR = 3,
        STOP_REASON_CONTENT_FILTER = 4,
        STOP_REASON_USER_STOPPED = 5,
        STOP_REASON_NONE = 6
    } stopReason;

    std::string content;
    std::string parentId;
    std::vector<std::string> childIds;
    int64_t timestamp;

    ConversationNode(std::string id, ROLE role, std::string content, std::string parentId, STOP_REASON stopReason = STOP_REASON_NONE)
        : id(id), role(role), stopReason(stopReason), content(content), parentId(parentId),
          timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                        .count())
    {
    }

    nlohmann::json toJson() const
    {
        return nlohmann::json{
            {"id", id},
            {"role", role},
            {"content", content},
            {"parentId", parentId},
            {"childIds", childIds},
            {"timestamp", timestamp},
            {"stopReason", stopReason}};
    }
};
