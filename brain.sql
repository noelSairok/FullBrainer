-- database: ./brain.db
CREATE TABLE neurons (
    id INTEGER PRIMARY KEY,
    activation REAL,
    bias REAL,
    leak REAL,
    avg REAL,
    posx FLOAT,
    posy FLOAT
);

CREATE TABLE connections (
    from_id INTEGER,
    to_id INTEGER,
    weight REAL
);
