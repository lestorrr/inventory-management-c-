    #include "Inventory.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iostream>

bool Inventory::loadFromFile(const std::string& path) {
    items.clear();
    std::ifstream in(path);
    if (!in.is_open()) return false;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        items.push_back(Item::fromCsv(line));
    }
    recomputeNextId();
    return true;
}

bool Inventory::saveToFile(const std::string& path) const {
    std::ofstream out(path, std::ios::trunc);
    if (!out.is_open()) return false;
    for (const auto& it : items) {
        out << it.toCsv() << '\n';
    }
    return true;
}

Item Inventory::addItem(const Item& item) {
    Item copy = item;
    copy.id = nextId++;
    items.push_back(copy);
    return copy;
}

bool Inventory::removeItem(int id) {
    auto newEnd = std::remove_if(items.begin(), items.end(), [&](const Item& i){ return i.id == id; });
    bool removed = (newEnd != items.end());
    if (removed) items.erase(newEnd, items.end());
    return removed;
}

std::optional<Item> Inventory::getItem(int id) const {
    for (const auto& i : items) if (i.id == id) return i;
    return std::nullopt;
}

bool Inventory::updateItem(int id, const Item& updated) {
    for (auto& i : items) {
        if (i.id == id) {
            i.name = updated.name;
            i.category = updated.category;
            i.supplier = updated.supplier;
            i.quantity = updated.quantity;
            i.price = updated.price;
            return true;
        }
    }
    return false;
}

std::vector<Item> Inventory::listAll() const {
    return items;
}

std::vector<Item> Inventory::searchByName(const std::string& term) const {
    std::vector<Item> out;
    std::string lowerTerm = term;
    std::transform(lowerTerm.begin(), lowerTerm.end(), lowerTerm.begin(), ::tolower);
    for (const auto& i : items) {
        std::string name = i.name;
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        if (name.find(lowerTerm) != std::string::npos) out.push_back(i);
    }
    return out;
}

std::vector<Item> Inventory::lowStock(int threshold) const {
    std::vector<Item> out;
    for (const auto& i : items) if (i.quantity <= threshold) out.push_back(i);
    return out;
}

bool Inventory::importCsv(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) return false;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        Item it = Item::fromCsv(line);
        // avoid ID collision: if id is zero or matches existing, reassign
        bool conflict = false;
        for (const auto& existing : items) if (existing.id == it.id) { conflict = true; break; }
        if (it.id <= 0 || conflict) {
            it.id = nextId++;
        } else {
            if (it.id >= nextId) nextId = it.id + 1;
        }
        items.push_back(it);
    }
    return true;
}

bool Inventory::exportCsv(const std::string& path) const {
    return saveToFile(path);
}

void Inventory::recomputeNextId() {
    int maxId = 0;
    for (const auto& i : items) if (i.id > maxId) maxId = i.id;
    nextId = maxId + 1;
}