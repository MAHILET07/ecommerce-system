#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstdio>

using namespace std;

// ===================== STRUCTS =====================

struct Product {
    int id;
    string name;
    string category;
    float price;
    int quantity;
};

struct User {
    string username;
    string password;
    string role; // admin or customer
};

// ===================== FILE HELPERS =====================

string productToCSV(Product p) {
    stringstream ss;
    ss << p.id << "," << p.name << "," << p.category << ","
       << p.price << "," << p.quantity;
    return ss.str();
}

Product productFromCSV(string line) {
    Product p;
    stringstream ss(line);
    string temp;

    getline(ss, temp, ','); p.id = stoi(temp);
    getline(ss, p.name, ',');
    getline(ss, p.category, ',');
    getline(ss, temp, ','); p.price = stof(temp);
    getline(ss, temp, ','); p.quantity = stoi(temp);

    return p;
}

// ===================== AUTH SYSTEM =====================

bool registerUser() {
    ofstream file("users.txt", ios::app);

    User u;

    cout << "\n=== REGISTER CUSTOMER ===\n";
    cout << "Username: ";
    cin >> u.username;

    cout << "Password: ";
    cin >> u.password;

    u.role = "customer";

    file << u.username << " " << u.password << " " << u.role << "\n";

    cout << "\nAccount created successfully!\n";

    return true;
}

User loginUser() {
    ifstream file("users.txt");

    User input, u;

    cout << "\n=== LOGIN ===\n";
    cout << "Username: ";
    cin >> input.username;

    cout << "Password: ";
    cin >> input.password;

    while (file >> u.username >> u.password >> u.role) {
        if (u.username == input.username && u.password == input.password) {
            return u;
        }
    }

    return {"", "", ""};
}

// ===================== ADMIN FEATURES =====================

void addProduct() {
    ofstream file("products.csv", ios::app);

    Product p;

    cout << "\nID: ";
    cin >> p.id;
    cin.ignore();

    cout << "Name: ";
    getline(cin, p.name);

    cout << "Category: ";
    getline(cin, p.category);

    cout << "Price: ";
    cin >> p.price;

    cout << "Quantity: ";
    cin >> p.quantity;

    file << productToCSV(p) << "\n";

    cout << "\nProduct added!\n";
}

void viewProducts() {
    ifstream file("products.csv");

    string line;

    cout << "\n================ PRODUCT LIST ================\n";

    cout << left
         << setw(6) << "ID"
         << setw(20) << "NAME"
         << setw(15) << "CATEGORY"
         << setw(10) << "PRICE"
         << setw(10) << "STOCK" << "\n";

    cout << "----------------------------------------------\n";

    while (getline(file, line)) {
        Product p = productFromCSV(line);

        cout << left
             << setw(6) << p.id
             << setw(20) << p.name
             << setw(15) << p.category
             << setw(10) << p.price
             << setw(10) << p.quantity << "\n";
    }
}

void updateStock() {
    int id;
    bool found = false;

    cout << "\nEnter Product ID: ";
    cin >> id;

    ifstream file("products.csv");
    ofstream temp("temp.csv");

    string line;

    while (getline(file, line)) {
        Product p = productFromCSV(line);

        if (p.id == id) {
            found = true;

            cout << "New price: ";
            cin >> p.price;

            cout << "New quantity: ";
            cin >> p.quantity;
        }

        temp << productToCSV(p) << "\n";
    }

    file.close();
    temp.close();

    remove("products.csv");
    rename("temp.csv", "products.csv");

    cout << (found ? "\nUpdated successfully!\n" : "\nProduct not found!\n");
}

void salesReport() {
    ifstream file("sales.txt");

    float sale, total = 0;
    int orders = 0;

    while (file >> sale) {
        total += sale;
        orders++;
    }

    cout << "\n=========== SALES REPORT ===========\n";
    cout << "Total Orders : " << orders << "\n";
    cout << "Revenue      : " << total << "\n";
}

// ===================== CUSTOMER FEATURES =====================

void purchaseProduct(string username) {
    int id, qty;
    bool found = false;

    cout << "\nProduct ID: ";
    cin >> id;

    cout << "Quantity: ";
    cin >> qty;

    ifstream file("products.csv");
    ofstream temp("temp.csv");

    string line;

    while (getline(file, line)) {
        Product p = productFromCSV(line);

        if (p.id == id) {
            found = true;

            if (qty <= p.quantity) {
                float total = qty * p.price;
                float discount = (total > 50000) ? total * 0.10 : 0;
                float finalTotal = total - discount;

                p.quantity -= qty;

                cout << "\n=========== BILL ===========\n";
                cout << "Customer : " << username << "\n";
                cout << "Product  : " << p.name << "\n";
                cout << "Total    : " << finalTotal << "\n";

                ofstream orders("orders.csv", ios::app);
                orders << username << "," << p.id << "," << qty << "," << finalTotal << "\n";

                ofstream sales("sales.txt", ios::app);
                sales << finalTotal << "\n";

            } else {
                cout << "\nNot enough stock!\n";
            }
        }

        temp << productToCSV(p) << "\n";
    }

    file.close();
    temp.close();

    remove("products.csv");
    rename("temp.csv", "products.csv");

    if (!found)
        cout << "\nProduct not found!\n";
}

// ===================== PANELS =====================

void adminPanel() {
    int c;

    do {
        cout << "\n\n========= ADMIN PANEL =========\n";
        cout << "1. Add Product\n";
        cout << "2. View Products\n";
        cout << "3. Update Stock\n";
        cout << "4. Sales Report\n";
        cout << "5. Logout\n";
        cout << "Choice: ";
        cin >> c;

        switch (c) {
            case 1: addProduct(); break;
            case 2: viewProducts(); break;
            case 3: updateStock(); break;
            case 4: salesReport(); break;
        }

    } while (c != 5);
}

void customerPanel(User user) {
    int c;

    do {
        cout << "\n\n========= CUSTOMER PANEL =========\n";
        cout << "Logged in as: " << user.username << "\n";
        cout << "1. View Products\n";
        cout << "2. Purchase Product\n";
        cout << "3. Logout\n";
        cout << "Choice: ";
        cin >> c;

        switch (c) {
            case 1: viewProducts(); break;
            case 2: purchaseProduct(user.username); break;
        }

    } while (c != 3);
}

// ===================== MAIN =====================

int main() {
    int choice;

    do {
        cout << "\n=================================\n";
        cout << "     E-COMMERCE SYSTEM\n";
        cout << "=================================\n";
        cout << "1. Admin Login\n";
        cout << "2. Customer Login\n";
        cout << "3. Register Customer\n";
        cout << "4. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            string u, p;

            cout << "\nAdmin Username: ";
            cin >> u;

            cout << "Admin Password: ";
            cin >> p;

            if (u == "admin" && p == "1234")
                adminPanel();
            else
                cout << "\nInvalid admin login!\n";
        }

        else if (choice == 2) {
            User u = loginUser();

            if (u.username == "") {
                cout << "\nLogin failed!\n";
            }
            else if (u.role == "customer") {
                customerPanel(u);
            }
            else if (u.role == "admin") {
                adminPanel();
            }
        }

        else if (choice == 3) {
            registerUser();
        }

    } while (choice != 4);

    return 0;
}
