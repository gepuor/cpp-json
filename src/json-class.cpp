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



void JsonLexer::skipWhitespace()
{
    while (!isAtEnd() && (
        *current == ' ' ||
        *current == '\n' ||
        *current == '\r' ||
        *current == '\t'))
    {
        advance();
    }
}

JsonToken JsonLexer::scanString()
{
    std::string text = "";
    advance();
    while (!isAtEnd() && *current != '"')
    {
        if(*current == '\\')    
        {
            advance();
            if(isAtEnd()) 
                break;

            if(*current == 'n')
            {
                text += '\n';
            }
            else if(*current == 'r')
            {
                text += '\r';
            }
            else if(*current == 't')
            {
                text += '\t';
            }
            else 
            {
                text += *current;
            }
        }
        else
        {
            text += *current;
            
        }
        advance();
    }

    if (!isAtEnd())
        advance();

    JsonToken result;
    result.type = JsonTokenType::String;
    result.value = text;
    return result;
}

JsonToken JsonLexer::scanNumber()
{
    std::string number = "";

    if (*current == '-') // Positive or negative
    {
        number += *current;
        advance();

        if (isAtEnd() || !std::isdigit(*current))
        {
            throw std::runtime_error("Invalid JSON number");
        }
    }

    if (*current == '0') // Integer
    {
        number += *current;
        advance();

        if (!isAtEnd() && std::isdigit(*current)) // 01, 02, ... is not allowed
        {
            throw std::runtime_error("Invalid JSON number");
        }
    }
    else
    {
        if (isAtEnd() || !std::isdigit(*current))
        {
            throw std::runtime_error("Invalid JSON number");
        }

        while (!isAtEnd() && std::isdigit(*current))
        {
            number += *current;
            advance();
        }
    }

    if (!isAtEnd() && *current == '.') // Decimal
    {
        number += *current;
        advance();

        if (isAtEnd() || !std::isdigit(*current))
        {
            throw std::runtime_error("Invalid JSON number");
        }

        while (!isAtEnd() && std::isdigit(*current))
        {
            number += *current;
            advance();
        }
    }

    if (!isAtEnd() && (*current == 'e' || *current == 'E')) // Exponent
    {
        number += *current;
        advance();

        if (!isAtEnd() && (*current == '+' || *current == '-'))
        {
            number += *current;
            advance();
        }

        if (isAtEnd() || !std::isdigit(*current))
        {
            throw std::runtime_error("Invalid JSON number");
        }

        while (!isAtEnd() && std::isdigit(*current))
        {
            number += *current;
            advance();
        }
    }

    JsonToken result;
    result.type = JsonTokenType::Number;
    result.value = number;

    return result;
}

JsonToken JsonLexer::scanKeyword()
{
    std::string keyWord = "";
    JsonTokenType type;
    bool correct = false;

    if (!isAtEnd())
    {
        switch (*current)
        {
            case 't':
                advance();
                if (!isAtEnd() && *current == 'r')
                {
                    advance();
                    if (!isAtEnd() && *current == 'u')
                    {
                        advance();
                        if (!isAtEnd() && *current == 'e')
                        {
                            correct = true;
                            keyWord = "true";
                            type = JsonTokenType::True;
                            advance();
                        }
                    }
                }
                break;

            case 'f':
                advance();
                if (!isAtEnd() && *current == 'a')
                {
                    advance();
                    if (!isAtEnd() && *current == 'l')
                    {
                        advance();
                        if (!isAtEnd() && *current == 's')
                        {
                            advance();
                            if (!isAtEnd() && *current == 'e')
                            {
                                correct = true;
                                keyWord = "false";
                                type = JsonTokenType::False;
                                advance();
                            }
                        }
                    }
                }
                break;

            case 'n':
                advance();
                if (!isAtEnd() && *current == 'u')
                {
                    advance();
                    if (!isAtEnd() && *current == 'l')
                    {
                        advance();
                        if (!isAtEnd() && *current == 'l')
                        {
                            correct = true;
                            keyWord = "null";
                            type = JsonTokenType::Null;
                            advance();
                        }
                    }
                }
                break;

            default:
                throw std::runtime_error("Invalid keyword");
        }
    }

    if (!correct)
        throw std::runtime_error("Invalid keyword");

    JsonToken result;
    result.type = type;
    result.value = keyWord;

    return result;
}

JsonToken JsonLexer::scanSymbol()
{
    JsonToken result;

    switch (*current)
    {
        case '{':
            result.type = JsonTokenType::LBrace;
            break;

        case '}':
            result.type = JsonTokenType::RBrace;
            break;

        case '[':
            result.type = JsonTokenType::LBracket;
            break;

        case ']':
            result.type = JsonTokenType::RBracket;
            break;

        case ':':
            result.type = JsonTokenType::Colon;
            break;

        case ',':
            result.type = JsonTokenType::Comma;
            break;

        default:
            throw std::runtime_error("Unknown character");
    }

    result.value = *current;
    advance();

    return result;
}


std::vector<JsonToken> JsonLexer::tokenize()
{
    
}