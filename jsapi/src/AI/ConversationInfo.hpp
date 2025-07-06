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

#include "BaseResponse.hpp"
#include <string>
#include <vector>

struct ConversationInfo
{
    std::string id;
    std::string title;
    long long createdAt;
    long long updatedAt;

    ConversationInfo(std::string id, std::string title, long long createdAt, long long updatedAt)
        : id(id), title(title), createdAt(createdAt), updatedAt(updatedAt) {}
};

struct ConversationListResponse : BaseResponse
{
    std::vector<ConversationInfo> conversations;

    ConversationListResponse(bool success, int statusCode, std::string errorMessage);
};
