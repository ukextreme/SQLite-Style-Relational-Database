// SQLite-style relational database — minimal core.
// Supports CREATE TABLE / INSERT / SELECT with a WHERE filter, over a B-tree-ordered store.
// Build: g++ -std=c++17 -O2 main.cpp -o minidb && ./minidb

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using Row = std::vector<std::string>;

struct Table {
    std::vector<std::string> columns;
    std::map<long long, Row> rows;  // ordered by primary key -> range scans are free
    long long next_key = 1;

    int indexOf(const std::string& col) const {
        for (size_t i = 0; i < columns.size(); ++i)
            if (columns[i] == col) return int(i);
        return -1;
    }
};

std::map<std::string, Table> g_tables;

std::vector<std::string> tokenize(const std::string& sql) {
    std::vector<std::string> out;
    std::string tok;
    for (char c : sql) {
        if (isspace((unsigned char)c) || c == ',' || c == '(' || c == ')') {
            if (!tok.empty()) { out.push_back(tok); tok.clear(); }
        } else {
            tok += c;
        }
    }
    if (!tok.empty()) out.push_back(tok);
    return out;
}

std::string upper(std::string s) {
    for (char& c : s) c = toupper((unsigned char)c);
    return s;
}

void doCreate(const std::vector<std::string>& t) {
    Table table;
    for (size_t i = 3; i < t.size(); ++i) table.columns.push_back(t[i]);
    g_tables[t[2]] = table;
    std::cout << "created " << t[2] << " with " << table.columns.size() << " columns\n";
}

void doInsert(const std::vector<std::string>& t) {
    auto it = g_tables.find(t[2]);
    if (it == g_tables.end()) { std::cout << "no such table: " << t[2] << "\n"; return; }
    Table& table = it->second;
    Row row;
    for (size_t i = 4; i < t.size(); ++i) row.push_back(t[i]);
    if (row.size() != table.columns.size()) { std::cout << "column count mismatch\n"; return; }
    table.rows[table.next_key++] = row;
    std::cout << "1 row inserted\n";
}

void doSelect(const std::vector<std::string>& t) {
    // SELECT * FROM <table> [WHERE <col> = <value>]
    size_t fromAt = 0;
    while (fromAt < t.size() && upper(t[fromAt]) != "FROM") ++fromAt;
    if (fromAt + 1 >= t.size()) { std::cout << "malformed select\n"; return; }

    auto it = g_tables.find(t[fromAt + 1]);
    if (it == g_tables.end()) { std::cout << "no such table: " << t[fromAt + 1] << "\n"; return; }
    const Table& table = it->second;

    int filterCol = -1;
    std::string filterVal;
    if (fromAt + 5 < t.size() && upper(t[fromAt + 2]) == "WHERE") {
        filterCol = table.indexOf(t[fromAt + 3]);
        filterVal = t[fromAt + 5];
    }

    for (const auto& col : table.columns) std::cout << col << "\t";
    std::cout << "\n";

    int matched = 0;
    for (const auto& [key, row] : table.rows) {
        if (filterCol >= 0 && row[filterCol] != filterVal) continue;
        for (const auto& cell : row) std::cout << cell << "\t";
        std::cout << "\n";
        ++matched;
    }
    std::cout << "(" << matched << " rows)\n";
}

void execute(const std::string& sql) {
    auto t = tokenize(sql);
    if (t.empty()) return;
    std::string verb = upper(t[0]);
    if (verb == "CREATE") doCreate(t);
    else if (verb == "INSERT") doInsert(t);
    else if (verb == "SELECT") doSelect(t);
    else std::cout << "unsupported statement: " << t[0] << "\n";
}

int main() {
    const char* demo[] = {
        "CREATE TABLE users (id, name, city)",
        "INSERT INTO users VALUES (1, ada, london)",
        "INSERT INTO users VALUES (2, linus, helsinki)",
        "INSERT INTO users VALUES (3, grace, newyork)",
        "SELECT * FROM users",
        "SELECT * FROM users WHERE city = helsinki",
    };
    for (const char* sql : demo) { std::cout << "> " << sql << "\n"; execute(sql); }

    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        if (upper(line) == ".EXIT") break;
        execute(line);
    }
    return 0;
}
