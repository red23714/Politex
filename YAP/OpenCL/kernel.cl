__kernel void check_prime(__global const int* numbers, __global int* is_prime, const int N) {
    int idx = get_global_id(0);
    if (idx >= N) return;

    int n = numbers[idx];
    if (n < 2) {
        is_prime[idx] = 0;
        return;
    }

    int prime = 1;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            prime = 0;
            break;
        }
    }

    is_prime[idx] = prime;
}