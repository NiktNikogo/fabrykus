#pragma once

#include <nlohmann/json.hpp>
#include "string"


struct Ingredient {
    double amount;
    std::string name;
};

inline void from_json(const nlohmann::json& json, Ingredient& ing) {
    json.at("amount").get_to(ing.amount);
    json.at("name").get_to(ing.name);
}

inline void to_json(nlohmann::json& json, const Ingredient& ing) {
    json = nlohmann::json{ 
        {"amount", ing.amount},
        {"name", ing.name}
    };
}