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


#include <iostream>
#include <cmath>
#include <vector>
#include <thread>
#include <chrono>
#include <random>

std::vector<Neuron> load_brain(const std::string& file) {
    std::ifstream in(file, std::ios::binary);

    int n;
    in.read((char*)&n, sizeof(int));

    std::vector<Neuron> brain(n);

    for (int i = 0; i < n; i++) {
        in.read((char*)&brain[i].activation, sizeof(double));
        in.read((char*)&brain[i].bias, sizeof(double));

        int m;
        in.read((char*)&m, sizeof(int));

        brain[i].out.resize(m);
        for (int j = 0; j < m; j++) {
            in.read((char*)&brain[i].out[j].to, sizeof(int));
            in.read((char*)&brain[i].out[j].weight, sizeof(double));
        }
    }

    return brain;
}

struct Connection {
    int to;
    double weight;
};

struct Neuron {
    double activation;
    double bias;
    std::vector<Connection> out;
};

void save_brain(const std::vector<Neuron>& brain, const std::string& file) {
    std::ofstream out(file, std::ios::binary);

    int n = brain.size();
    out.write((char*)&n, sizeof(int));

    for (const auto& neuron : brain) {
        out.write((char*)&neuron.activation, sizeof(double));
        out.write((char*)&neuron.bias, sizeof(double));

        int m = neuron.out.size();
        out.write((char*)&m, sizeof(int));

        for (const auto& conn : neuron.out) {
            out.write((char*)&conn.to, sizeof(int));
            out.write((char*)&conn.weight, sizeof(double));
        }
    }
}


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

