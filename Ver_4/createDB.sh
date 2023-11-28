#!/bin/bash

# Script to create a PostgreSQL database
# TODO: Add libpqxx installation
# Define PostgreSQL user and database details
DB_NAME="grad"
DB_USER="cs744"
DB_PASSWORD="cs744"

# Define table details
TABLE_NAME="requests"
TABLE_COLUMNS="req_id BIGINT PRIMARY KEY NOT NULL, program text NOT NULL, request_status VARCHAR(50), grading_status VARCHAR(50), output text"

# Install PostgreSQL (if not already installed)
sudo apt update
sudo apt install postgresql postgresql-contrib

# Switch to the postgres user
sudo -u postgres psql -c "CREATE USER $DB_USER WITH PASSWORD '$DB_PASSWORD';"

# Create a new database
sudo -u postgres createdb $DB_NAME

# Grant privileges to the user on the database
sudo -u postgres psql -c "ALTER ROLE $DB_USER SET client_encoding TO 'utf8';"
sudo -u postgres psql -c "ALTER ROLE $DB_USER SET default_transaction_isolation TO 'read committed';"
sudo -u postgres psql -c "ALTER ROLE $DB_USER SET timezone TO 'UTC';"
sudo -u postgres psql -c "GRANT ALL PRIVILEGES ON DATABASE $DB_NAME TO $DB_USER;"
sudo -u postgres psql -c " GRANT ALL PRIVILEGES ON $TABLE_NAME TO $DB_USER;"

# Connect to the database and create a table
sudo -u postgres psql -d $DB_NAME -c "CREATE TABLE $TABLE_NAME ($TABLE_COLUMNS);"
