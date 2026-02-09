// src/main.rs

use std::thread;
use std::time::Duration;
use rusqlite::Connection;
use fullbrainer_rust::brain::{Brain, create_random_brain, set_inputs, read_outputs, step_brain, mutate};
use fullbrainer_rust::{Visualizer, load_brain, save_brain};

fn main() {
    let db = Connection::open("brain.db").expect("Failed to open database");

    let mut brain = load_brain(&db).map(|neurons| Brain { neurons }).unwrap_or_else(|_| {
        println!("No brain found. Creating a new one.");
        create_random_brain(5, 5, 20, 40)
    });

    let mut visualizer = Visualizer::new(800, 800);
    let num_inputs = 5;
    let num_outputs = 5;

    let input_thread = thread::spawn(move || {
        loop {
            thread::sleep(Duration::from_millis(100));
        }
    });

    while visualizer.is_open() {
        let inputs = vec![random_sensor(), random_sensor(), random_sensor(), random_sensor(), random_sensor()];
        set_inputs(&mut brain, &inputs);

        step_brain(&mut brain);
        let _outputs = read_outputs(&brain, num_outputs);

        visualizer.draw(&brain, num_inputs, num_outputs);
        mutate(&mut brain);

        thread::sleep(Duration::from_millis(50));
    }

    let _ = save_brain(&db, &brain.neurons);
    let _ = input_thread.join();
}

fn random_sensor() -> f64 {
    rand::random::<f64>() * 2.0 - 1.0
}