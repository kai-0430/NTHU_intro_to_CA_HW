#include <iostream>
using namespace std;

int main(void)
{
    int a = 2, b, i;

    for (i = 0; i < 10; i++) {
        if (i == 5) break;
    }
    cout << i << endl;
    
    return 0;
}