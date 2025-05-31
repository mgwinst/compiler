int main() {
    const int a1 = 5;
    const int b = 10;
    int result = a1 + b * 2;
    const string s = "hello world";

    if (result > 10) {
        result += 1;
    } else {
        result -= 1;
    }

    while (result < 100) {
        result = result * 2;
    }

    return result;
}