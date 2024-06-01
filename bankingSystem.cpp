#include <iostream>
#include <fstream>
#include <map>
#include <string>
using namespace std;

#define MIN_BALANCE 500

class InsufficientFunds {};
class AccountNotFound {};

class Account {
private:
    long accountNumber;
    string firstName;
    string lastName;
    float balance;
    static long nextAccountNumber;

public:
    Account(){};
    Account(string fName, string lName, float balance);
    long getAccountNumber() { return accountNumber; }
    string getFirstName() { return firstName; }
    string getLastName() { return lastName; }
    float getBalance() { return balance; }

    void deposit(float amount);
    void withdraw(float amount);
    static void setLastAccountNumber(long accountNumber);
    static long getLastAccountNumber();
    friend ofstream& operator<<(ofstream& ofs, Account& acc);
    friend ifstream& operator>>(ifstream& ifs, Account& acc);
    friend ostream& operator<<(ostream& os, Account& acc);
}; long Account::nextAccountNumber = 0;

class Bank {
private:
    map<long, Account> accountList;

public:
    Bank();
    Account createAccount(string fName, string lName, float balance);
    Account getBalance(long accountNumber);
    Account deposit(long accountNumber, float amount);
    Account withdraw(long accountNumber, float amount);
    void deleteAccount(long accountNumber);
    void displayAllAccounts();
    void saveToFile();
    ~Bank();
};

int main() {
    Bank bank;
    Account account;

    int userChoice;
    string fName, lName;
    long accountNumber;
    float balance;
    float amount;
    cout << "*** Welcome to the Banking System ***" << endl;
    
    while (userChoice != 7) {
        cout << "\n\tPlease choose an option: ";
        cout << "\n\t1. Open an Account";
        cout << "\n\t2. Check Account Balance";
        cout << "\n\t3. Deposit Money";
        cout << "\n\t4. Withdraw Money";
        cout << "\n\t5. Close an Account";
        cout << "\n\t6. View All Accounts";
        cout << "\n\t7. Exit";
        cout << "\nEnter your choice: ";
        cin >> userChoice;
        
        switch (userChoice) {
            case 1:
                cout << "Enter First Name: ";
                cin >> fName;
                cout << "Enter Last Name: ";
                cin >> lName;
                cout << "Enter Initial Balance: ";
                cin >> balance;
                try {
                    account = bank.createAccount(fName, lName, balance);
                    cout << endl << "Congratulations! Your account has been created." << endl;
                    cout << account;
                } catch (InsufficientFunds) {
                    cout << "Initial balance must be at least the minimum balance requirement." << endl;
                }
                break;

            case 2:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                try {
                    account = bank.getBalance(accountNumber);
                    cout << endl << "Your Account Details:" << endl;
                    cout << account;
                } catch (AccountNotFound) {
                    cout << "Account not found." << endl;
                }
                break;

            case 3:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                cout << "Enter Amount to Deposit: ";
                cin >> amount;
                try {
                    account = bank.deposit(accountNumber, amount);
                    cout << endl << "Amount Deposited Successfully" << endl;
                    cout << account;
                } catch (AccountNotFound) {
                    cout << "Account not found." << endl;
                }
                break;

            case 4:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                cout << "Enter Amount to Withdraw: ";
                cin >> amount;
                try {
                    account = bank.withdraw(accountNumber, amount);
                    cout << endl << "Amount Withdrawn Successfully" << endl;
                    cout << account;
                } catch (AccountNotFound) {
                    cout << "Account not found." << endl;
                } catch (InsufficientFunds) {
                    cout << "Insufficient funds." << endl;
                }
                break;

            case 5:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                try {
                    bank.deleteAccount(accountNumber);
                    cout << endl << "Account Closed Successfully" << endl;
                } catch (AccountNotFound) {
                    cout << "Account not found." << endl;
                }
                break;

            case 6:
                bank.displayAllAccounts();
                break;

            case 7:
                // cout << "Exiting the system. Goodbye!" << endl;
                break;

            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
    }
    
    return 0;
}

Account::Account(string fName, string lName, float balance) {
    if (balance < MIN_BALANCE) {
        throw InsufficientFunds();
    }
    nextAccountNumber++;
    accountNumber = nextAccountNumber;
    firstName = fName;
    lastName = lName;
    this->balance = balance;
}

void Account::deposit(float amount) {
    balance += amount;
}

void Account::withdraw(float amount) {
    if (balance - amount < MIN_BALANCE) {
        throw InsufficientFunds();
    }
    balance -= amount;
}

void Account::setLastAccountNumber(long accountNumber) {
    nextAccountNumber = accountNumber;
}

long Account::getLastAccountNumber() {
    return nextAccountNumber;
}

ofstream& operator<<(ofstream& ofs, Account& acc) {
    ofs << acc.accountNumber << endl;
    ofs << acc.firstName << endl;
    ofs << acc.lastName << endl;
    ofs << acc.balance << endl;
    return ofs;
}

ifstream& operator>>(ifstream& ifs, Account& acc) {
    ifs >> acc.accountNumber;
    ifs >> acc.firstName;
    ifs >> acc.lastName;
    ifs >> acc.balance;
    return ifs;
}

ostream& operator<<(ostream& os, Account& acc) {
    os << "First Name: " << acc.getFirstName() << endl;
    os << "Last Name: " << acc.getLastName() << endl;
    os << "Account Number: " << acc.getAccountNumber() << endl;
    os << "Balance: " << acc.getBalance() << endl;
    return os;
}

Bank::Bank() {
    Account account;
    ifstream file("bankRecords.txt");
    if (!file) {
        return;
    }
    while (file >> account) {
        accountList.insert({account.getAccountNumber(), account});
    }
    Account::setLastAccountNumber(account.getAccountNumber());
    file.close();
}

Account Bank::createAccount(string fName, string lName, float balance) {
    Account account(fName, lName, balance);
    accountList.insert({account.getAccountNumber(), account});
    return account;
}

Account Bank::getBalance(long accountNumber) {
    auto itr = accountList.find(accountNumber);
    if (itr != accountList.end()) {
        return itr->second;
    }
    throw AccountNotFound();
}

Account Bank::deposit(long accountNumber, float amount) {
    auto itr = accountList.find(accountNumber);
    if (itr != accountList.end()) {
        itr->second.deposit(amount);
        return itr->second;
    }
    throw AccountNotFound();
}

Account Bank::withdraw(long accountNumber, float amount) {
    auto itr = accountList.find(accountNumber);
    if (itr != accountList.end()) {
        try {
            itr->second.withdraw(amount);
            return itr->second;
        } catch (const InsufficientFunds&) {
            throw;
        }
    }
    throw AccountNotFound();
}

void Bank::deleteAccount(long accountNumber) {
    auto itr = accountList.find(accountNumber);
    if (itr != accountList.end()) {
        cout << "Account Deleted: " << itr->second;
        accountList.erase(accountNumber);
    } else {
        throw AccountNotFound();
    }
}

void Bank::displayAllAccounts() {
    for (auto &entry : accountList) {
        cout << "Account " << entry.first << endl << entry.second << endl;
    }
}

void Bank::saveToFile() {
    ofstream file("bankRecords.txt", ios::trunc);
    for (auto &entry : accountList) {
        file << entry.second;
    }
    file.close();
}

Bank::~Bank() {
    saveToFile();
}