#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <SFML/Graphics.hpp>
#include <vector>

struct Neuron;  // forward declaration
struct Brain;   // forward declaration

class Visualizer {
public:
    Visualizer(int width = 800, int height = 800);
    ~Visualizer();
    
    void draw(const Brain& brain, int inputs, int outputs);
    bool isOpen();
    void close();
    
private:
    sf::RenderWindow window;
    int width;
    int height;
    
    sf::Vector2f getNeuronPosition(int neuronIndex, int totalNeurons, int inputs, int outputs);
    float getNeuronRadius(double leak);
    sf::Color getNeuronColor(int neuronIndex, int totalNeurons, int inputs, int outputs, double activation, double leak);
    
    void drawConnections(const Brain& brain, int inputs, int outputs);
    void drawNeurons(const Brain& brain, int inputs, int outputs);
};

#endif // VISUALIZER_H
