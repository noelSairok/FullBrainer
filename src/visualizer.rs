use crate::brain::Brain;
use std::time::{Instant, Duration};

pub struct Visualizer {
    open: bool,
    last_print: Instant,
}

impl Visualizer {
    pub fn new(_width: u32, _height: u32) -> Self {
        Visualizer {
            open: true,
            last_print: Instant::now(),
        }
    }

    pub fn is_open(&self) -> bool {
        self.open
    }

    pub fn close(&mut self) {
        self.open = false;
    }

    pub fn draw(&mut self, brain: &Brain, _num_inputs: usize, num_outputs: usize) {
        if self.last_print.elapsed() < Duration::from_millis(250) {
            return;
        }
        self.last_print = Instant::now();

        let n = brain.neurons.len();
        let outs = if num_outputs == 0 || num_outputs > n {
            vec![]
        } else {
            brain.neurons[n - num_outputs..n].iter().map(|n| n.activation).collect::<Vec<_>>()
        };
        println!("brain: neurons={} outputs={:?}", n, outs);
    }
}