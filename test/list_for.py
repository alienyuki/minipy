def f():
    l = []
    i = 0
    while i < 5:
        i += 1
        l.append(i)
    for i in l:
        print(i)

def g():
    l = []
    i = 0
    it = iter(l)
    while i < 5:
        i += 1
        l.append(i * 10000 + 4)

    b = next(it, 12306)
    while b != 12306:
        print('py_iter ', b)
        b = next(it, 12306)

    print(next(it, 'over'))

f()
g()
