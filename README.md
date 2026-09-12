# SQLite-Style Relational Database

A small relational database engine in C++.

This repository holds the **minimal engine**: a SQL tokenizer, `CREATE TABLE` / `INSERT` /
`SELECT ... WHERE`, and an ordered key-value row store so range scans come for free.

```bash
g++ -std=c++17 -O2 main.cpp -o minidb
./minidb
```

It runs a short demo script, then drops into a REPL (`.exit` to quit):

```
> SELECT * FROM users WHERE city = helsinki
id  name   city
2   linus  helsinki
(1 rows)
```

## Status

Core only. The on-disk pager, B-tree cursors, joins/grouping, and ARIES recovery are not part of
this repository.
