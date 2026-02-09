
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
#include <cmath>
#include <thread>
#include <chrono>
#include <sqlite3.h>
#include "visualizer.h"

// ░▒▓███████▓▒░░▒▓███████▓▒░ ░▒▓██████▓▒░░▒▓█▓▒░▒▓███████▓▒░         
// ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓██▓▒░ 
// ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓██▓▒░ 
// ░▒▓███████▓▒░░▒▓███████▓▒░░▒▓████████▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░        
// ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓██▓▒░ 
// ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓██▓▒░ 
// ░▒▓███████▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░        

// ---- BRAIN STRUCTURES ----

struct Connection {
    int to;
    double weight;
};

struct Neuron {
    double activation = 0.0;
    double bias = 0.0;
    double leak = 0.0;
    double avg = 0.0;
    float x, y; // for visualization only
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

        // load leak and avg (columns 3 and 4 in the schema). default to 0.0 if NULL.
        if (sqlite3_column_type(stmt, 3) == SQLITE_NULL)
        {
            n.x = static_cast<float>(rand() % 501);
        }
        else
        {
            n.leak = sqlite3_column_double(stmt, 3);
        }

        if (sqlite3_column_type(stmt, 4) == SQLITE_NULL)
        {
            n.avg = 0.0;
        }
        else
        {
            n.avg = sqlite3_column_double(stmt, 4);
        }

        // Initialize x,y from DB if present (columns 5 and 6), otherwise set randomly 0-500
        if (sqlite3_column_type(stmt, 5) == SQLITE_NULL)
        {
            n.x = static_cast<float>(rand() % 501);
        }
        else
        {
            n.x = static_cast<float>(sqlite3_column_double(stmt, 5));
        }

        if (sqlite3_column_type(stmt, 6) == SQLITE_NULL)
        {
            n.y = static_cast<float>(rand() % 501);
        }
        else
        {
            n.y = static_cast<float>(sqlite3_column_double(stmt, 6));
        }

        loaded_brain.push_back(n);
    }
    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db, "SELECT * FROM connections;", -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int from = sqlite3_column_int(stmt, 0);
        int to   = sqlite3_column_int(stmt, 1);
        double w = sqlite3_column_double(stmt, 2);
        loaded_brain[static_cast<size_t>(from)].out.push_back({to, w});
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
            std::to_string(brain[i].bias) + "," +
            std::to_string(brain[i].leak) + "," +
            std::to_string(brain[i].avg) + "," +
            std::to_string(brain[i].x) + "," +
            std::to_string(brain[i].y) + ");";
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

// ---- KILL NEURON FUNCTION ----

void kill_neuron(Brain& brain, int index) {
    brain.neurons.erase(brain.neurons.begin() + index);

    // fix connections
    for (auto &n : brain.neurons)
    {
        for (auto &c : n.out)
        {
            if (c.to > index)
                c.to--;
        }
    }
}

// ---- NEURON DUPLICATION FUNCTION ----

void duplicate_neuron(Brain& brain, int index) {
    Neuron copy = brain.neurons[index];

    // mutate slightly
    copy.bias += ((rand() % 100) / 5000.0 - 0.01);

    brain.neurons.push_back(copy);
}

// ---- EVOLUTION FUNCTION ----

void mutate(Brain& brain)
{
    for (auto &n : brain.neurons)
    {
        if (rand() % 100 < 5)
        {
            n.bias += ((rand() % 100) / 1000.0 - 0.05);
        }

        for (auto &c : n.out)
        {
            if (rand() % 100 < 5)
            {
                c.weight += ((rand() % 100) / 1000.0 - 0.05);
            }
        }
    }

    if (rand() % 100 < 2)
    {
        duplicate_neuron(brain, rand() % brain.neurons.size());
    }

    if (brain.neurons.size() > 5 && rand() % 100 < 2)
    {
        kill_neuron(brain, rand() % brain.neurons.size());
    }
}

// ---- INPUT SETTING FUNCTION ----

void set_inputs(Brain &brain, std::vector<double> inputs) {
    for (int i = 0; i < inputs.size(); i++)
    {
        brain.neurons[i].activation = inputs[i];
    }
}

// ---- OUTPUT READING FUNCTION ----

std::vector<double> read_outputs(Brain &brain, int outputs) {
    std::vector<double> out;
    int n = brain.neurons.size();

    for (int i = n - outputs; i < n; i++)
    {
        out.push_back(brain.neurons[i].activation);
    }

    return out;
}

// ---- BRAIN THINKING FUNCTION ----

void step_brain(Brain& brain) {
    int neuron_count = brain.neurons.size();
    std::vector<double> new_activations(neuron_count);
    
    // Update all neurons
    for (int i = 0; i < neuron_count; i++) {
        double sum = brain.neurons[i].bias;
        
        // Sum inputs from all neurons
        for (int j = 0; j < neuron_count; j++) {
            for (auto& conn : brain.neurons[j].out) {
                if (conn.to == i) {
                    sum += brain.neurons[j].activation * conn.weight;
                }
            }
        }
        
        // Sigmoid activation function
        new_activations[i] = 1.0 / (1.0 + exp(-sum));
    }
    
    // Update all neuron activations
    for (int i = 0; i < neuron_count; i++) {
        brain.neurons[i].activation = new_activations[i];
    }
}

// ---- RANDOM SENSOR FUNCTION (NOT FOR REAL WORK BUT FOR DEBUGGING ONLY) ----

double random_sensor() {
    return (rand() % 100) / 50.0;
}

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

// ---- ASCII WISUALIZATION FUNCTION (JUST FOR FUN) ----

void ascii_visualize(Brain &brain)
{
    for (int i = 0; i < brain.neurons.size(); i++)
    {
        int bars = (int)(brain.neurons[i].activation * 20);
        std::cout << i << " ";
        for (int j = 0; j < bars; j++)
            std::cout << "#";
        std::cout << "\n";
    }
}

// ###################################################################################
//                         <===== MAIN FUNCTION =====>
// ###################################################################################

int main() {
    sqlite3* db;                    // Opening of the database.
    sqlite3_open("brain.db", &db);

    Brain brain;                    // Creation of a namespace for our brain, which will contain all the neurons and their connections. This is what we will be saving and loading from the database.
    brain.neurons = load_brain(db);

    if (brain.neurons.empty()) {  // If the database is empty, we need to create a new brain.
        std::cout << "No brain found. Creating a new one." << std::endl;
        brain = create_random_brain(5, 5, 20, 40);
    }

    // Create visualizer
    Visualizer visualizer(800, 800);

    // Configuration: 3 inputs, 3 outputs
    const int num_inputs = 3;
    const int num_outputs = 3;

    std::thread input_thread([&brain, &db]() {
        while (true) {
            char input;
            std::cin >> input;
            if (input == 'Q' || input == 'q') {
                save_brain(db, brain.neurons);
                std::cout << "Brain saved. Exiting..." << std::endl;
                exit(0);
            }
        }
    });
    input_thread.detach();


    // THE MAIN LOOP, EVERY THOUGHT HAPPENS HERE.

    while (visualizer.isOpen()) {
        std::vector<double> inputs = {
            random_sensor(),
            random_sensor(),
            random_sensor()
        };

        set_inputs(brain, inputs);

        // std::cout << "\n" << "Inputs: ";
        // for (double i : inputs) std::cout << i << " ";
        // std::cout << std::endl;

        step_brain(brain);

        auto outputs = read_outputs(brain, num_outputs);

        // std::cout << "\n" << "Outputs: ";
        // for (double o : outputs) std::cout << o << " ";
        // std::cout << std::endl;

        // Draw the network
        visualizer.draw(brain, num_inputs, num_outputs);

        mutate(brain);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

    }
    save_brain(db, brain.neurons);

    auto loaded = load_brain(db);

    std::cout << "Loaded neurons: " << loaded.size() << std::endl;

    sqlite3_close(db);
};
