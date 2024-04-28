def perfect_number(n):
    i = 1
    t = 0
    while i < n // 2 + 1:
        if n % i == 0:
            t += i
        i += 1

    if t == n:
        return True
    return False


res = 8000
while res < 10000:
    res += 1
    if perfect_number(res):
        break
