#include "json-class.hpp"
#include <type_traits>

void JsonData::copyFrom(const JsonData& other)
{
    if (this == &other)
        return;

    dataType = other.dataType;

    std::visit(
        [this](const auto& value)
        {
            using Type = std::decay_t<decltype(value)>;

            if constexpr (
                std::is_same_v<Type, std::string> ||
                std::is_same_v<Type, double> ||
                std::is_same_v<Type, bool> ||
                std::is_same_v<Type, std::nullptr_t>
            )
            {
                data = value;
            }
            else if constexpr (
                std::is_same_v<Type, std::unique_ptr<JsonObject>>
            )
            {
                if (value)
                {
                    data = std::make_unique<JsonObject>(*value);
                }
                else
                {
                    data = nullptr;
                }
            }
            else if constexpr (
                std::is_same_v<Type, std::unique_ptr<JsonArray>>
            )
            {
                if (value)
                {
                    data = std::make_unique<JsonArray>(*value);
                }
                else
                {
                    data = nullptr;
                }
            }
        },
        other.data
    );
}

JsonData::JsonData(const JsonData& other)
    : dataType(other.dataType)
{
    copyFrom(other);
}

JsonData& JsonData::operator=(const JsonData& other)
{
    copyFrom(other);
    return *this;
}

void JsonObject::add(const std::string& key, const JsonData& value)
{
    content.push_back(JsonPair(key,value));
}

bool JsonObject::removeFirst(const std::string& key)
{
    std::vector<JsonPair>::iterator it_cont;
    for(it_cont = content.begin();it_cont != content.end();)
    {
        if(it_cont->getKey() == key)
        {
            content.erase(it_cont);
            return true;
        }
        ++it_cont;
    }
    return false;
}


std::size_t JsonObject::remove(const std::string& key, std::size_t count)
{
    std::size_t removed = 0;
    std::vector<JsonPair>::iterator it_cont;
    if(count > 0)
    {
        for(it_cont = content.begin();it_cont != content.end();)
        {
            if(it_cont->getKey() == key)
            {
                it_cont = content.erase(it_cont);
                removed++;
                if(removed == count) return removed;
            }
            else
            {
                ++it_cont;
            }
        }
    }
    return removed;
}

std::size_t JsonObject::removeAll(const std::string& key)
{
    std::size_t removed = 0;
    std::vector<JsonPair>::iterator it_cont;
    for(it_cont = content.begin();it_cont != content.end();)
    {
        if(it_cont->getKey() == key)
        {
            it_cont = content.erase(it_cont);
            removed++;
        }
        else
        {
            ++it_cont;
        }
    }
    return removed;
}

JsonPair* JsonObject::find(const std::string& key)
{
    for(size_t i = 0;i<content.size();i++)
    {
        if(key == content[i].getKey()) return &(content[i]);
    }
    return nullptr;
}
const JsonPair* JsonObject::find(const std::string& key) const
{
    for(size_t i = 0;i<content.size();i++)
    {
        if(key == content[i].getKey()) return &(content[i]);
    }
    return nullptr;
}
/*
JsonData parse(const std::string& json)
{ 
}

std::string serialize(const JsonData& data);
*/


