#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> split_string(const std::string_view &string, const std::string_view &delimiter)
{
    auto start = 0U;
    auto end = string.find(delimiter);

    std::vector<std::string> result;
    std::string token;

    while (end = string.find(delimiter, start), end != std::string::npos)
    {
        token = string.substr(start, end - start);

        start = end + delimiter.length();

        result.push_back(token);
    }

    token = string.substr(start);
    result.push_back(token);

    return result;
}

inline void ltrim(std::string &string)
{
    string.erase(string.begin(), std::find_if(string.begin(), string.end(), [](unsigned char ch)
                                    { return !std::isspace(ch); }));
}