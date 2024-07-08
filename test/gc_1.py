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

def h(n):
    dicts = {}
    i = 0
    while i < n:
        dicts[i] = {}
        i += 1

    print("dicts: \n\n")
    print(dicts)

    i = 0
    j = 0
    while i < n:
        while j < n:
            print("dicts: \n\n")
            print(dicts)

            dicts[i][j+10] = dicts[j]
            j += 1
        i += 1
        j = 0


# f()
g(4)
# h(2)