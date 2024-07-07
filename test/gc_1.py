def f():
    a = ['a']
    b = ['b']
    a.append(b)
    a.append(b)
    b.append(a)
    b.append(a)


def g(n):
    lists = []
    i = 0
    while i < n:
        lists.append([i+100])
        i += 1

    i = 0
    j = 0
    while i < n:
        while j < n:
            lists[i].append(lists[j])
            j += 1
        i += 1
        j = 0

    print(lists)

f()
g(4)
