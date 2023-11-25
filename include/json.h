#ifndef JSON_H
#define JSON_H

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stack>
#include <exception>
#include <stdexcept>
#include <cctype>

class InvalidJSONException : public std::exception
{
public:
    InvalidJSONException(const std::string &message) : errorMsg(message) {}

    const char *what() const noexcept override
    {
        return errorMsg.c_str();
    }

private:
    std::string errorMsg;
};

enum class JSONTYPE
{
    STRING,
    NUMBER,
    BOOL,
    MAP,
    LIST,
    NIL
};

class JSON
{
private:
public:
    virtual JSONTYPE getType() const = 0;
    virtual std::string toString() const = 0;
    virtual ~JSON(){};
    size_t find_close(std::string str, size_t start, char c)
    {
        if (c == '{')
        {
            int brackets = 1;
            size_t i = start;
            while (i < str.length())
            {
                if (str[i] == '{')
                    brackets++;
                else if (str[i] == '}')
                    brackets--;
                if (brackets == 0)
                    return i;
                i++;
            }
        }
        else if (c == '[')
        {
            int brackets = 1;
            size_t i = start;
            while (i < str.length())
            {
                if (str[i] == '[')
                    brackets++;
                else if (str[i] == ']')
                    brackets--;
                if (brackets == 0)
                    return i;
                i++;
            }
        }
        throw InvalidJSONException("Invalid JSON format: Unable to parse.");
    }
};

class JSONSTRING : public JSON
{
private:
    std::string contents;
    JSONTYPE type = JSONTYPE::STRING;

public:
    JSONSTRING(std::string init)
    {
        contents = init;
    }
    JSONSTRING()
    {
        contents = "";
    };
    void set(std::string val)
    {
        contents = val;
    }
    std::string get()
    {
        return contents;
    }
    JSONTYPE getType() const override
    {
        return type;
    }
    std::string toString() const override
    {
        std::ostringstream stream;
        stream << "\"" << contents << "\"";
        return stream.str();
    }
    friend std::ostream &operator<<(std::ostream &os, const JSONSTRING &object)
    {
        os << object.toString();
        return os;
    }
};

class JSONBOOL : public JSON
{
private:
    bool contents;
    JSONTYPE type = JSONTYPE::BOOL;

public:
    JSONBOOL(bool init)
    {
        contents = init;
    }
    JSONBOOL()
    {
        contents = false;
    }
    void set(bool val)
    {
        contents = val;
    }
    bool get()
    {
        return contents;
    }
    JSONTYPE getType() const override
    {
        return type;
    }
    std::string toString() const override
    {
        std::ostringstream stream;
        stream << ((contents) ? "TRUE" : "FALSE");
        return stream.str();
    }
    friend std::ostream &operator<<(std::ostream &os, const JSONBOOL &object)
    {
        os << object.toString();
        return os;
    }
};

class JSONNUMBER : public JSON
{
private:
    double contents;
    JSONTYPE type = JSONTYPE::NUMBER;

public:
    JSONNUMBER(double init)
    {
        contents = init;
    }
    JSONNUMBER(int init)
    {
        contents = (double)init;
    }
    JSONNUMBER()
    {
        contents = 0;
    }
    void set(double val)
    {
        contents = val;
    }
    void set(int val)
    {
        contents = (double)val;
    }
    double get()
    {
        return contents;
    }
    int get_int()
    {
        return (int)contents;
    }
    JSONTYPE getType() const override
    {
        return type;
    }
    std::string toString() const override
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << contents;
        return stream.str();
    }
    friend std::ostream &operator<<(std::ostream &os, const JSONNUMBER &object)
    {
        os << object.toString();
        return os;
    }
};

class JSONLIST;
class JSONMAP;

class JSONMAP : public JSON
{
private:
    std::map<std::string, std::shared_ptr<JSON>> contents;
    JSONTYPE type = JSONTYPE::MAP;

public:
    JSONMAP()
    {
        contents = std::map<std::string, std::shared_ptr<JSON>>();
    }
    void set(const std::string &key, const std::string &val)
    {
        contents[key] = std::make_shared<JSONSTRING>(JSONSTRING(val));
    }
    void set(const std::string &key, const double &val)
    {
        contents[key] = std::make_shared<JSONNUMBER>(JSONNUMBER(val));
    }
    void set(const std::string &key, const int &val)
    {
        contents[key] = std::make_shared<JSONNUMBER>(JSONNUMBER(val));
    }
    void set(const std::string &key, std::shared_ptr<JSON> val)
    {
        contents[key] = val;
    }

    std::shared_ptr<JSON> get(const std::string &key)
    {
        return contents[key];
    }
    JSONTYPE getType() const override
    {
        return type;
    }
    bool contains_key(std::string str)
    {
        bool found = false;
        for (auto iter = contents.begin(); iter != contents.end(); iter++)
        {
            if (iter->first == str)
                return true;
            else if (iter->second->getType() == JSONTYPE::MAP)
                if (std::dynamic_pointer_cast<JSONMAP>(iter->second)->contains_key(str))
                    return true;
        }
        return false;
    }
    std::string toString() const override
    {
        std::ostringstream stream;
        auto last = std::prev(contents.end());
        stream << "{ ";
        for (auto iter = contents.begin(); iter != contents.end(); iter++)
        {
            stream << "\"" << iter->first << "\": " << iter->second->toString();
            if (iter != last)
                stream << ", ";
        }
        stream << " }";
        return stream.str();
    }
    friend std::ostream &operator<<(std::ostream &os, const JSONMAP &object)
    {
        os << object.toString();
        return os;
    }
};

class JSONLIST : public JSON
{
private:
    std::vector<std::shared_ptr<JSON>> contents;
    JSONTYPE type = JSONTYPE::LIST;

public:
    JSONLIST()
    {
        contents = std::vector<std::shared_ptr<JSON>>();
    }
    void append(std::shared_ptr<JSON> val)
    {
        contents.push_back(val);
    }
    void set(size_t i, std::shared_ptr<JSON> val)
    {
        contents[i] = val;
    }
    std::shared_ptr<JSON> get(const int i)
    {
        return contents[i];
    }
    JSONTYPE getType() const override
    {
        return type;
    }
    std::string toString() const override
    {
        std::ostringstream stream;
        stream << "[ " << std::endl;
        auto last = std::prev(contents.end());
        for (auto iter = contents.begin(); iter != contents.end(); iter++)
        {
            stream << (*iter)->toString();
            if (iter != last)
                stream << ", ";
        }
        stream << " ]";
        return stream.str();
    }
    friend std::ostream &operator<<(std::ostream &os, const JSONLIST &object)
    {
        os << object.toString();
        return os;
    }
};

class JSONParser
{
public:
    static JSONMAP loads(const std::string &str)
    {
        return *std::dynamic_pointer_cast<JSONMAP>(JSONParser::loadString(str));
    }
    static std::shared_ptr<JSON> loadString(const std::string &str)
    {
        if (str[0] != '{')
        {
            std::shared_ptr<JSON> json_value;
            if (str == "true" || str == "false")
            {
                json_value = std::make_shared<JSONBOOL>(str == "true");
            }
            else if (std::isdigit(str[0]))
            {
                // parse JSON NUMBER
                json_value = std::make_shared<JSONNUMBER>(std::stod(str));
            }
            else if (std::isalpha(str[0]) || str[0] == '"')
            {
                // parse JSON STRING
                size_t pos_first = str.find_first_not_of("\"");
                size_t pos_last = str.find_last_not_of("\"");
                json_value = std::make_shared<JSONSTRING>(str.substr(pos_first, pos_last - pos_first + 1));
            }
            else if (str[0] == '[')
            {
                // parse JSON LIST
                std::shared_ptr<JSONLIST> list = std::make_shared<JSONLIST>();
                std::string cur_item = "";
                size_t item_pos = 0;
                while (item_pos < str.length())
                {
                    size_t item_start = str.find_first_not_of("[ \t\r\n", item_pos);
                    size_t item_end = str.find_first_of(",]", item_start + 1);
                    cur_item = str.substr(item_start, item_end - item_start);
                    std::shared_ptr<JSON> cur_item_object = JSONParser::loadString(cur_item);
                    list->append(cur_item_object);
                    item_pos = item_end + 1;
                }
                json_value = list;
            }
            return json_value;
        }
        JSONMAP object;
        size_t pos = 0;
        size_t length = str.length();

        while (pos < length)
        {
            size_t key_start = str.find('"', pos);
            size_t key_end = str.find('"', key_start + 1);
            std::string key = str.substr(key_start + 1, key_end - key_start - 1);
            size_t val_start = str.find(':', key_end);
            size_t val_end = str.find_first_of(",}", val_start);
            std::string val = str.substr(val_start + 1, val_end - val_start - 1);
            size_t val_start_t = val.find_first_not_of(" \t\r\n");
            size_t val_end_t = val.find_last_not_of(" \t\r\n");
            val = val.substr(val_start_t, val_end_t - val_start_t + 1);
            if (val[0] == '[')
            {
                val_end = str.find_first_of("]", val_start);
                val = str.substr(val_start + 1, val_end - val_start);
                val_start_t = val.find_first_not_of(" \t\r\n");
                val_end_t = val.find_last_not_of(" \t\r\n");
                val = val.substr(val_start_t, val_end_t - val_start_t + 1);
            }
            if (val[0] == '{')
            {
                val_end = str.find_first_of("}", val_start);
                val = str.substr(val_start + 1, val_end - val_start);
                val_start_t = val.find_first_not_of(" \t\r\n");
                val_end_t = val.find_last_not_of(" \t\r\n");
                val = val.substr(val_start_t, val_end_t - val_start_t + 1);
            }
            std::shared_ptr<JSON> json_value = JSONParser::loadString(val);
            object.set(key, json_value);
            pos = val_end + 1;
            if (str[str.find_first_not_of(" \t\r\n", pos)] == '}')
                break;
        }
        return std::make_shared<JSONMAP>(object);
    }
};

#endif