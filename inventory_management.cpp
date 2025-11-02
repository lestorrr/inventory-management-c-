#include <iostream>
#include <limits>
#include <iomanip>
#include <string>
#include <algorithm>
#include <vector>
#include "Inventory.h"

#ifdef _WIN32
static void clearScreen() { std::system("cls"); }
#else
static void clearScreen() { std::system("clear"); }
#endif

struct Settings {
    std::string dataFile = "inventory.csv";
    int pageSize = 10;
    bool autosave = false;
    std::string sortField = "id"; // id, name, category, supplier, quantity, price
    std::string configFile = "config.json"; // new line for config file
} settings;

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

static void printItemRow(const Item& it) {
    std::cout << std::left << std::setw(5) << it.id
              << std::setw(20) << it.name.substr(0, 19)
              << std::setw(15) << it.category.substr(0, 14)
              << std::setw(12) << it.supplier.substr(0, 11)
              << std::setw(8) << it.quantity
              << std::fixed << std::setprecision(2) << it.price << '\n';
}

static void showHeader() {
    std::cout << "=== Inventory Management ===\n";
    std::cout << "Data file: " << settings.dataFile
              << " | Page size: " << settings.pageSize
              << " | Autosave: " << (settings.autosave ? "On" : "Off")
              << " | Sort: " << settings.sortField << "\n\n";
}

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

static void sortItems(std::vector<Item>& items, const std::string& field) {
    if (field == "id") {
        std::stable_sort(items.begin(), items.end(), [](const Item& a, const Item& b){ return a.id < b.id; });
    } else if (field == "name") {
        std::stable_sort(items.begin(), items.end(), [](const Item& a, const Item& b){ return toLower(a.name) < toLower(b.name); });
    } else if (field == "category") {
        std::stable_sort(items.begin(), items.end(), [](const Item& a, const Item& b){ return toLower(a.category) < toLower(b.category); });
    } else if (field == "supplier") {
        std::stable_sort(items.begin(), items.end(), [](const Item& a, const Item& b){ return toLower(a.supplier) < toLower(b.supplier); });
    } else if (field == "quantity") {
        std::stable_sort(items.begin(), items.end(), [](const Item& a, const Item& b){ return a.quantity < b.quantity; });
    } else if (field == "price") {
        std::stable_sort(items.begin(), items.end(), [](const Item& a, const Item& b){ return a.price < b.price; });
    }
}

static void listUI(Inventory& inv) {
    auto items = inv.listAll();
    if (items.empty()) {
        std::cout << "Inventory is empty.\n";
        pause();
        return;
    }

    sortItems(items, settings.sortField);

    int pageSize = std::max(1, settings.pageSize);
    int total = static_cast<int>(items.size());
    int pages = (total + pageSize - 1) / pageSize;
    int page = 0;

    while (true) {
        clearScreen();
        showHeader();
        std::cout << std::left << std::setw(5) << "ID" << std::setw(20) << "Name" << std::setw(15) << "Category"
                  << std::setw(12) << "Supplier" << std::setw(8) << "Qty" << "Price\n";
        std::cout << std::string(70, '-') << '\n';

        int start = page * pageSize;
        int end = std::min(total, start + pageSize);
        for (int i = start; i < end; ++i) printItemRow(items[i]);

        std::cout << "\nPage " << (page + 1) << " / " << pages << "  (Enter=next, b=prev, s=sort, g=goto, q=quit)\n";
        std::string cmd = readLine("Command: ");
        if (cmd.empty() || cmd == "\n") { page++; if (page >= pages) break; }
        else if (cmd == "b") { if (page > 0) --page; }
        else if (cmd == "q") break;
        else if (cmd == "g") {
            int p = readInt("Goto page #: ") - 1;
            if (p >= 0 && p < pages) page = p;
        }
        else if (cmd == "s") {
            std::cout << "Sort by: 1) id 2) name 3) category 4) supplier 5) quantity 6) price\n";
            std::string opt = readLine("Choose: ");
            if (opt == "1") settings.sortField = "id";
            else if (opt == "2") settings.sortField = "name";
            else if (opt == "3") settings.sortField = "category";
            else if (opt == "4") settings.sortField = "supplier";
            else if (opt == "5") settings.sortField = "quantity";
            else if (opt == "6") settings.sortField = "price";
            sortItems(items, settings.sortField);
            page = 0;
        }
    }
}

static void settingsUI(Inventory& inv) {
    while (true) {
        clearScreen();
        showHeader();
        std::cout << "Settings\n";
        std::cout << "1) Data file: " << settings.dataFile << '\n';
        std::cout << "2) Page size: " << settings.pageSize << '\n';
        std::cout << "3) Autosave: " << (settings.autosave ? "On" : "Off") << '\n';
        std::cout << "4) Reload data file now\n";
        std::cout << "5) Config file: " << settings.configFile << '\n'; // new config option
        std::cout << "0) Back\n";
        std::string c = readLine("Choose: ");
        if (c == "1") {
            std::string p = readLine("New data file path: ");
            if (!p.empty()) {
                settings.dataFile = p;
                inv.loadFromFile(settings.dataFile);
                std::cout << "Loaded " << settings.dataFile << '\n';
                pause();
            }
        } else if (c == "2") {
            int ps = readInt("New page size: ");
            settings.pageSize = std::max(1, ps);
        } else if (c == "3") {
            settings.autosave = !settings.autosave;
            std::cout << "Autosave is now " << (settings.autosave ? "On" : "Off") << '\n';
            pause();
        } else if (c == "4") {
            if (inv.loadFromFile(settings.dataFile)) std::cout << "Reloaded.\n"; else std::cout << "Reload failed.\n";
            pause();
        } else if (c == "5") { // config file handling
            std::string newConfig = readLine("Enter new config file path: ");
            if (!newConfig.empty()) {
                settings.configFile = newConfig;
                std::cout << "Config file set to " << settings.configFile << '\n';
                pause();
            }
        } else if (c == "0") break;
    }
}

int main() {
    Inventory inv;
    inv.loadFromFile(settings.dataFile); // ignore failure (file may not exist)

    // Persist settings: load from config file if exists
    if (std::ifstream configIn(settings.configFile)) {
        nlohmann::json j;
        configIn >> j;
        settings.dataFile = j.value("dataFile", settings.dataFile);
        settings.pageSize = j.value("pageSize", settings.pageSize);
        settings.autosave = j.value("autosave", settings.autosave);
        settings.sortField = j.value("sortField", settings.sortField);
        configIn.close();
    }

    while (true) {
        clearScreen();
        showHeader();
        std::cout << "1) List all items\n"
                  << "2) Add item\n"
                  << "3) Update item\n"
                  << "4) Remove item\n"
                  << "5) Search by name\n"
                  << "6) Low-stock report\n"
                  << "7) Import CSV\n"
                  << "8) Export CSV\n"
                  << "9) Save and Exit\n"
                  << "10) Settings\n"
                  << "0) Exit without saving\n"
                  << "Select: ";
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            listUI(inv);
        } else if (choice == "2") {
            Item it;
            it.name = readLine("Name: ");
            it.category = readLine("Category: ");
            it.supplier = readLine("Supplier: ");
            it.quantity = readInt("Quantity: ");
            it.price = readDouble("Price: ");
            Item added = inv.addItem(it);
            std::cout << "Added with ID " << added.id << '\n';
            if (settings.autosave) inv.saveToFile(settings.dataFile);
            pause();
        } else if (choice == "3") {
            int id = readInt("ID to update: ");
            auto maybe = inv.getItem(id);
            if (!maybe) { std::cout << "Item not found.\n"; pause(); continue; }
            Item updated = *maybe;
            std::string s;
            s = readLine("Name (" + updated.name + "): "); if (!s.empty()) updated.name = s;
            s = readLine("Category (" + updated.category + "): "); if (!s.empty()) updated.category = s;
            s = readLine("Supplier (" + updated.supplier + "): "); if (!s.empty()) updated.supplier = s;
            s = readLine("Quantity (" + std::to_string(updated.quantity) + "): ");
            if (!s.empty()) updated.quantity = std::stoi(s);
            s = readLine("Price (" + std::to_string(updated.price) + "): ");
            if (!s.empty()) updated.price = std::stod(s);
            if (inv.updateItem(id, updated)) {
                std::cout << "Updated.\n";
                if (settings.autosave) inv.saveToFile(settings.dataFile);
            } else std::cout << "Update failed.\n";
            pause();
        } else if (choice == "4") {
            int id = readInt("ID to remove: ");
            if (inv.removeItem(id)) {
                std::cout << "Removed.\n";
                if (settings.autosave) inv.saveToFile(settings.dataFile);
            } else std::cout << "Item not found.\n";
            pause();
        } else if (choice == "5") {
            std::string term = readLine("Search term: ");
            auto results = inv.searchByName(term);
            if (results.empty()) std::cout << "No matches.\n"; else {
                std::cout << std::left << std::setw(5) << "ID" << std::setw(20) << "Name" << std::setw(15) << "Category"
                          << std::setw(12) << "Supplier" << std::setw(8) << "Qty" << "Price\n";
                for (const auto& it : results) printItemRow(it);
            }
            pause();
        } else if (choice == "6") {
            int thresh = readInt("Low-stock threshold: ");
            auto list = inv.lowStock(thresh);
            if (list.empty()) std::cout << "No items at or below threshold.\n";
            else {
                std::cout << std::left << std::setw(5) << "ID" << std::setw(20) << "Name" << std::setw(15) << "Category"
                          << std::setw(12) << "Supplier" << std::setw(8) << "Qty" << "Price\n";
                for (const auto& it : list) printItemRow(it);
            }
            pause();
        } else if (choice == "7") {
            std::string path = readLine("CSV path to import: ");
            if (inv.importCsv(path)) std::cout << "Imported.\n"; else std::cout << "Import failed.\n";
            if (settings.autosave) inv.saveToFile(settings.dataFile);
            pause();
        } else if (choice == "8") {
            std::string path = readLine("CSV path to export: ");
            if (inv.exportCsv(path)) std::cout << "Exported.\n"; else std::cout << "Export failed.\n";
            pause();
        } else if (choice == "9") {
            if (inv.saveToFile(settings.dataFile)) std::cout << "Saved to " << settings.dataFile << '\n'; else std::cout << "Save failed.\n";
            pause();
            break;
        } else if (choice == "10") {
            settingsUI(inv);
        } else if (choice == "0") {
            break;
        } else {
            std::cout << "Unknown option.\n";
            pause();
        }
    }

    // Persist settings: save to config file on exit
    nlohmann::json j;
    j["dataFile"] = settings.dataFile;
    j["pageSize"] = settings.pageSize;
    j["autosave"] = settings.autosave;
    j["sortField"] = settings.sortField;
    std::ofstream configOut(settings.configFile);
    configOut << j.dump(4);
    configOut.close();

    return 0;
}

## Extending
- Persist runtime settings to a small JSON/INI file.
- Add a GUI (Win32/WinForms/wxWidgets/Qt) or a lightweight web interface.
- Add unit tests and CI (GitHub Actions) to build on push.

## License
Pick a license for your repo (MIT recommended for permissive use). Add a `LICENSE` file.

---

If you want, I can:
- Create a complete `README.md` file in your repo (I created this content for you to save),
- Add a `.gitignore`, `LICENSE`, and a GitHub Actions workflow that builds the project on push.