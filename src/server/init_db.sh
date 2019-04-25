#!/usr/bin/env bash

DB_NAME="db.sqlite3"

if [ -f $DB_NAME ]; then
  rm $DB_NAME
fi
sqlite3 db.sqlite3 <init_db.sql
