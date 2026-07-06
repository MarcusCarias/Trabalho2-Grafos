# Trabalho 2 - DCC059 Teoria dos Grafos

Tema A - Prize-Collecting Steiner Tree Problem (PCSTP)

## Estrutura

Na raiz do repositório ficam os fontes (`main.cpp`, `grafo.cpp/hpp`, `guloso.cpp/hpp`,
`mst.cpp/hpp`, `poda.cpp/hpp`) e as pastas de instâncias `PCSPG-CRR/` e `PCSPG-JMP/`,
no formato `.stp` padrão da literatura.

## Como compilar (Linux)

```
make
```

Ou, sem Makefile:

```
g++ -std=c++17 -O2 main.cpp grafo.cpp guloso.cpp mst.cpp poda.cpp -o pcstp
```

Para limpar os arquivos compilados:

```
make clean
```

## Como executar

```
./pcstp                          # modo lote: todas as instancias, seed do relogio
./pcstp -s 12345                 # modo lote com seed fixa
./pcstp -i PCSPG-CRR/01.stp      # instancia unica, seed do relogio
./pcstp -i PCSPG-CRR/01.stp -s 12345   # instancia unica com seed fixa
./pcstp -h                       # exibe ajuda
```

O programa deve ser executado a partir da raiz do repositório (onde estão as pastas
de instâncias). Uma única execução roda os três algoritmos em sequência sobre a
instância configurada — ou sobre **todas** as instâncias de `PCSPG-CRR/` e
`PCSPG-JMP/`, se nenhuma for informada com `-i`:

1. **guloso** — construtivo com alfa = 0
2. **guloso randomizado** — parâmetros: alfa e número de iterações
3. **guloso randomizado reativo** — parâmetros: lista de alfas, número de iterações e tamanho do bloco

## Como configurar a instância e os parâmetros

Instância e seed podem ser informadas pela linha de comando:

| Opção | Descrição |
|---|---|
| `-i <arquivo>` | caminho do `.stp` (vazio = modo lote) |
| `-s <numero>`  | seed de randomização (vazio = horário atual) |
| `-h`           | exibe ajuda |

Os demais parâmetros dos algoritmos ficam no bloco "configuração" no início da
função `main` (`main.cpp`), e para alterar basta editar e recompilar:

| Variável | Descrição |
|---|---|
| `alfaRandomizado` | alfa do guloso randomizado |
| `iteracoesRandomizado` | iterações do guloso randomizado|
| `alfasReativo` | lista de alfas do reativo |
| `iteracoesReativo` | iterações do reativo |
| `tamanhoBloco` | tamanho do bloco do reativo |

### Reprodutibilidade

A seed principal é gerada (e impressa) uma única vez por execução. Cada instância do
lote reinicia o gerador com `seed principal + posição da instância`, e é essa seed da
instância que aparece na tela e no csv. Para reproduzir o resultado de uma instância
isolada, copie a seed da linha desejada no `resultados.csv` e execute:

```
./pcstp -i <instancia.stp> -s <seed>
```

## Saídas

**Na tela:** a semente utilizada e, para cada instância, o melhor custo, a média dos
custos das iterações, o alfa da melhor solução (no caso do reativo) e o tempo em segundos
de cada algoritmo. No final de cada instância é impressa a árvore da melhor solução
encontrada, uma aresta `u v custo` por linha (se a solução for um único vértice, só ele)
— o formato pode ser copiado e colado direto em
<https://csacademy.com/app/graph_editor/> para visualizar a árvore.

**`resultados.csv`:** a cada execução são acrescentadas três linhas (uma por algoritmo)
com data/hora, instância, algoritmo, parâmetros, semente, tempo e custos. O arquivo não
é apagado entre execuções, então ele acumula o histórico dos testes.
