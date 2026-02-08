#include <iostream>
#include <cmath>

double w = 0.8;
double b = 0.2;
double a = 0.0;
double leak = 0.8;

double neuron(double x) {
    double y = std::tanh(w * x + b + a * leak);
    return y;
}

int main() {
    for (double x : {-2, -1, 0, 1, 2}) {
        std::cout << x << " => " << neuron(x) << std::endl;
    }
}
