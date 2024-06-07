called = 0
def hanoi(a, b, c, layer):
    if layer == 0:
        return
    
    global called
    called += 1
    hanoi(a, c, b, layer-1)
    print("move from ", a, " to ", c)
    hanoi(b, a, c, layer-1)

hanoi('a', 'b', 'c', 3)
print('called times: ', called)
# print('called times: ', call2)
