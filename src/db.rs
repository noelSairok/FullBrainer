// src/db.rs

use rusqlite::{params, Connection, Result};
use crate::brain::{Neuron, Conn};

pub fn load_brain(db: &Connection) -> Result<Vec<Neuron>> {
    let mut neurons = Vec::new();
    let mut stmt = db.prepare(
        "SELECT id, activation, bias, leak, avg, posx, posy FROM neurons ORDER BY id;"
    )?;
    let rows = stmt.query_map([], |row| {
        Ok(Neuron {
            activation: row.get(1)?,
            bias: row.get(2)?,
            leak: row.get::<_, Option<f64>>(3)?.unwrap_or(0.0),
            avg: row.get::<_, Option<f64>>(4)?.unwrap_or(0.0),
            x: row.get::<_, Option<f32>>(5)?.unwrap_or(rand::random::<f32>() * 500.0),
            y: row.get::<_, Option<f32>>(6)?.unwrap_or(rand::random::<f32>() * 500.0),
            out: Vec::new(),
        })
    })?;

    for r in rows {
        neurons.push(r?);
    }

    // Load connections
    let mut stmt = db.prepare("SELECT from_id, to_id, weight FROM connections;")?;
    let conn_iter = stmt.query_map([], |row| {
        Ok((row.get::<_, i64>(0)?, row.get::<_, i64>(1)?, row.get::<_, f64>(2)?))
    })?;

    for conn in conn_iter {
        let (from_id, to_id, weight) = conn?;
        let from_idx = from_id as usize;
        let to_idx = to_id as usize;
        if from_idx < neurons.len() {
            neurons[from_idx].out.push(Conn { to: to_idx, weight });
        }
    }

    Ok(neurons)
}

pub fn save_brain(db: &Connection, brain: &[Neuron]) -> Result<()> {
    db.execute("DELETE FROM neurons;", [])?;
    db.execute("DELETE FROM connections;", [])?;

    for (i, neuron) in brain.iter().enumerate() {
        db.execute(
            "INSERT OR REPLACE INTO neurons (id, activation, bias, leak, avg, posx, posy) VALUES (?, ?, ?, ?, ?, ?, ?);",
            params![i as i64, neuron.activation, neuron.bias, neuron.leak, neuron.avg, neuron.x, neuron.y],
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