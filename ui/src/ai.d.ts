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

export interface ConversationNode {
    id: string;
    role: 'user' | 'assistant' | 'system';
    content: string;
    parentId: string;
    childIds: string[];
    timestamp: string;
}

export interface BaseResponse {
    success: boolean;
    statusCode: number;
    errorMessage: string;
}

export interface ChatCompletionResponse extends BaseResponse {
    content: string;
}

export interface ModelsResponse extends BaseResponse {
    models: string[];
}

export interface UserBalanceResponse extends BaseResponse {
    isAvailable: boolean;
    balance: number;
}

export enum AIStreamType {
    MESSAGE = "MESSAGE",
    LENGTH = "LENGTH",
    DONE = "DONE",
    ERROR = "ERROR"
}

export interface AIStreamResult {
    type: AIStreamType;
    messageDelta: string;
    errorMessage: string;
}
