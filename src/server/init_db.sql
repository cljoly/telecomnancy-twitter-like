CREATE TABLE "user" (
    "name"  TEXT UNIQUE,
    "password"  TEXT,
    "cookie"  INT UNIQUE,
    PRIMARY KEY("name")
);

