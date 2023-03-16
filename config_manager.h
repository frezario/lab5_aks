//
// Created by Naz on 3/16/2023.
//

#ifndef TEMPLATE_CONFIG_MANAGER_H
#define TEMPLATE_CONFIG_MANAGER_H

#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

namespace text_formatters {
    // trim from start (in place)
    void inline ltrim(std::string &s);

    // trim from end (in place)
    void inline rtrim(std::string &s);

    // trim from both ends (in place)
    void inline trim(std::string &s);
}


/*
 * A function to read a config file.
 * Reads only lines with '=' in them.
 * Returns a vector of nonempty, significant strings.
 * */
std::vector<std::string> file_reader(std::ifstream &filestream);

/*
 * Gets configuration settings from config file and returns a map
 * where keys are config parameters, and values are the
 * corresponding values of config parameters.
 * */
std::map<std::string, double> get_config(const std::string &file_name);

#endif //TEMPLATE_CONFIG_MANAGER_H
