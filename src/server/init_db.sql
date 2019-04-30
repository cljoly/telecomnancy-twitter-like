-- Table des utilisateurs
CREATE TABLE "user" (
    "name"  TEXT UNIQUE,
    "password"  TEXT,
    "cookie"  INT UNIQUE,
    PRIMARY KEY("name")
);

-- Tables des messages
CREATE TABLE "gazou" (
    id INT PRIMARY KEY,
    "date"  TEXT,
    "author"  TEXT,
    "content"  TEXT,
    FOREIGN KEY(author) REFERENCES user(name),
    UNIQUE ("date","author", "content")
    ON CONFLICT IGNORE
);

-- Tables des thématiques
CREATE TABLE "tag" (
    "name"  TEXT UNIQUE,
    PRIMARY KEY("name")
);

-- Association thématiques & message
CREATE TABLE "gazou_tag" (
    "gazou_id" INT,
    "tag"  TEXT,
    FOREIGN KEY("gazou_id") REFERENCES gazou("id"),
    FOREIGN KEY("tag") REFERENCES tag("name"),
    UNIQUE ("gazou_id","tag")
    ON CONFLICT IGNORE
);

-- Qui est abooné à qui
CREATE TABLE "user_subscription" (
    "followed" TEXT,
    "follower" TEXT,
    FOREIGN KEY("followed") REFERENCES user(name),
    FOREIGN KEY("follower") REFERENCES user(name),
    UNIQUE ("follower","followed")
    ON CONFLICT IGNORE
);

-- Qui est abonné à quel thématique
CREATE TABLE "tag_subscription" (
    "tag" TEXT,
    "follower" TEXT,
    FOREIGN KEY("tag") REFERENCES tag(name),
    FOREIGN KEY("follower") REFERENCES user(name),
    UNIQUE ("follower","tag")
    ON CONFLICT IGNORE
);

-- TODO Table des retweet par exemple avec (gazou_id, relayeur)
