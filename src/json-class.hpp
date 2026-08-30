#ifndef JSON-CLASS

#define JSON-CLASS

#include <string>
#include <fstream>

const std::string JSON_DATA_TYPES = {"String","Number","Boolean","Null","Object","Array"};

template<class T>
class Object
{
    private:
        std::string objectDataType;
        T content;
    public:

}


class JsonFile
{
    private:
        Object * content;
        unsigned int nObjects;
    public:

}


#endif