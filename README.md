# inventory management c++

Simple console-based Inventory Management System written in modern C++ (C++20).  
This project uses a Visual Studio C++ project (`inventory management c++.vcxproj`) and persists data in `inventory.csv`.

## Features
- Add, update, remove items
- List with pagination and runtime sorting
- Search by name
- Low-stock report
- Import / export CSV
- Autosave and runtime settings (change data file, page size, autosave)

## Files
- `inventory_management.cpp` — CLI entry point with UI and settings.
- `Inventory.cpp` / `Inventory.h` — inventory data management and file I/O.
- `Item.h` — `Item` model and CSV (quote-aware) serialization/parsing.
- `inventory management c++.vcxproj` and `inventory management c++.vcxproj.filters` — Visual Studio project files.
- `inventory.csv` — runtime data file created when you save.

## Requirements
- Visual Studio 2022/2025/2026 with C++ toolset (project uses PlatformToolset `v145` in the .vcxproj; change if needed).
- C++20 (project uses `stdc++20` / MSVC conformance modes).
- Alternatively a command-line compiler supporting C++20 (g++/clang) for a simple build.

## Build & Run

### Visual Studio (recommended)
1. Open the project: __File > Open > Project/Solution__ and select `inventory management c++.vcxproj`.
2. Set the project as startup: right-click project in Solution Explorer → __Set as StartUp Project__.
3. Build: __Build > Rebuild Solution__.
4. Run without debugger: __Debug > Start Without Debugging__ (Ctrl+F5)  
   Or run with debugger: __Debug > Start Debugging__ (F5).

### Command line (MSVC Developer Prompt)
From the project folder:

### Command line (g++ / MinGW on Windows or Linux)
From the project folder: