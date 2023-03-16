//
// Created by Naz on 3/16/2023.
//
#include "config_manager.h"

inline void text_formatters::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

inline void text_formatters::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

inline void text_formatters::trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}


std::vector<std::string> file_reader(std::ifstream &filestream) {
    std::vector<std::string> result;
    std::string line;
    while (std::getline(filestream, line)) {
        if (line.empty() || line.find('=') == line.size()) {
            continue;
        }
        result.push_back(line);
    }
    return result;
}

std::map<std::string, double> get_config(const std::string &file_name) {
    std::ifstream config_file(file_name);
    if (!config_file.is_open()) {
        throw std::runtime_error("");
    }
    // reading the content
    auto expressions = file_reader(config_file);

    // making the map of default config properties
    std::map<std::string, double> result = {
            {"abs_err",      0.000005},
            {"rel_err",      0.0002},
            {"x_start",      -50},
            {"x_end",        50},
            {"y_start",      -50},
            {"y_end",        50},
            {"init_steps_x", 100},
            {"init_steps_y", 100},
            {"max_iter",     20}
    };

    for (auto &expr: expressions) {
        text_formatters::trim(expr);
        expr.erase(std::remove_if(expr.begin(), expr.end(), [](char c) { return std::iswspace(c); }), expr.end());

        if (expr.empty() || expr[0] == '#') {
            continue;
        }

        // checking whether expression is correct
        if (!std::count(expr.begin(), expr.end(), '=')) {
            std::string error_message = std::string("Error while parsing a line: " + std::to_string((int) expr[0]));
            error_message.append(expr);
            throw std::logic_error(error_message);
        }

        std::replace(expr.begin(), expr.end(), '=', ' ');
        std::stringstream ss{expr};

        // getting the parameter and value
        std::string param;
        std::string value;

        ss >> param >> value;

        // checking whether parameter name is correct
        if (!result.contains(param)) {
            std::string error_message = std::string("Unknown argument ");
            error_message.append(param);
            error_message.append(" in config file");
            throw std::logic_error(error_message);
        }

        // checking whether the value of the parameter is correct number
        double value_parsed;
        try {
            value_parsed = std::stod(value);
        }
        catch (std::invalid_argument &) {
            std::string error_message = std::string("Invalid value ");
            error_message.append(value);
            error_message.append(" for parameter ");
            error_message.append(param);
            throw std::logic_error(error_message);
        }

        // adding new value of parameter to map
        result[param] = value_parsed;
    }

// checking whether config properties are correct
    if (result["x_start"] > result["x_end"] || result["y_start"] > result["y_end"]) {
        throw std::logic_error("Wrong bounds for x or y passed.");
    }

// all checks passed! closing the file and returning a map

    config_file.close();
    return result;
}
