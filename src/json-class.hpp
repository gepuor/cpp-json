#ifndef JSON_CLASS_HPP
#define JSON_CLASS_HPP

#include <string>
#include <vector>
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

using JsonPrimitive = std::variant<
    std::string,
    double,
    bool,
    std::nullptr_t
>;

class JsonData
{
    private:
        JsonPrimitive data;
        JsonType dataType;

    public:

        JsonData();
        template <class T>
        JsonData(T data, JsonType type);
        ~JsonData();

        const JsonPrimitive& getData() const;
        const JsonType& getType() const;
        template <class T>
        void setData(T newData);
};


class JsonPair
{
    private:
        std::string key;
        JsonData value;

    public:
        // TODO: constructores, getters, setters...
};


class JsonObject
{
    private:
        std::vector<JsonPair> content;

    public:
        JsonObject();
        ~JsonObject();

        // TODO: métodos para añadir, eliminar y buscar pairs...
};


class JsonArray
{
    private:
        std::vector<JsonData> content;

    public:
        JsonArray();
        ~JsonArray();

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
