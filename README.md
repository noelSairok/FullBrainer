# FullBrainer - Neural Network Visualizer

A C++ neural network simulator with SFML-based real-time visualization.

## Features

- **Neural Network Simulation**: Evolving neural networks with mutation and neuron creation/deletion
- **Real-time Visualization** (800x800 window):
  - **Input neurons** (left column): Orange circles
  - **Output neurons** (right column): Green circles
  - **Hidden neurons** (center): Desaturated blue, brightness varies with activation
  - **Connections**: Lines with thickness proportional to connection weight
  - Neuron size varies with leak value
- **Persistent Storage**: Brain state saved to SQLite database
- **Evolutionary Algorithm**: Networks mutate, duplicate, and prune over time

## Building

### Prerequisites
- `g++` (C++17 compatible)
- SFML 3.0+ development libraries
- SQLite3 development libraries

### Manual Build
```bash
g++ -std=c++17 -o fullBrainer fullBrainer.cpp visualizer.cpp \
    $(pkg-config --cflags --libs sfml-all) \
    $(pkg-config --cflags --libs sqlite3)
```

## Running
```bash
./fullBrainer
```
Press **Q** to save and exit.

## Visualization
- **Orange circles** (left): Input neurons
- **Green circles** (right): Output neurons  
- **Blue circles** (center): Hidden neurons (brightness = activation level)
- **Line thickness**: Proportional to connection weight
- **Circle size**: Increases with leak value

