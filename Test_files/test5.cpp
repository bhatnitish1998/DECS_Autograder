#include <iostream>
#include <unistd.h>
using namespace std;
int main(int argc, char const *argv[])
{
    system("ls");
    for (int i = 1; i <= 10; i++)
        cout << i << " ";
    return 0;
}
