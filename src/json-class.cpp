#include "json-class.hpp"


JsonData::JsonData(const JsonData& other)
    : dataType(other.dataType)
{
    std::visit(
        [this](const auto& value)
        {
            using Type = std::decay_t<decltype(value)>;

            if constexpr ( // Check if data type is simple
                std::is_same_v<Type, std::string> ||
                std::is_same_v<Type, double> ||
                std::is_same_v<Type, bool> ||
                std::is_same_v<Type, std::nullptr_t>
            )
            {
                data = value;
            }
            // Now complex data type
            else if constexpr (std::is_same_v<Type, std::unique_ptr<JsonObject>>)
            {
                // TODO: JsonObject deepcopy
            }
            else if constexpr (std::is_same_v<Type, std::unique_ptr<JsonArray>>)
            {
                // TODO: JsonArray deepcopy
            }
        },
        other.data
    );
}


void JsonData::copyFrom(const JsonData& other)
{
    dataType = other.dataType;
    std::visit(
        [this](const auto& value)
        {
            using Type = std::decay_t<decltype(value)>;

            if constexpr ( // Check if data type is simple
                std::is_same_v<Type, std::string> ||
                std::is_same_v<Type, double> ||
                std::is_same_v<Type, bool> ||
                std::is_same_v<Type, std::nullptr_t>
            )
            {
                data = value;
            }
            // Now complex data type
            else if constexpr (std::is_same_v<Type, std::unique_ptr<JsonObject>>)
            {
                // TODO: JsonObject deepcopy
            }
            else if constexpr (std::is_same_v<Type, std::unique_ptr<JsonArray>>)
            {
                // TODO: JsonArray deepcopy
            }
        },
        other.data
    );
}

JsonData& JsonData::operator=(const JsonData& other)
{
    copyFrom(other);
    return *this;
}



