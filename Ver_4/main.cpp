#include "headers/Database.hpp"

int main()
{
    Database db;
    db.insertRequest(1, "Hello world");
    db.showAll();
    return 0;
}