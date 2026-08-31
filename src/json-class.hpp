#ifndef JSON_CLASS_HPP
#define JSON_CLASS_HPP

#include <memory>
#include <string>
#include <vector>
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <variant>

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
        data = newData;
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
          value()
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
        JsonObject();
        //~JsonObject();

        // TODO: métodos para añadir, eliminar y buscar pairs...
};


class JsonArray
{
    private:
        std::vector<JsonData> content;

    public:
        JsonArray();
        //~JsonArray();

        // TODO: métodos para añadir, eliminar y acceder a valores...
};


class JsonFile
{
    private:
        JsonObject content;

    public:
        // TODO: métodos para leer/escribir el archivo...
};


#endif
