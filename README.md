# BankManagementSystem
A simple Bank Management System built in C++ with SQLite. Supports account registration, login, deposits, and withdrawals with balance updates stored in a database. Includes an admin panel to view and search accounts. Demonstrates OOP, input validation, and persistent data management.


# Bank Management System

## 📌 Project Description
This is a simple **Bank Management System** built in **C++** with **SQLite** as the database.  
It allows users to create accounts, log in, deposit, withdraw, and check balances.  
The goal of this project is to practice **OOP in C++**, **file/database handling**, and **basic system design**.

---

## 🚀 How to Run
1. Clone or download the project.
2. Make sure you have **SQLite3** installed (already included in this repo with `sqlite3.c` and `sqlite3.h`).
3. Compile using g++:
   ```bash
   g++ main.cpp sqlite3.c -o BankManagementSystem
````

4. Run the executable:

   ```bash
   ./BankManagementSystem
   ```

---

## 📚 What I Learned

* Fundamentals of **C++ OOP** (classes, objects, constructors).
* How to use **SQLite in C++** for data storage instead of text files.
* Linking external libraries (`sqlite3`) in a project.
* Debugging linker errors and memory management issues.

---

## 🔧 Improvements Needed

* Add **input validation** and error handling (e.g., wrong PIN, negative deposits).
* Implement **transaction history** for each account.
* Improve UI/UX (menus, cleaner output).
* Stronger **database design** (separate tables for users, transactions).
* Optionally, add **password encryption**.

---

## 🙌 Personal Note

This project taught me that building even a small system requires understanding how different parts (logic, database, compilation) fit together. I struggled with linker issues, pointers, and SQLite setup — but that’s exactly where the real learning happened.

```

