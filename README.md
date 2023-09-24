# rinha-de-compiler

[![Tests And Image](https://github.com/giancarlopro/rinha-de-compiler/actions/workflows/run_rinha_samples.yml/badge.svg)](https://github.com/giancarlopro/rinha-de-compiler/actions/workflows/run_rinha_samples.yml)

Esse projeto é um interpretador criado para ser submetido a [rinha de compilers](https://github.com/aripiprazole/rinha-de-compiler).

## Como executar

Basta compilar o projeto e executar o binário gerado passando o caminho para o arquivo que deseja executar.

```bash
make
./build/interpreter <path-to-file>
```

Ou usando o docker

```bash
docker build . -f Dockerfile -t rinha-c-interpreter

docker run \
    --rm \
    -v <path-to-file>:/var/rinha/source.rinha.json \
    rinha-c-interpreter
```

## CheckList de Funcionalidades

- [x] Let
- [x] Var
- [x] Call
- [x] Function
- [x] If
- [x] Binary
- [x] Int
- [x] Str
- [x] Print
- [x] First
- [x] Second
- [x] Bool
- [x] Tuple


## Tail Recursion Optmization

This branch contains a working TCO implementation, but since the interpreter doesn't care to free memory, it doesn't make any difference. So i will leave it here just because ive lost a lot of time implementing it.
