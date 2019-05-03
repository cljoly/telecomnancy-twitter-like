-- Table des utilisateurs
CREATE TABLE "user" (
    "name"  TEXT UNIQUE,
    "password"  TEXT,
    "cookie"  INT UNIQUE,
    PRIMARY KEY("name")
);

-- Tables des messages
CREATE TABLE "gazou" (
    id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "date"  TEXT,
    "author"  TEXT,
    "content"  TEXT,
    FOREIGN KEY(author) REFERENCES user(name),
    UNIQUE ("date","author", "content")
    ON CONFLICT ABORT
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
    ON CONFLICT ABORT
);

-- Qui est abooné à qui
CREATE TABLE "user_subscription" (
    "followed" TEXT,
    "follower" TEXT,
    FOREIGN KEY("followed") REFERENCES user(name),
    FOREIGN KEY("follower") REFERENCES user(name),
    UNIQUE ("follower","followed")
    ON CONFLICT ABORT
);

-- Qui est abonné à quel thématique (la thématique n’est pas obligé de
    -- préexister, donc pas de foreign key)
CREATE TABLE "tag_subscription" (
    "tag" TEXT,
    "follower" TEXT,
    FOREIGN KEY("follower") REFERENCES user(name),
    UNIQUE ("follower","tag")
    ON CONFLICT ABORT
);

-- Table des retweet, comme une table d’association un peu
CREATE TABLE "relay" (
    "gazou_id" INT,
    "retweeter"  TEXT,
    "retweet_date"  TEXT NOT NULL,
    FOREIGN KEY("gazou_id") REFERENCES gazou("id"),
    FOREIGN KEY("retweeter") REFERENCES user("name"),
    UNIQUE ("gazou_id","retweeter", "retweet_date")
    ON CONFLICT ABORT
);

-- Index pour accélérer les requêtes de récupération des gazou suivis
CREATE INDEX user_follower ON user_subscription(follower);
CREATE INDEX tag_follower ON tag_subscription(follower);
