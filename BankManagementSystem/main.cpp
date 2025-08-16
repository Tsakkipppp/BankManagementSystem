#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <regex>
#include <limits>
#include <functional>
#include <algorithm>
#include "sqlite3.h"


class Account {
    public:
    int accId;
    std::string username;
    double initialDeposit;
    std::string pinNum;
Account(int accId_, std::string username_, double initialDeposit_, std::string pinNum_)
  : accId(accId_), username(username_), initialDeposit(initialDeposit_), pinNum(pinNum_){}
};

std::vector<Account> accountList;
std::regex namePattern("^[A-Za-z ]+$");
std::regex pinPattern("^[0-9]+$");
int loginIndex;
void mainMenu(sqlite3* db);
void datatoVector(sqlite3* db);

//callback for sqlite
static int callback(void* data, int argc, char** argv, char** AzColName){
    const char* msg = static_cast<const char*>(data);
    int id = 0;
    std::string usernameRetrieved = "", pinRetrieved = "";
    double depositRetrieved;
    if(argv[0] != nullptr && argv[1] != nullptr && argv[2] != nullptr && argv[3] != nullptr){
        id = std::stoi(argv[0]);
        usernameRetrieved = argv[1];
        pinRetrieved = argv[2];
        depositRetrieved = std::stod(argv[3]);
    }else{
        std::cout << "Error Retrieving DATA" << std::endl;
        return -1;
    }
    Account a(id, usernameRetrieved, depositRetrieved, pinRetrieved);
    accountList.push_back(a);
    return 0;
};

//input wrapper system
template <typename C>
C getInput(const std:: string& prompt, std::function <bool (C)> validator, const std:: string& errorMsg){
    C value;
    while(true){
        std::cout << prompt;
        std::cin >> value;

        if(std::cin.fail()){
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input type.Please enter correct input." << std::endl;
            continue;
        }
        if(validator(value)){
            std::cin.ignore(10000, '\n');
            return value;
        }else{
            std::cout << errorMsg << std::endl;
        }
    }
}

//return last id
int returnId(){
    auto it = std::max_element(accountList.begin(), accountList.end(),
      [](const Account &a, Account &b){
        return a.accId < b.accId;
      }
    );
    if(it != accountList.end()){
        return it->accId;
    }else{
        std::cout << "Error retrieving id iterator" << std::endl;
        return -1;
    }
}

std::string getString(const std::string& prompt, std::function <bool(const std::string&)> validator, const std::string& errorMsg){
    std::string value;
    while(true){
        std::cout << prompt;
        std::getline(std::cin, value);
        if(validator(value)){
            return value;
        }else{
            std::cout << errorMsg << std::endl;
        }
    }
}


//add acc function
void addAccount(sqlite3* db){
    static int iterateId = 0;
    std::string newUsername, newPin;
    double newDeposit;
    char* errorMsg;
    std::cin.ignore(10000, '\n');
    newUsername = getString(
        "Enter New Username: ",
        [](const std::string& val){return !val.empty() && std::regex_match(val, namePattern);},
        "Name cannot be empty.Enter Proper Name."
    );
    newPin = getString(
        "Enter New pin: ",
        [](const std::string& val){return !val.empty() && std::regex_match(val, pinPattern);},
        "Error in entering pin.Please enter proper pin!"
    );
    newDeposit = getInput<double>(
        "Enter your initial deposit (At least RM10.00):RM",
        [](double val){return val >= 10.00;},
        "Error: Please enter correct amount."
    );
    iterateId = returnId() + 1;
    accountList.emplace_back(iterateId, newUsername, newDeposit, newPin);
    std::cout << "New account created: " << std::endl;
    std::cout << iterateId << " " << newUsername << " " << newPin << " RM" << std::fixed << std::setprecision(2) << newDeposit << std::endl;
    
    std::string sql = "INSERT INTO account (username, pin, balance)  VALUES ('" + newUsername + "', '"+ newPin + "', '"+std::to_string(newDeposit)+"');";
    int insertion = sqlite3_exec(db, sql.c_str(), 0, 0, &errorMsg);
    if(insertion != SQLITE_OK){
        std::cout << "Fail Insertion: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    }else{
        std::cout << "Good,data insertion success!" << std::endl;
    }
    mainMenu(db);

}

void adminMenu(sqlite3* db){
    const int Adminpin = 23456;
    int pinInput;
    bool adminLoop = true;
    std::cout << "Enter admin pin: ";
    std::cin >> pinInput;
    if(Adminpin == pinInput){
        while(adminLoop){
            int adminOption;
        std::cout << "Welcome, admin" << std::endl;
        std::cout << "1. View all account info\n2. Search Account By Id\n3. Logout" << std::endl;
        std::cin >> adminOption;
        if(adminOption == 1){
            //view acc function
            for(int x = 0; x < accountList.size(); x++){
                std::cout << accountList[x].accId << " " << accountList[x].username << " " << accountList[x].pinNum << " RM" << std::fixed << std::setprecision(2) << accountList[x].initialDeposit << std::endl;
            }
            system("pause");
        }else if(adminOption == 2){
            //searchOption
            int searchId;
            std::cout << "Enter acc id: ";
            std::cin >>  searchId;
            for(int x = 0; x < accountList.size(); x++){
                if(accountList[x].accId == searchId){
                  std::cout << accountList[x].accId << " " << accountList[x].username << " " << accountList[x].pinNum << " RM" << std::fixed << std::setprecision(2) << accountList[x].initialDeposit << std::endl;
                }
            }
            system("pause");
        }else if(adminOption == 3){
            adminLoop = false;
            mainMenu(db);
        }else{
            std::cout << "Invalid option." << std::endl;
        }
        }
    }else{
        std::cout << "Invalid pin!Please dont do that" << std::endl;
    }
}

//withdraw function
void withdrawMoney(sqlite3* db){
    int idLogin = accountList[loginIndex].accId;
    char* errorMsg;
    double withdrawAmount;
    withdrawAmount = getInput<double>(
        "Enter your withdrawal amount:RM",
        [](double val){return val <= accountList[loginIndex].initialDeposit && val >= 0.01;},
        "Error,your withdrawal exceed your deposit"
    );
    accountList[loginIndex].initialDeposit -= withdrawAmount;
    std::string updateStr = "UPDATE account SET balance = '"+std::to_string(accountList[loginIndex].initialDeposit)+"' WHERE id = '"+ std::to_string(idLogin) + "';";
    const char* update = updateStr.c_str();
    int query = sqlite3_exec(db, update, 0, 0, &errorMsg);
    if(query != SQLITE_OK){
        std::cerr << errorMsg << std::endl;
        sqlite3_free(errorMsg); 
    }else{
        std::cout << "Withdrawal Successful,Current Balance: RM" << accountList[loginIndex].initialDeposit << std::endl;
        datatoVector(db); 
    }
    system("pause");
}

//deposit function
void depositFunction(sqlite3* db){
    char* errorMsg;
    int idLogin = accountList[loginIndex].accId;
    double depositAmount = getInput<double>(
        "Enter deposit amount: RM",
        [](double val){return val != 0;},
        "Error,please enter correct amount for deposit."
    );
    accountList[loginIndex].initialDeposit += depositAmount;
    std::string updateStr = "UPDATE account SET balance = '"+std::to_string(accountList[loginIndex].initialDeposit)+"' WHERE id = '"+ std::to_string(idLogin) + "';";
    const char* update = updateStr.c_str();
    int query = sqlite3_exec(db, update, 0, 0, &errorMsg);
    if(query != SQLITE_OK){
        std::cerr << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    }else{
        std::cout << "Deposit Successful,Current Balance: RM" << accountList[loginIndex].initialDeposit << std::endl;
        datatoVector(db); 
    }
}

void loginMenu(sqlite3* db){
    int userOption;
    bool loginLoop = true;
    while(loginLoop){
        std::cout << "1. Withdraw Money\n2. Deposit Money\n3. Logout" << std::endl;
    std::cin >> userOption;
    switch(userOption){
        case 1:
        withdrawMoney(db);
        break;
        case 2:
        depositFunction(db);
        break;
        case 3:
        mainMenu(db);
        loginLoop = false;
        break;
        default:
        std:: cout << "Please Enter correct option" << std::endl;
        break;
    }
    };

}

void logIn(sqlite3* db){
    std::string nameLogin, pinLogin;
    int idLogin = 0;
    std::cin.ignore(10000, '\n');
    nameLogin = getString(
        "Enter your username: ",
        [](std::string val){return std::regex_match(val, namePattern) && !val.empty();},
        "Enter correct username"
    );
    pinLogin = getString(
        "Enter your pin: ",
        [](std::string val){return std::regex_match(val, pinPattern) && !val.empty();},
        "Enter correct pin"
    );

    for(int x = 0; x < accountList.size(); x++){
        if(accountList[x].username == nameLogin && accountList[x].pinNum == pinLogin){
            std::cout << "Welcome," << accountList[x].username << std::endl;
            loginIndex = x;
            idLogin = accountList[x].accId;
            loginMenu(db);
            break;
        }else if(x == accountList.size()){
            std::cout << "Please enter valid username and/or pin" << std::endl;
            exit(1);
        }
    }
    
}

void mainMenu(sqlite3* db){
    int optionLog;
    std::cout << "Welcome To Bank Management System!\n" << std::endl;
    std::cout << "1. Login Account\n2. Register Account\n3. Administration(FOR ADMIN ONLY)" << std::endl;
    std::cin >> optionLog;
    if(optionLog == 1){
        logIn(db);
    }else if(optionLog == 2){
        addAccount(db);
    }else if(optionLog == 3){
        //admin function
        adminMenu(db);
    }else if(optionLog == 0){
        
    }else{
        std::cout << "Invalid Option" << std::endl;
    }

}

void datatoVector(sqlite3* db){
    char* errorMsg;
    accountList.clear();
    const char* retrieveQuery = "SELECT * FROM account;";
    int execute = sqlite3_exec(db, retrieveQuery, callback, nullptr, &errorMsg);
    if(execute != SQLITE_OK){
        std::cout << "Error Retrieving Data: " << errorMsg << std::endl;
        sqlite3_free(&errorMsg);
    }else{
        std::cout << "Aight lesgo!" << std::endl;
    }
}

int main(){
    sqlite3* db;
    int exit = sqlite3_open("bankmanagement.db", &db);
    char* errorMsg;

    if(exit){
        std::cerr << "Error connecting db: " << sqlite3_errmsg(db) << std::endl;
    }else{
        std::cout << "DB OK!" << std::endl;
    }

    //create Account table
    const char* createTable = 
    "CREATE TABLE IF NOT EXISTS account ("
    "id INTEGER PRIMARY KEY, "
    "username TEXT NOT NULL,"
    "pin TEXT NOT NULL, "
    "balance DOUBLE);";
    int createQueries = sqlite3_exec(db, createTable, 0, 0, &errorMsg);
    if(createQueries != SQLITE_OK){
        std::cerr << "Error Executing Queries: " << errorMsg << std::endl;
        sqlite3_free(&errorMsg);
    }else{
        std::cout << "Table created" << std::endl;
    }
    datatoVector(db);
    mainMenu(db);
    system("pause");
    return 0;
}