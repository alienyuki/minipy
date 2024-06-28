a = []
b = [1]
c = [b]
b.append(1)
c.append(a)
d = c.pop()
c.pop()

i = 0
l = []
while i < 10:
    l.append(i)
    i += 1
    print(l)

print(l.pop(1))
print(l.pop(3))
print(l.pop(5))

print(l[1])
l[1] = "yuki"
print(l[1])

a.append(a)

r1 = [1]
r2 = [2]
r1.append(r2)
r2.append(r1)
print("r1: ", r1)
print("r2: ", r2)
