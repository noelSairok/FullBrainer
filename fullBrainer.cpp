//  /$$$$$$$                                               /$$           /$$ /$$ /$$$$$$$$        /$$ /$$         /$$$$$$$                     /$$                               /$$ /$$
// | $$__  $$                                             | $$          | $$| $$| $$_____/       | $$| $$        | $$__  $$                   |__/                              | $$| $$
// | $$  \ $$ /$$$$$$   /$$$$$$  /$$  /$$$$$$   /$$$$$$$ /$$$$$$        | $$| $$| $$    /$$   /$$| $$| $$        | $$  \ $$  /$$$$$$  /$$$$$$  /$$ /$$$$$$$   /$$$$$$   /$$$$$$ | $$| $$
// | $$$$$$$//$$__  $$ /$$__  $$|__/ /$$__  $$ /$$_____/|_  $$_/        |__/|__/| $$$$$| $$  | $$| $$| $$ /$$$$$$| $$$$$$$  /$$__  $$|____  $$| $$| $$__  $$ /$$__  $$ /$$__  $$|__/|__/
// | $$____/| $$  \__/| $$  \ $$ /$$| $$$$$$$$| $$        | $$                  | $$__/| $$  | $$| $$| $$|______/| $$__  $$| $$  \__/ /$$$$$$$| $$| $$  \ $$| $$$$$$$$| $$  \__/        
// | $$     | $$      | $$  | $$| $$| $$_____/| $$        | $$ /$$              | $$   | $$  | $$| $$| $$        | $$  \ $$| $$      /$$__  $$| $$| $$  | $$| $$_____/| $$              
// | $$     | $$      |  $$$$$$/| $$|  $$$$$$$|  $$$$$$$  |  $$$$/              | $$   |  $$$$$$/| $$| $$        | $$$$$$$/| $$     |  $$$$$$$| $$| $$  | $$|  $$$$$$$| $$              
// |__/     |__/       \______/ | $$ \_______/ \_______/   \___/                |__/    \______/ |__/|__/        |_______/ |__/      \_______/|__/|__/  |__/ \_______/|__/              
//                         /$$  | $$                                                                                                                                                    
//                        |  $$$$$$/                                                                                                                                                    
//                         \______/                                                                                                                                                     

#include <iostream>
#include <vector>
#include <string>
#include <sqlite3.h>

// ░▒▓███████▓▒░░▒▓███████▓▒░ ░▒▓██████▓▒░░▒▓█▓▒░▒▓███████▓▒░         
// ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓██▓▒░ 
// ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓██▓▒░ 
// ░▒▓███████▓▒░░▒▓███████▓▒░░▒▓████████▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░        
// ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓██▓▒░ 
// ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓██▓▒░ 
// ░▒▓███████▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░        

// ---- BRAIN STRUCTIONS ----

struct Connection {
    int to;
    double weight;
};

struct Neuron {
    double activation = 0.0;
    double bias = 0.0;
    std::vector<Connection> out;
};

struct Brain {
    std::vector<Neuron> neurons;
};

// ###################################################################################
//                          <===== BRAIN FUNCTIONS =====>
// ###################################################################################

// ---- LOAD FUNCTION ----

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

// ---- SAVE FUNCTION ----

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

// ---- RANDOM BRAIN CREATION FUNCTION ----
Brain create_random_brain(int inputs, int outputs, int min_neurons, int max_neurons) {
    Brain brain;
    int neuron_count = min_neurons + rand() % (max_neurons - min_neurons + 1);
    
    brain.neurons.resize(neuron_count);
    
    for (int i = 0; i < neuron_count; i++) {
        brain.neurons[i].activation = 0.0;
        brain.neurons[i].bias = (rand() % 200 - 100) / 100.0;
    }
    
    for (int i = 0; i < neuron_count; i++) {
        int num_connections = rand() % (neuron_count / 2 + 1);
        for (int j = 0; j < num_connections; j++) {
            int to = rand() % neuron_count;
            double weight = (rand() % 200 - 100) / 100.0;
            brain.neurons[i].out.push_back({to, weight});
        }
    }
    
    return brain;
}

// ###################################################################################
//                         <===== MAIN FUNCTION =====>
// ###################################################################################

int main() {
    sqlite3* db;                    // Opening of the database.
    sqlite3_open("brain.db", &db);

    Brain brain;                    // Creation of a namespace for our brain, which will contain all the neurons and their connections. This is what we will be saving and loading from the database.
    brain.neurons = load_brain(db);

    if (brain.neurons.empty()) {  // If the database is empty, we need to create a new brain. In the future, the function for creation a blank random brain will be present, so you will be able to reset existing brain to get a new one with the same amount of inputs and outputs.
        std::cout << "No brain found. Creating a new one." << std::endl;
        brain = create_random_brain(3, 3, 7, 21);
    }

    // THE MAIN LOOP, EVERY THOUGHT HAPPENS HERE.

//    while (true) {
//
//    }
    save_brain(db, brain.neurons);

    auto loaded = load_brain(db);

    std::cout << "Loaded neurons: " << loaded.size() << std::endl;

    sqlite3_close(db);
};
