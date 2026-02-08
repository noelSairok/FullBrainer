// #include <iostream>
// #include <cmath>

// double w = 0.8;
// double b = 0.2;
// double a = 0.0;
// double leak = 0.8;

// double neuron(double x) {
//     double y = std::tanh(w * x + b + a * leak);
//     return y;
// }

// int main() {
//     for (double x : {-2, -1, 0, 1, 2}) {
//         std::cout << x << " => " << neuron(x) << std::endl;
//     }
// }


// #include <iostream>
// #include <cmath>
// #include <vector>
// #include <thread>
// #include <chrono>
// #include <random>
// #include <sqlite3.h>
// #include <string>


// // Here structires are defined.
// struct Connection {
//     int to;
//     double weight;
// };

// struct Neuron {
//     double activation;
//     double bias;
//     std::vector<Connection> out;
// };


// std::vector<Neuron> load_brain(sqlite3* db) {
//     std::vector<Neuron> brain;

//     // Load neurons
//     sqlite3_stmt* stmt;
//     sqlite3_prepare_v2(db, "SELECT * FROM neurons;", -1, &stmt, nullptr);

//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         Neuron n;
//         n.activation = sqlite3_column_double(stmt, 1);
//         n.bias = sqlite3_column_double(stmt, 2);
//         brain.push_back(n);
//     }
//     sqlite3_finalize(stmt);

//     // Load connections
//     sqlite3_prepare_v2(db, "SELECT * FROM connections;", -1, &stmt, nullptr);
//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         int from = sqlite3_column_int(stmt, 0);
//         int to   = sqlite3_column_int(stmt, 1);
//         double w = sqlite3_column_double(stmt, 2);
//         brain[from].out.push_back({to, w});
//     }
//     sqlite3_finalize(stmt);

//     return brain;
// }

// std::vector<Neuron> brain;

// void save_brain(sqlite3* db, const std::vector<Neuron>& brain) {
//     char* err = nullptr;

//     sqlite3_exec(db, "DELETE FROM neurons;", nullptr, nullptr, &err);
//     sqlite3_exec(db, "DELETE FROM connections;", nullptr, nullptr, &err);

//     for (int i = 0; i < brain.size(); i++) {
//         std::string q =
//             "INSERT INTO neurons VALUES (" +
//             std::to_string(i) + "," +
//             std::to_string(brain[i].activation) + "," +
//             std::to_string(brain[i].bias) + ");";
//         sqlite3_exec(db, q.c_str(), nullptr, nullptr, &err);

//         for (auto& c : brain[i].out) {
//             std::string q2 =
//                 "INSERT INTO connections VALUES (" +
//                 std::to_string(i) + "," +
//                 std::to_string(c.to) + "," +
//                 std::to_string(c.weight) + ");";
//             sqlite3_exec(db, q2.c_str(), nullptr, nullptr, &err);
//         }
//     }
// };

// int main() {
//     sqlite3* db;
//     sqlite3_open("brain.db", &db);

//     // create some brain
//     brain.resize(3);
//     brain[0].out.push_back({1, 0.5});
//     brain[1].out.push_back({2, -0.3});

//     save_brain(db, brain);

//     auto loaded = load_brain(db);

//     sqlite3_close(db);
// };

#include <iostream>
#include <vector>
#include <string>
#include <sqlite3.h>


// ---- STRUCTS FIRST ----

struct Connection {
    int to;
    double weight;
};

struct Neuron {
    double activation = 0.0;
    double bias = 0.0;
    std::vector<Connection> out;
};

// ---- GLOBAL BRAIN ----

std::vector<Neuron> brain;

// ---- LOAD ----

std::vector<Neuron> load_brain(sqlite3* db) {
    std::vector<Neuron> loaded_brain;

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT * FROM neurons;", -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Neuron n;
        n.activation = sqlite3_column_double(stmt, 1);
        n.bias = sqlite3_column_double(stmt, 2);
        loaded_brain.push_back(n);
    }
    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db, "SELECT * FROM connections;", -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int from = sqlite3_column_int(stmt, 0);
        int to   = sqlite3_column_int(stmt, 1);
        double w = sqlite3_column_double(stmt, 2);
        loaded_brain[from].out.push_back({to, w});
    }
    sqlite3_finalize(stmt);

    return loaded_brain;
};

// ---- SAVE ----

void save_brain(sqlite3* db, const std::vector<Neuron>& brain) {
    char* err = nullptr;

    sqlite3_exec(db, "DELETE FROM neurons;", nullptr, nullptr, &err);
    sqlite3_exec(db, "DELETE FROM connections;", nullptr, nullptr, &err);

    for (int i = 0; i < brain.size(); i++) {
        std::string q =
            "INSERT INTO neurons VALUES (" +
            std::to_string(i) + "," +
            std::to_string(brain[i].activation) + "," +
            std::to_string(brain[i].bias) + ");";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, &err);

        for (auto& c : brain[i].out) {
            std::string q2 =
                "INSERT INTO connections VALUES (" +
                std::to_string(i) + "," +
                std::to_string(c.to) + "," +
                std::to_string(c.weight) + ");";
            sqlite3_exec(db, q2.c_str(), nullptr, nullptr, &err);
        }
    }
};

// ---- MAIN ----

int main() {
    sqlite3* db;
    sqlite3_open("brain.db", &db);

    brain.resize(3);
    brain[0].out.push_back({1, 0.5});
    brain[1].out.push_back({2, -0.3});

    save_brain(db, brain);

    auto loaded = load_brain(db);

    std::cout << "Loaded neurons: " << loaded.size() << std::endl;

    sqlite3_close(db);
};



// // Those are the sizes of our network
// const int INPUTS = 3;
// const int NEURONS = 9;
// const int OUTPUTS = 3;


// // State of the network
// std::vector<double> input(INPUTS, 0.0);
// std::vector<double> a(NEURONS, 0.0); // Activations of the neurons
// std::vector<double> new_a(NEURONS, 0.0); // Temporary vector to store new activations


// // Parameters of the network
// std::vector<double> bias(NEURONS);
// std::vector<std::vector<double>> W(NEURONS, std::vector<double>(NEURONS));
// std::vector<std::vector<double>> Win(NEURONS, std::vector<double>(INPUTS));
// std::vector<std::vector<double>> Wout(OUTPUTS, std::vector<double>(NEURONS));

