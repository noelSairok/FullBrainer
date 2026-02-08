-- database: ./brain.db
CREATE TABLE neurons (
    id INTEGER PRIMARY KEY,
    activation REAL,
    bias REAL
);

CREATE TABLE connections (
    from_id INTEGER,
    to_id INTEGER,
    weight REAL
);
