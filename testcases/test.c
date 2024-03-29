#include "for_gcc_build.h" // for gcc build, tiny cc will ignore it

int factor(int n) {
    if (n < 2) {
        return 1;
    }
    return n * factor(n - 1);
}

int main() {
    int i;
    i = 0;
    while (i < 10) {
        i = i + 1;
        if (i == 3 || i == 5) {
            continue;
        }
        if (i == 8) {
            break;
        }
        print("%d! = %d\n", i, factor(i));
    }
    return 0;
}
