#pragma once
#include "Item.h"
#include <vector>
#include <string>
#include <optional>

class Inventory
{
public:
    Inventory() = default;

    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path) const;

    Item addItem(const Item& item); // returns item with assigned id
    bool removeItem(int id);
    std::optional<Item> getItem(int id) const;
    bool updateItem(int id, const Item& updated); // id is preserved
    std::vector<Item> listAll() const;
    std::vector<Item> searchByName(const std::string& term) const;
    std::vector<Item> lowStock(int threshold) const;

    bool importCsv(const std::string& path); // merges (skips duplicates id)
    bool exportCsv(const std::string& path) const;

private:
    std::vector<Item> items;
    int nextId = 1;
    void recomputeNextId();
};