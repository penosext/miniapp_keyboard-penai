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

export enum ROLE {
    ROLE_USER = 0,
    ROLE_ASSISTANT = 1,
    ROLE_SYSTEM = 2
}

export interface ConversationNode {
    id: string;
    role: ROLE;
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

export declare class AI {
    static initialize(apiKey: string, baseUrl: string): boolean;
    static getCurrentPath(): ConversationNode[];
    static getChildNodes(nodeId: string): string[];
    static switchToNode(nodeId: string): boolean;
    static getCurrentNodeId(): string;
    static getRootNodeId(): string;

    static addUserMessage(message: string): Promise<BaseResponse>;
    static generateResponse(): Promise<ChatCompletionResponse>;
    static getModels(): Promise<ModelsResponse>;
    static getUserBalance(): Promise<UserBalanceResponse>;

    static on(event: 'ai_stream', callback: (data: AIStreamResult) => void): void;
}


export type PinYin = string[]
export interface Candidate {
    pinYin: PinYin;
    hanZi: string;
    freq: number;
}

export declare class IME {
    initialized: boolean;

    static initialize(): void;
    static getCandidates(rawPinyin: string): Candidate[];
    static updateWordFrequency(pinYin: PinYin, hanZi: string): void;
    static splitPinyin(rawPinyin: string): PinYin;
}
