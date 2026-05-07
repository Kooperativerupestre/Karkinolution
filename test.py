funcs = []

ids = [10, 20, 30]

for i in ids:
    def f(i=i):
        def g():
            return f'Processando entidade {i}'
    funcs.append(f)

# em algum momento futuro:
resultados = [f()) for f in funcs]


print(resultados)

