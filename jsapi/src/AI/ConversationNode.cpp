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

#include "ConversationNode.hpp"
#include <chrono>

ConversationNode::ConversationNode(std::string id, ROLE role, std::string content, std::string parentId)
    : id(id), role(role), content(content), parentId(parentId)
{
    timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch())
                    .count();
}
nlohmann::json ConversationNode::toJson() const
{
    return nlohmann::json{
        {"id", id},
        {"role", role},
        {"content", content},
        {"parentId", parentId},
        {"childIds", childIds},
        {"timestamp", timestamp}};
}
