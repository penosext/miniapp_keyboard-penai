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

import * as langningchen from './langningchen';

export declare class AI {
    static initialize(apiKey: string, baseUrl: string): boolean;
    static getCurrentPath(): langningchen.ConversationNode[];
    static getChildNodes(nodeId: string): string[];
    static switchToNode(nodeId: string): boolean;
    static getCurrentNodeId(): string;
    static getRootNodeId(): string;

    static addUserMessage(message: string): Promise<langningchen.BaseResponse>;
    static generateResponse(): Promise<langningchen.ChatCompletionResponse>;
    static getModels(): Promise<langningchen.ModelsResponse>;
    static getUserBalance(): Promise<langningchen.UserBalanceResponse>;

    static on(event: 'ai_stream', callback: (data: langningchen.AIStreamResult) => void): void;
}

export declare class IME {
    initialized: boolean;

    static initialize(): void;
    static getCandidates(rawPinyin: string): langningchen.Candidate[];
    static updateWordFrequency(pinYin: langningchen.PinYin, hanZi: string): void;
    static splitPinyin(rawPinyin: string): langningchen.PinYin;
}
