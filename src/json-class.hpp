#ifndef JSON_CLASS_HPP
#define JSON_CLASS_HPP

#include <memory>
#include <string>
#include <vector>
#include <cstddef>
#include <utility>
#include <variant>
#include <iostream>


enum class JsonType
{
    String,
    Number,
    Boolean,
    Null,
    Object,
    Array
};

class JsonObject;
class JsonArray;

using JsonPrimitive = std::variant<
    std::string,
    double,
    bool,
    std::nullptr_t,
    std::unique_ptr<JsonObject>,
    std::unique_ptr<JsonArray>
>;

class JsonData
{
    private:
        JsonPrimitive data = nullptr;
        JsonType dataType = JsonType::Null;

    public:
        JsonData() = default;
        template <class T>
        JsonData(T newData){setData(newData);}
        JsonData(const JsonData& other);
        ~JsonData(){}

        JsonData& operator=(const JsonData& other);


        const JsonPrimitive& getData() const {return data;}
        JsonType getType() const {return dataType;}
        template <class T>
        void setData(T newData);

        void copyFrom(const JsonData& other);

};

template <class T>
void JsonData::setData(T newData)
{
    using Type = std::decay_t<T>;

    if constexpr (std::is_same_v<Type, std::string>)
    {
        dataType = JsonType::String;
        data = std::move(newData);
    }
    else if constexpr (std::is_same_v<Type, bool>)
    {
        dataType = JsonType::Boolean;
        data = newData;
    }
    else if constexpr (
        std::is_integral_v<Type> ||
        std::is_floating_point_v<Type>
    )
    {
        dataType = JsonType::Number;
        data = static_cast<double>(newData);
    }
    else if constexpr (std::is_same_v<Type, std::nullptr_t>)
    {
        dataType = JsonType::Null;
        data = nullptr;
    }
    else if constexpr (std::is_same_v<Type, JsonObject>)
    {
        dataType = JsonType::Object;
        data = std::make_unique<JsonObject>(std::move(newData));
    }
    else if constexpr (std::is_same_v<Type, JsonArray>)
    {
        dataType = JsonType::Array;
        data = std::make_unique<JsonArray>(std::move(newData));
    }
    else
    {
        std::cerr << "Data type is not valid, ignoring data insertion\n";
    }
}

class JsonPair
{
private:
    std::string key;
    JsonData value;

public:
    JsonPair() = default;

    JsonPair(std::string newKey)
        : key(newKey),
          value(nullptr)
    {
    }

    JsonPair(std::string newKey, JsonData newValue)
        : key(newKey),
          value(newValue)
    {
    }

    const std::string& getKey() const{return key;}
    JsonData& getValue(){return value;}
    const JsonData& getValue() const{return value;}

    void setKey(std::string newKey){key = newKey;}
    void setValue(const JsonData& newValue){value = newValue;}
};



class JsonObject
{
    private:
        std::vector<JsonPair> content;

    public:
        JsonObject() = default;
        JsonObject(std::vector<JsonPair> newContent): 
            content(newContent)
        {

        }

        void add(const std::string& key, const JsonData& value);

        bool removeFirst(const std::string& key);
        std::size_t remove(const std::string& key, std::size_t count);
        std::size_t removeAll(const std::string& key);

        JsonPair* find(const std::string& key);
        const JsonPair* find(const std::string& key) const;

        bool contains(const std::string& key) const{return find(key) != nullptr;}

        std::size_t size() const{return content.size();}
        
        void resize( size_t count ) {content.resize(count);}
        void resize( size_t count, const JsonPair& value ) {content.resize(count,value);}

        std::vector<JsonPair>::iterator erase( std::vector<JsonPair>::iterator pos ) {return content.erase(pos);}
        std::vector<JsonPair>::iterator erase( const std::vector<JsonPair>::iterator &pos ) { return content.erase(pos);}
        std::vector<JsonPair>::iterator erase( std::vector<JsonPair>::iterator first, std::vector<JsonPair>::iterator last ) { return content.erase(first,last);}
        std::vector<JsonPair>::iterator erase( const std::vector<JsonPair>::iterator &first, const std::vector<JsonPair>::iterator &last ) { return content.erase(first,last);}

        void push_back( const JsonPair& value ) {content.push_back(value);}
        void push_back( JsonPair& value ) {content.push_back(value);}
        void pop_back() {content.pop_back();}
};


class JsonArray
{
    private:
        std::vector<JsonData> content;

    public:
        JsonArray() = default;
        JsonArray(std::vector<JsonData> newContent): content(newContent)
        {

        }
        //~JsonArray();
        std::vector<JsonData>& getContent() {return content;}
        const std::vector<JsonData>& getContent() const {return content;}

        void setContent(std::vector<JsonData> newContent)
        {
            content = std::move(newContent);
        }

        size_t size(){return content.size();}

        void resize( size_t count) {content.resize(count);}
        void resize( size_t count, const JsonData& value ) {content.resize(count,value);}
        
        std::vector<JsonData>::iterator erase( std::vector<JsonData>::iterator pos ) {return content.erase(pos);}
        std::vector<JsonData>::iterator erase( const std::vector<JsonData>::iterator &pos ) { return content.erase(pos);}
        std::vector<JsonData>::iterator erase( std::vector<JsonData>::iterator first, std::vector<JsonData>::iterator last ) { return content.erase(first,last);}
        std::vector<JsonData>::iterator erase( const std::vector<JsonData>::iterator &first, const std::vector<JsonData>::iterator &last ) { return content.erase(first,last);}

        void push_back( const JsonData& value ) {content.push_back(value);}
        void push_back( JsonData& value ) {content.push_back(value);}
        void pop_back() {content.pop_back();}

};


class JsonFile
{
    private:
        JsonData content;

    public:
        JsonFile() = default;
        JsonFile(JsonData newContent): content(newContent) 
        {

        }

        JsonData& getContent() {return content;}
        const JsonData& getContent() const {return content;}

        void setContent(JsonData newContent)
        {
            content = std::move(newContent);
        }
};
enum class JsonTokenType
{
    LBrace,      // {
    RBrace,      // }
    LBracket,    // [
    RBracket,    // ]
    Colon,       // :
    Comma,       // ,
    String,
    Number,
    True,
    False,
    Null,
    End
};

struct JsonToken
{
    JsonTokenType type;
    std::string value;
};


class JsonLexer
{
    private:
        std::string input;
        std::size_t current = 0;

        void skipWhitespace();

        JsonToken scanString();
        JsonToken scanNumber();
        JsonToken scanKeyword();

        char peek() const;
        char advance();
        bool isAtEnd() const;
    public:
        JsonLexer(const std::string& input);

        std::vector<JsonToken> tokenize();
};


class JsonParser
{
    private:
        std::vector<JsonToken> tokens;
        std::size_t current = 0;

        JsonData parseValue();
        JsonObject parseObject();
        JsonArray parseArray();

        std::string parseString();
        double parseNumber();

        bool match(JsonTokenType type);
        bool check(JsonTokenType type) const;
        const JsonToken& advance();
        const JsonToken& peek() const;
    public:
        JsonParser(const std::vector<JsonToken>& tokens);

        JsonData parse();
};

#endif
