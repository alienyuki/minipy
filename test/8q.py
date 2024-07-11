N = 8
ans = 0
dfs_call = 0

def ok(board, i, j):
    # check row
    k = 0
    while k < N:
        if k == j:
            k += 1
            continue
        if board[i][k] == 1:
            return False
        k += 1

    # check col
    k = 0
    while k < N:
        if k == i:
            k += 1
            continue
        if board[k][j] == 1:
            return False
        k += 1

    # check x
    k = i
    l = j
    while k > 0 and l < N - 1:
        k -= 1
        l += 1
        if board[k][l] == 1:
            return False
    k = i
    l = j
    while k < N - 1 and l > 0:
        k += 1
        l -= 1
        if board[k][l] == 1:
            return False
    k = i
    l = j
    while k < N - 1 and l < N - 1:
        k += 1
        l += 1
        if board[k][l] == 1:
            return False
    k = i
    l = j
    while k > 0 and l > 0:
        k -= 1
        l -= 1
        if board[k][l] == 1:
            return False
    return True


def dfs(board, i):
    global dfs_call
    dfs_call += 1
    j = 0
    while j < N:
        board[i][j] = 1
        if ok(board, i, j):
            if i == N - 1:
                global ans
                ans += 1
                # pb(board)
                board[i][j] = 0
                return
            else:
                dfs(board, i+1)
        board[i][j] = 0
        j += 1


def pb(board):
    print("")
    i = 0
    while i < N:
        print(board[i])
        i += 1


def gb():
    a = []
    i = 0
    j = 0

    while i < N:
        line = []
        j = 0
        while j < N:
            line.append(0)
            j += 1
        a.append(line)
        i += 1
    return a

a = gb()
dfs(a, 0)
print(ans)
print(dfs_call)

