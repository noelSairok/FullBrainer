# FullBrainer - Graph Neural Network

A Rust neural network simulator with real-time visualization.

## Features

- **Neural Network Simulation**: Evolving neural networks with mutation and neuron creation/deletion.
- **Real-time Visualization**:
  - **Input neurons** (left column): Orange circles.
  - **Output neurons** (right column): Green circles.
  - **Hidden neurons** (center): Desaturated blue, brightness varies with activation.
  - **Connections**: Lines with thickness proportional to connection weight.
  - Neuron size varies with leak value.
- **Persistent Storage**: Brain state saved to SQLite database.
- **Evolutionary Algorithm**: Networks mutate, duplicate, and prune over time.

## Building

### Prerequisites
- Rust (latest stable version)
- SQLite3 development libraries

### Build
To build the project, run:
```bash
cargo build --release
```

## Running
To run the application, use:
```bash
cargo run
```
Press **Q** to save and exit.

## Visualization
- **Orange circles** (left): Input neurons.
- **Green circles** (right): Output neurons.  
- **Blue circles** (center): Hidden neurons (brightness = activation level).
- **Line thickness**: Proportional to connection weight.
- **Circle size**: Increases with leak value.