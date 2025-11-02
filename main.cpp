#include <iostream>
#include <limits>
#include <iomanip>
#include "Inventory.h"

static void pause() {
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

static int readInt(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt);
        try { return std::stoi(s); } catch (...) { std::cout << "Please enter a valid integer.\n"; }
    }
}

static double readDouble(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt);
        try { return std::stod(s); } catch (...) { std::cout << "Please enter a valid number.\n"; }
    }
}

static void printItem(const Item& it) {
    std::cout << std::left << std::setw(5) << it.id
              << std::setw(20) << it.name
              << std::setw(15) << it.category
              << std::setw(12) << it.supplier
              << std::setw(8) << it.quantity
              << std::fixed << std::setprecision(2) << it.price << '\n';
}

int main() {
    Inventory inv;
    const std::string dataFile = "inventory.csv";
    inv.loadFromFile(dataFile); // ignore failure (file may not exist)

    while (true) {
        std::cout << "\nInventory Management\n"
                  << "1) List all items\n"
                  << "2) Add item\n"
                  << "3) Update item\n"
                  << "4) Remove item\n"
                  << "5) Search by name\n"
                  << "6) Low-stock report\n"
                  << "7) Import CSV\n"
                  << "8) Export CSV\n"
                  << "9) Save and Exit\n"
                  << "0) Exit without saving\n"
                  << "Select: ";
        std::string choice;
        std::getline(std::cin, choice);
        if (choice == "1") {
            auto list = inv.listAll();
            std::cout << std::left << std::setw(5) << "ID" << std::setw(20) << "Name" << std::setw(15) << "Category"
                      << std::setw(12) << "Supplier" << std::setw(8) << "Qty" << "Price\n";
            for (const auto& it : list) printItem(it);
            pause();
        } else if (choice == "2") {
            Item it;
            it.name = readLine("Name: ");
            it.category = readLine("Category: ");
            it.supplier = readLine("Supplier: ");
            it.quantity = readInt("Quantity: ");
            it.price = readDouble("Price: ");
            Item added = inv.addItem(it);
            std::cout << "Added with ID " << added.id << '\n';
            pause();
        } else if (choice == "3") {
            int id = readInt("Enter ID to update: ");
            auto existing = inv.getItem(id);
            if (!existing) { std::cout << "Not found.\n"; pause(); continue; }
            Item up = *existing;
            std::string s;
            s = readLine("Name (" + up.name + "): "); if (!s.empty()) up.name = s;
            s = readLine("Category (" + up.category + "): "); if (!s.empty()) up.category = s;
            s = readLine("Supplier (" + up.supplier + "): "); if (!s.empty()) up.supplier = s;
            s = readLine("Quantity (" + std::to_string(up.quantity) + "): "); if (!s.empty()) up.quantity = std::stoi(s);
            s = readLine("Price (" + std::to_string(up.price) + "): "); if (!s.empty()) up.price = std::stod(s);
            if (inv.updateItem(id, up)) std::cout << "Updated.\n"; else std::cout << "Update failed.\n";
            pause();
        } else if (choice == "4") {
            int id = readInt("Enter ID to remove: ");
            if (inv.removeItem(id)) std::cout << "Removed.\n"; else std::cout << "Not found.\n";
            pause();
        } else if (choice == "5") {
            std::string term = readLine("Search term: ");
            auto res = inv.searchByName(term);
            for (const auto& it : res) printItem(it);
            pause();
        } else if (choice == "6") {
            int thresh = readInt("Low stock threshold: ");
            auto res = inv.lowStock(thresh);
            for (const auto& it : res) printItem(it);
            pause();
        } else if (choice == "7") {
            std::string path = readLine("CSV path to import: ");
            if (inv.importCsv(path)) std::cout << "Imported.\n"; else std::cout << "Failed to import.\n";
            pause();
        } else if (choice == "8") {
            std::string path = readLine("CSV path to export: ");
            if (inv.exportCsv(path)) std::cout << "Exported.\n"; else std::cout << "Failed to export.\n";
            pause();
        } else if (choice == "9") {
            if (inv.saveToFile(dataFile)) std::cout << "Saved to " << dataFile << '\n';
            else std::cout << "Failed to save.\n";
            break;
        } else if (choice == "0") {
            std::cout << "Exit without saving.\n";
            break;
        } else {
            std::cout << "Unknown option.\n";
        }
    }

    return 0;
}