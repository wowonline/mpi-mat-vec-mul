#include <iostream>
#include <fstream>
#include <random>

int main(int argc, char *argv[])
{
    std::fstream file{"a", std::fstream::out | std::fstream::binary};
    int n = std::stoi(argv[1]);
    file.write(reinterpret_cast<char*>(&n), sizeof(n));

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(-2147483648, 2147483647);

    int num;
    for (int i = 0; i < n * n; ++i) {
        num = dist6(rng);
        file.write(reinterpret_cast<char*>(&num), sizeof(num));
    }
    file.close();

    return 0;
}