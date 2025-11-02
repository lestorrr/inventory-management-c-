#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <cctype>

struct Item
{
    int id = 0;
    std::string name;
    std::string category;
    std::string supplier;
    int quantity = 0;
    double price = 0.0;

    std::string toCsv() const {
        auto quote = [](const std::string& s)->std::string {
            bool need = s.find_first_of(",\"\n\r") != std::string::npos;
            std::string out;
            for (char c : s) {
                if (c == '"') out += "\"\"";
                else out += c;
            }
            if (need) return std::string("\"") + out + "\"";
            return out;
        };
        std::ostringstream ss;
        ss << id << ',' << quote(name) << ',' << quote(category) << ',' << quote(supplier)
           << ',' << quantity << ',' << price;
        return ss.str();
    }

    static Item fromCsv(const std::string& line) {
        Item it;
        std::vector<std::string> fields;
        fields.reserve(6);

        // parse CSV line supporting quoted fields
        std::string cur;
        bool inQuotes = false;
        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (inQuotes) {
                if (c == '"' && i + 1 < line.size() && line[i+1] == '"') {
                    cur += '"'; ++i; // escaped quote
                } else if (c == '"') {
                    inQuotes = false;
                } else {
                    cur += c;
                }
            } else {
                if (c == '"') {
                    inQuotes = true;
                } else if (c == ',') {
                    fields.push_back(cur);
                    cur.clear();
                } else {
                    cur += c;
                }
            }
        }
        fields.push_back(cur);

        auto get = [&](size_t idx)->std::string {
            if (idx < fields.size()) return fields[idx];
            return std::string();
        };

        try { it.id = std::stoi(get(0)); } catch(...) { it.id = 0; }
        it.name = get(1);
        it.category = get(2);
        it.supplier = get(3);
        try { it.quantity = std::stoi(get(4)); } catch(...) { it.quantity = 0; }
        try { it.price = std::stod(get(5)); } catch(...) { it.price = 0.0; }

        return it;
    }
};