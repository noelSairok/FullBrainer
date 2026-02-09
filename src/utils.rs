// src/utils.rs

use rand::Rng;

pub fn random_sensor() -> f64 {
    let mut rng = rand::thread_rng();
    rng.gen_range(0.0..2.0) // Generates a random value between 0 and 2
}

pub fn generate_random_position() -> (f32, f32) {
    let mut rng = rand::thread_rng();
    (rng.gen_range(0.0..500.0), rng.gen_range(0.0..500.0)) // Generates random x, y positions
}