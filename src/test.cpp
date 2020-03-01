#include <iostream>

#include "slice2d.h"

using namespace std;

int main()
{
    vec_byte va{1, 2, 3, 4, 5, 6}, vb(9);
    Slice2D sa(va, 3), sb(vb, 3);
    Slice2D sa2(va, 3, 0, 2, 1, 0, 2, 1), sb2(vb, 3, 1, 3, 1, 0, 3, 2);
    sa2.swap(sb2);
    sa.print();
    cout << endl;
    sb.print();
    return 0;
}
