-- database: ./brain.db
CREATE TABLE neurons (
    id INTEGER PRIMARY KEY,
    activation REAL,
    bias REAL,
    x_pos REAL,
    y_pos REAL
);

CREATE TABLE connections (
    from_id INTEGER,
    to_id INTEGER,
    weight REAL
);
