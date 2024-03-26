#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <tuple>

enum http_Code
{
    OK = 200,
    CREATED = 201,
    NOT_FOUND = 404
};

std::vector<std::string> split_string(const std::string_view &string, const std::string_view &delimiter = " ")
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

inline void rtrim(std::string &string)
{
    string.erase(std::find_if(string.rbegin(), string.rend(), [](unsigned char ch)
                              { return !std::isspace(ch); })
                     .base(),
                 string.end());
}

std::tuple<std::string, http_Code> load_from_file(const std::string &filename, std::string &directory)
{
    std::string line;
    std::string result_body = "";
    std::string full_path = directory + "/" + filename;

    std::ifstream myfile(full_path);

    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            result_body += line;
            result_body += "\n";
        }
        result_body.erase(result_body.end() - 1, result_body.end());

        myfile.close();
        return {result_body, OK};
    }
    std::cout << "Unable to open file";
    return {result_body, NOT_FOUND};
}

http_Code write_file(const std::string &filename, std::string &directory, std::string &content)
{
    std::string full_path = directory + "/" + filename;
    std::vector<std::string> lines = split_string(content, "\n");
    std::ofstream myfile(full_path);

    if (myfile.is_open())
    {

        for (int i = 0; i < lines.size() - 1; i++)
        {
            myfile << lines[i] << std::endl;
        }

        myfile << lines[lines.size() - 1];

        myfile.close();

        return CREATED;
    }
    std::cout << "Unable to open file";

    return NOT_FOUND;
}