#pragma once

#include <boost/rational.hpp>
#include <nlohmann/json.hpp>
#include "string"

typedef boost::rational<long long> Rational;
struct Ingredient
{
    static constexpr long long PRECISION = 1e6;
    Rational amount;
    std::string name;

    auto asFloat() const -> float
    {
        return boost::rational_cast<float>(amount);
    }
    auto asDouble() const -> double
    {
        return boost::rational_cast<double>(amount);
    }
    static auto makeFromFloat(const float value) -> Rational
    {
        return Rational(static_cast<long long>(value * PRECISION), PRECISION);
    }
    static auto makeFromDouble(const double value) -> Rational
    {
        return Rational(static_cast<long long>(value * PRECISION), PRECISION);
    }
    auto fromDouble(const double value) -> void
    {
        amount = makeFromDouble(value);
    }
    auto fromFloat(const float value) -> void
    {
        amount = makeFromFloat(value);
    }

    Ingredient() : amount(0), name("")
    {
    }
    Ingredient(Rational amount, std::string name) : amount(amount), name(name)
    {
    }
    Ingredient(double amount, std::string name) : name(name)
    {
        this->amount = makeFromDouble(amount);
    }
};

inline void from_json(const nlohmann::json &json, Ingredient &ing)
{
    if (json.at("amount").is_number())
    {
        double oldValue = json.at("amount").get<double>();
        ing.amount = Rational(static_cast<long long>(oldValue * Ingredient::PRECISION), Ingredient::PRECISION);
    }
    else
    {
        long long num = json.at("amount").at("num").get<long long>();
        long long den = json.at("amount").at("den").get<long long>();
        ing.amount = Rational(num, den);
    }
    json.at("name").get_to(ing.name);
}

inline void to_json(nlohmann::json &json, const Ingredient &ing)
{
    json = nlohmann::json{
        {"amount", {{"num", ing.amount.numerator()}, {"den", ing.amount.denominator()}}},
        {"name", ing.name}};
}