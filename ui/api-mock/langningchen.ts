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

import { BaseResponse, ChatCompletionResponse, ConversationNode, ModelsResponse, UserBalanceResponse } from "../src/@types/langningchen";
import { ROLE } from "../src/Role";

// Mock AI class implementation
class MockAI {
    private initialized = false;
    private apiKey = "";
    private baseUrl = "";
    private currentNodeId = "root";
    private rootNodeId = "root";
    private conversations: Map<string, ConversationNode> = new Map();
    private streamCallbacks: ((data: any) => void)[] = [];

    constructor() {
        // Initialize with root node
        this.conversations.set("root", {
            id: "root",
            role: ROLE.ROLE_SYSTEM,
            content: "You are a helpful AI assistant.",
            parentId: "",
            childIds: [],
            timestamp: Date.now().toString()
        });
    }

    // Initialize the AI with API key and base URL
    initialize(apiKey: string, baseUrl: string): boolean {
        if (typeof apiKey !== 'string' || typeof baseUrl !== 'string') {
            return false;
        }

        this.apiKey = apiKey;
        this.baseUrl = baseUrl;
        this.initialized = true;
        return true;
    }

    // Get current conversation path from root to current node
    getCurrentPath(): ConversationNode[] {
        if (!this.initialized) {
            return [];
        }

        const path: ConversationNode[] = [];
        let currentId = this.currentNodeId;

        // Build path by traversing from current to root
        while (currentId) {
            const node = this.conversations.get(currentId);
            if (!node) break;
            path.unshift(node);
            currentId = node.parentId;
        }

        return path;
    }

    // Get child node IDs for a given node
    getChildNodes(nodeId: string): string[] {
        if (!this.initialized) {
            return [];
        }

        const node = this.conversations.get(nodeId);
        return node ? node.childIds : [];
    }

    // Switch to a specific node
    switchToNode(nodeId: string): boolean {
        if (!this.initialized || !this.conversations.has(nodeId)) {
            return false;
        }

        this.currentNodeId = nodeId;
        return true;
    }

    // Get current node ID
    getCurrentNodeId(): string {
        return this.initialized ? this.currentNodeId : "";
    }

    // Get root node ID
    getRootNodeId(): string {
        return this.initialized ? this.rootNodeId : "";
    }

    addUserMessage(userMessage: string): BaseResponse {
        if (!this.initialized || typeof userMessage !== 'string') {
            return {
                success: false,
                statusCode: 0,
                errorMessage: "AI not initialized or invalid user message"
            };
        }

        if (typeof userMessage !== 'string') {
            return {
                success: false,
                statusCode: 0,
                errorMessage: "Invalid user message format"
            };
        }

        // Create user message node
        const userNodeId = `user_${Date.now()}`;
        const userNode: ConversationNode = {
            id: userNodeId,
            role: ROLE.ROLE_USER,
            content: userMessage,
            parentId: this.currentNodeId,
            childIds: [],
            timestamp: Date.now().toString()
        };

        // Add user node to conversation
        this.conversations.set(userNodeId, userNode);

        // Update parent's children
        const currentNode = this.conversations.get(this.currentNodeId);
        if (currentNode) {
            currentNode.childIds.push(userNodeId);
        }

        this.currentNodeId = userNodeId;
        return {
            success: true,
            statusCode: 200,
            errorMessage: ""
        };
    }

    // Send a message (async with streaming simulation)
    async generateResponse(): Promise<ChatCompletionResponse> {
        if (!this.initialized) {
            return {
                success: false,
                statusCode: 0,
                errorMessage: "AI not initialized",
                content: ""
            };
        }

        try {
            const userNodeId = this.currentNodeId;
            const userNode = this.conversations.get(userNodeId)!;

            // Simulate AI response with streaming
            const assistantNodeId = `assistant_${Date.now() + 1}`;
            const assistantContent = this.generateMockResponse(userNode.content);

            // Simulate streaming and wait for completion
            await this.simulateStreaming(assistantContent);

            // Create assistant response node
            const assistantNode: ConversationNode = {
                id: assistantNodeId,
                role: ROLE.ROLE_ASSISTANT,
                content: assistantContent,
                parentId: userNodeId,
                childIds: [],
                timestamp: (Date.now() + 1).toString()
            };

            this.conversations.set(assistantNodeId, assistantNode);
            userNode.childIds.push(assistantNodeId);
            this.currentNodeId = assistantNodeId;

            return {
                success: true,
                statusCode: 200,
                content: assistantContent,
                errorMessage: ""
            };

        } catch (error) {
            return {
                success: false,
                statusCode: 0,
                errorMessage: `Exception occurred: ${error}`,
                content: ""
            };
        }
    }

    // Get available models
    async getModels(): Promise<ModelsResponse> {
        if (!this.initialized) {
            return {
                success: false,
                statusCode: 0,
                errorMessage: "AI not initialized",
                models: []
            };
        }

        try {
            // Mock models list
            const models = [
                "deepseek-chat",
                "deepseek-coder",
                "gpt-3.5-turbo",
                "gpt-4",
                "claude-3-haiku",
                "claude-3-sonnet"
            ];

            return {
                success: true,
                statusCode: 200,
                models,
                errorMessage: ""
            };
        } catch (error) {
            return {
                success: false,
                statusCode: 0,
                errorMessage: `Exception occurred while getting models: ${error}`,
                models: []
            };
        }
    }

    // Get user balance
    async getUserBalance(): Promise<UserBalanceResponse> {
        if (!this.initialized) {
            return {
                success: false,
                statusCode: 0,
                errorMessage: "AI not initialized",
                isAvailable: false,
                balance: 0
            };
        }

        try {
            // Mock balance data
            return {
                success: true,
                statusCode: 200,
                isAvailable: true,
                balance: 99.5,
                errorMessage: ""
            };
        } catch (error) {
            return {
                success: false,
                statusCode: 0,
                errorMessage: `Exception occurred while getting user balance: ${error}`,
                isAvailable: false,
                balance: 0
            };
        }
    }

    // Private method to generate mock AI response
    private generateMockResponse(userMessage: string): string {
        const responses = [
            `I understand you said: "${userMessage}". That's an interesting point!`,
            `Thanks for your message about "${userMessage}". Let me help you with that.`,
            `I see you mentioned "${userMessage}". Here's what I think about it...`,
            `Your message "${userMessage}" is quite thought-provoking. My response would be...`,
            `Regarding "${userMessage}", I believe this is a topic worth exploring further.`
        ];

        return responses[Math.floor(Math.random() * responses.length)];
    }

    // Subscribe to AI stream events (implements the "on" function from C++ JQPublishObject)
    on(eventName: string, callback: (data: any) => void): void {
        if (eventName === 'ai_stream') {
            this.streamCallbacks.push(callback);
        }
    }

    // Unsubscribe from AI stream events (implements the "off" function)
    off(eventName: string, callback: (data: any) => void): void {
        if (eventName === 'ai_stream') {
            const index = this.streamCallbacks.indexOf(callback);
            if (index > -1) {
                this.streamCallbacks.splice(index, 1);
            }
        }
    }

    // Private method to publish events (mimics C++ publish function)
    private publish(eventName: string, data: any): void {
        if (eventName === 'ai_stream') {
            this.streamCallbacks.forEach(callback => {
                callback(data);
            });
        }
    }

    // Private method to simulate streaming
    private async simulateStreaming(content: string): Promise<void> {
        return new Promise<void>((resolve) => {
            const words = content.split(' ');
            let currentText = '';

            words.forEach((word, index) => {
                setTimeout(() => {
                    currentText += (index > 0 ? ' ' : '') + word;

                    // Emit message delta using publish (same as C++ implementation)
                    this.publish('ai_stream', {
                        type: "MESSAGE",
                        messageDelta: (index > 0 ? ' ' : '') + word,
                        errorMessage: ""
                    });

                    // Emit done when finished and resolve the promise
                    if (index === words.length - 1) {
                        setTimeout(() => {
                            this.publish('ai_stream', {
                                type: "DONE",
                                messageDelta: "",
                                errorMessage: ""
                            });
                            resolve(); // 完成流式传输后 resolve Promise
                        }, 100);
                    }
                }, index * 100); // Simulate typing delay
            });
        });
    }
}

// Create and export the mock AI instance
const AI = new MockAI();

// Export the mock module in the same format as the C++ module
export { AI };
export default { AI };
