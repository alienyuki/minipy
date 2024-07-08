d = {"k0": 1}
a = d["k0"]
print(a)
d["k1"] = 2
d["k2"] = d
print(d)
p = d.pop("k1")
print(p)
