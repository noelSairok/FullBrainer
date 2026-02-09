#include "visualizer.h"
#include <iostream>
#include <cmath>

// Forward declare Neuron and Brain since we only need them for drawing
struct Connection;
struct Neuron {
    double activation;
    double bias;
    double leak;
    double avg;
    float x, y;
    std::vector<Connection> out;
};

struct Connection {
    int to;
    double weight;
};

struct Brain {
    std::vector<Neuron> neurons;
};

Visualizer::Visualizer(int width, int height)
    : window(sf::VideoMode(sf::Vector2u(width, height)), "Neural Network Visualizer"),
      width(width), height(height) {
    window.setFramerateLimit(60);
}

Visualizer::~Visualizer() {
    if (window.isOpen()) {
        window.close();
    }
}

bool Visualizer::isOpen() {
    return window.isOpen();
}

void Visualizer::close() {
    window.close();
}

sf::Vector2f Visualizer::getNeuronPosition(int neuronIndex, int totalNeurons, int inputs, int outputs) {
    int hiddenCount = totalNeurons - inputs - outputs;
    
    if (neuronIndex < inputs) {
        // Input neurons on the left
        float x = 30.0f;
        float y = 50.0f + neuronIndex * 34.0f;  // radius ~15, gap 4 = 34 total
        return sf::Vector2f(x, y);
    } else if (neuronIndex < inputs + outputs) {
        // Output neurons on the right
        int outputIndex = neuronIndex - inputs;
        float x = width - 30.0f;
        float y = 50.0f + outputIndex * 34.0f;
        return sf::Vector2f(x, y);
    } else {
        // Hidden neurons: use coordinates from neuron struct, but we'll use random for now
        // In a real implementation, use neuron.x and neuron.y
        float x = 50.0f + (neuronIndex % 10) * 70.0f;
        float y = 100.0f + ((neuronIndex - inputs - outputs) / 10) * 70.0f;
        return sf::Vector2f(x, y);
    }
}

float Visualizer::getNeuronRadius(double leak) {
    // Base radius of 12, plus leak influence (clamped to reasonable range)
    float leakInfluence = std::min(8.0, leak * 20.0);
    return 12.0f + static_cast<float>(leakInfluence);
}

sf::Color Visualizer::getNeuronColor(int neuronIndex, int totalNeurons, int inputs, int outputs, 
                                      double activation, double leak) {
    if (neuronIndex < inputs) {
        // Input neurons: orange
        return sf::Color(255, 165, 0);
    } else if (neuronIndex < inputs + outputs) {
        // Output neurons: green
        return sf::Color(50, 200, 50);
    } else {
        // Hidden neurons: desaturated blue with brightness based on activation
        // Activation ranges from 0 to 1 (sigmoid output)
        int brightness = static_cast<int>(100 + activation * 155);  // 100 to 255
        brightness = std::min(255, std::max(0, brightness));
        return sf::Color(brightness / 2, brightness / 2, brightness);  // desaturated blue
    }
}

void Visualizer::drawConnections(const Brain& brain, int inputs, int outputs) {
    int totalNeurons = brain.neurons.size();
    
    for (int i = 0; i < totalNeurons; i++) {
        sf::Vector2f fromPos = getNeuronPosition(i, totalNeurons, inputs, outputs);
        
        for (const auto& conn : brain.neurons[i].out) {
            sf::Vector2f toPos = getNeuronPosition(conn.to, totalNeurons, inputs, outputs);
            
            // Line thickness based on weight (normalized)
            // Weight ranges roughly from -1 to 1, thickness from 0.5 to 3
            float thickness = 0.5f + static_cast<float>(std::abs(conn.weight)) * 1.25f;
            thickness = std::min(3.0f, thickness);
            
            // Color based on weight sign
            sf::Color lineColor = (conn.weight > 0) ? sf::Color(0, 150, 255) : sf::Color(255, 100, 0);
            
            // Draw thick line using RectangleShape
            sf::Vector2f direction = toPos - fromPos;
            float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            
            sf::RectangleShape line(sf::Vector2f(distance, thickness));
            line.setPosition(fromPos);
            line.setFillColor(lineColor);
            
            // Calculate rotation angle
            float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159f;
            line.setRotation(sf::degrees(angle));
            
            window.draw(line);
        }
    }
}

void Visualizer::drawNeurons(const Brain& brain, int inputs, int outputs) {
    int totalNeurons = brain.neurons.size();
    
    for (int i = 0; i < totalNeurons; i++) {
        sf::Vector2f pos = getNeuronPosition(i, totalNeurons, inputs, outputs);
        float radius = getNeuronRadius(brain.neurons[i].leak);
        
        sf::CircleShape neuronCircle(radius);
        neuronCircle.setPosition(sf::Vector2f(pos.x - radius, pos.y - radius));
        neuronCircle.setFillColor(getNeuronColor(i, totalNeurons, inputs, outputs, 
                                                  brain.neurons[i].activation, 
                                                  brain.neurons[i].leak));
        
        window.draw(neuronCircle);
    }
}

void Visualizer::draw(const Brain& brain, int inputs, int outputs) {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
    }
    
    window.clear(sf::Color(30, 30, 30));  // Dark background
    
    drawConnections(brain, inputs, outputs);
    drawNeurons(brain, inputs, outputs);
    
    window.display();
}
