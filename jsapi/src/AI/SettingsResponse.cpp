#include "SettingsResponse.hpp"

SettingsResponse::SettingsResponse(bool success, int statusCode, std::string errorMessage, std::string apiKey, std::string baseUrl, std::string modelName, int maxTokens, double temperature, double topP, std::string systemPrompt)
    : BaseResponse(success, statusCode, errorMessage),
      apiKey(apiKey), baseUrl(baseUrl), modelName(modelName),
      maxTokens(maxTokens), temperature(temperature), topP(topP),
      systemPrompt(systemPrompt) {}
