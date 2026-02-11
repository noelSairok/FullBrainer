// filepath: /src/brain.rs

use rusqlite::{params, Connection, Result};
use rand::Rng;
use std::vec::Vec;

#[derive(Debug, Clone)]
pub struct Conn {
    pub to: usize,
    pub weight: f64,
}

#[derive(Debug, Clone)]
pub struct Neuron {
    pub activation: f64,
    pub bias: f64,
    pub leak: f64,
    pub avg: f64,
    pub x: f32,
    pub y: f32,
    pub out: Vec<Conn>,
}

#[derive(Debug, Clone)]
pub struct Brain {
    pub neurons: Vec<Neuron>,
}

pub fn load_brain(db: &Connection) -> Result<Vec<Neuron>> {
    let mut loaded_brain = Vec::new();
    let mut stmt = db.prepare("SELECT id, activation, bias, leak, avg, posx, posy FROM neurons;")?;
    
    let neuron_iter = stmt.query_map([], |row| {
        Ok(Neuron {
            activation: row.get(1)?,
            bias: row.get(2)?,
            leak: row.get::<_, Option<f64>>(3)?.unwrap_or(0.0),
            avg: row.get::<_, Option<f64>>(4)?.unwrap_or(0.0),
            x: row.get::<_, Option<f32>>(5)?.unwrap_or(rand::thread_rng().gen_range(0.0..500.0)),
            y: row.get::<_, Option<f32>>(6)?.unwrap_or(rand::thread_rng().gen_range(0.0..500.0)),
            out: Vec::new(),
        })
    })?;

    for neuron in neuron_iter {
        loaded_brain.push(neuron?);
    }

    let mut stmt = db.prepare("SELECT from_id, to_id, weight FROM connections;")?;
    let connection_iter = stmt.query_map([], |row| {
        Ok((row.get::<_, i64>(0)?, row.get::<_, i64>(1)?, row.get::<_, f64>(2)?))
    })?;

    for result in connection_iter {
        let (from_id, to_id, weight) = result?;
        let from_idx = from_id as usize;
        let to_idx = to_id as usize;
        if from_idx < loaded_brain.len() {
            loaded_brain[from_idx].out.push(Conn { to: to_idx, weight });
        }
    }

    Ok(loaded_brain)
}

pub fn save_brain(db: &Connection, brain: &[Neuron]) -> Result<()> {
    db.execute("DELETE FROM neurons;", [])?;
    db.execute("DELETE FROM connections;", [])?;

    for (i, neuron) in brain.iter().enumerate() {
        db.execute(
            "INSERT INTO neurons (id, activation, bias, leak, avg, posx, posy) VALUES (?, ?, ?, ?, ?, ?, ?);",
            params![
                i as i64,
                neuron.activation,
                neuron.bias,
                neuron.leak,
                neuron.avg,
                neuron.x,
                neuron.y,
            ],
        )?;

        for connection in &neuron.out {
            db.execute(
                "INSERT INTO connections (from_id, to_id, weight) VALUES (?, ?, ?);",
                params![i as i64, connection.to as i64, connection.weight],
            )?;
        }
    }

    Ok(())
}

pub fn mutate(brain: &mut Brain) {
    let mut rng = rand::thread_rng();
    for neuron in &mut brain.neurons {
        if rng.gen_range(0..100) < 5 {
            neuron.bias += rng.gen_range(-0.01..0.01);
        }
        for connection in &mut neuron.out {
            if rng.gen_range(0..100) < 5 {
                connection.weight += rng.gen_range(-0.1..0.1);
            }
        }
    }

    if !brain.neurons.is_empty() && rng.gen_range(0..100) < 2 {
        duplicate_neuron(brain, rng.gen_range(0..brain.neurons.len()));
    }

    if brain.neurons.len() > 5 && rng.gen_range(0..100) < 2 {
        kill_neuron(brain, rng.gen_range(0..brain.neurons.len()));
    }
}

pub fn set_inputs(brain: &mut Brain, inputs: &[f64]) {
    for (i, &input) in inputs.iter().enumerate() {
        if i < brain.neurons.len() {
            brain.neurons[i].activation = input;
        }
    }
}

pub fn read_outputs(brain: &Brain, outputs: usize) -> Vec<f64> {
    let n = brain.neurons.len();
    if outputs == 0 || outputs > n {
        return Vec::new();
    }
    brain.neurons[n - outputs..n].iter().map(|n| n.activation).collect()
}

pub fn step_brain(brain: &mut Brain) {
    let neuron_count = brain.neurons.len();
    if neuron_count == 0 {
        return;
    }
    let mut new_activations = vec![0.0; neuron_count];

    for (i, neuron) in brain.neurons.iter().enumerate() {
        let sum: f64 = neuron.out.iter().map(|c| brain.neurons[c.to].activation * c.weight).sum();
        new_activations[i] = 1.0 / (1.0 + (-sum - neuron.bias).exp());
    }

    for (i, activation) in new_activations.into_iter().enumerate() {
        brain.neurons[i].activation = activation;
    }
}

pub fn create_random_brain(_inputs: usize, _outputs: usize, min_neurons: usize, max_neurons: usize) -> Brain {
    let mut brain = Brain { neurons: Vec::new() };
    let mut rng = rand::thread_rng();
    let neuron_count = min_neurons + rng.gen_range(0..=max_neurons.saturating_sub(min_neurons));

    for _ in 0..neuron_count {
        brain.neurons.push(Neuron {
            activation: 0.0,
            bias: rng.gen_range(-1.0..1.0),
            leak: 0.0,
            avg: 0.0,
            x: rng.gen_range(0.0..500.0),
            y: rng.gen_range(0.0..500.0),
            out: Vec::new(),
        });
    }

    for i in 0..neuron_count {
        let num_connections = rng.gen_range(0..=neuron_count / 2);
        for _ in 0..num_connections {
            let to = rng.gen_range(0..neuron_count);
            let weight = rng.gen_range(-1.0..1.0);
            brain.neurons[i].out.push(Conn { to, weight });
        }
    }

    brain
}

pub fn kill_neuron(brain: &mut Brain, index: usize) {
    if index >= brain.neurons.len() {
        return;
    }
    brain.neurons.remove(index);
    for neuron in &mut brain.neurons {
        neuron.out.retain(|c| c.to != index);
        for c in &mut neuron.out {
            if c.to > index {
                c.to -= 1;
            }
        }
    }
}

pub fn duplicate_neuron(brain: &mut Brain, index: usize) {
    if index >= brain.neurons.len() {
        return;
    }
    let mut copy = brain.neurons[index].clone();
    copy.bias += rand::thread_rng().gen_range(-0.01..0.01);
    brain.neurons.push(copy);
}