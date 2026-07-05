#include "poda.hpp"

#include <queue>

Poda::Poda(Grafo *g, std::vector<std::tuple<int, int, double>> arvore) {

    arestas = arvore;
    grafo = g;
    custoFinal = 0.0;
}

void Poda::podar(){

    double premio, custo;
    int v, u;

    arvoreFinal.clear();
    custoFinal = 0.0;

    int numVertices = grafo->getNumVertices();

    std::vector<std::vector<std::pair<int, double>>> adjPoda(numVertices + 1);
    std::vector<int> grau(numVertices + 1, 0);
    std::vector<bool> removido(numVertices + 1, false);

    for (const auto &aresta : arestas)
    {
        u = std::get<0>(aresta);
        v = std::get<1>(aresta);
        custo = std::get<2>(aresta);

        adjPoda[u].push_back(std::make_pair(v, custo));
        adjPoda[v].push_back(std::make_pair(u, custo));
    }

    for (int i = 1; i <= numVertices; i++) {
        grau[i] = adjPoda[i].size();
    }

    std::queue<int> fila;

    for (int i = 1; i <= numVertices; i++) {
        if (grau[i] == 1) {
            fila.push(i);
        }
    }

    while (!fila.empty())
    {
        v = fila.front();
        fila.pop();

        if (removido[v]) {
            continue;
        }

        u = -1;
        custo = 0.0;

        for (const auto &vizinho : adjPoda[v]) {
            if (!removido[vizinho.first]) {
                u = vizinho.first;
                custo = vizinho.second;
                break;
            }
        }

        premio = grafo->getPremio(v);

        if (custo > premio) {
            removido[v] = true;
            custoFinal += premio;

            grau[u]--;
            if (grau[u] == 1 && !removido[u]) {
                fila.push(u);
            }
        }
    }

    for (const auto &aresta : arestas)
    {
        u = std::get<0>(aresta);
        v = std::get<1>(aresta);
        custo = std::get<2>(aresta);

        if (!removido[u] && !removido[v]) {
            arvoreFinal.push_back(aresta);
            custoFinal += custo;
        }
    }

    mantido.assign(numVertices + 1, false);
    for (int i = 1; i <= numVertices; i++) {
        mantido[i] = !removido[i];
    }
}

void Poda::podarForte(){

    arvoreFinal.clear();
    custoFinal = 0.0;

    int numVertices = grafo->getNumVertices();

    double totalPremios = 0.0;
    for (int v = 1; v <= numVertices; v++) {
        totalPremios += grafo->getPremio(v);
    }

    mantido.assign(numVertices + 1, false);

    // arvore vazia: a melhor solucao possivel e ficar so com o vertice de maior premio
    if (arestas.empty()) {
        int melhorVertice = 1;
        for (int v = 2; v <= numVertices; v++) {
            if (grafo->getPremio(v) > grafo->getPremio(melhorVertice)) {
                melhorVertice = v;
            }
        }
        mantido[melhorVertice] = true;
        custoFinal = totalPremios - grafo->getPremio(melhorVertice);
        return;
    }

    adjArvore.assign(numVertices + 1, std::vector<std::pair<int, double>>());

    for (const auto &aresta : arestas)
    {
        int u = std::get<0>(aresta);
        int v = std::get<1>(aresta);
        double custo = std::get<2>(aresta);

        adjArvore[u].push_back(std::make_pair(v, custo));
        adjArvore[v].push_back(std::make_pair(u, custo));
    }

    ganho.assign(numVertices + 1, 0.0);
    pai.assign(numVertices + 1, 0);
    visitado.assign(numVertices + 1, false);

    
    int raiz = std::get<0>(arestas[0]);
    calculaGanho(raiz, 0);

    
    int melhorVertice = raiz;
    for (int v = 1; v <= numVertices; v++) {
        if (visitado[v] && ganho[v] > ganho[melhorVertice]) {
            melhorVertice = v;
        }
    }

    mantido[melhorVertice] = true;
    coletaMantidas(melhorVertice, pai[melhorVertice]);

   
    custoFinal = totalPremios - ganho[melhorVertice];
}

void Poda::calculaGanho(int u, int p){

    visitado[u] = true;
    pai[u] = p;
    ganho[u] = grafo->getPremio(u);

    for (const auto &vizinho : adjArvore[u]) {
        int v = vizinho.first;
        double custo = vizinho.second;

        if (v == p) {
            continue;
        }

        calculaGanho(v, u);

        if (ganho[v] - custo > 0) {
            ganho[u] += ganho[v] - custo;
        }
    }
}

void Poda::coletaMantidas(int u, int p){

    for (const auto &vizinho : adjArvore[u]) {
        int v = vizinho.first;
        double custo = vizinho.second;

        if (v == p) {
            continue;
        }

        if (ganho[v] - custo > 0) {
            arvoreFinal.push_back(std::make_tuple(u, v, custo));
            mantido[v] = true;
            coletaMantidas(v, u);
        }
    }
}

double Poda::getCustoFinal(){
    return custoFinal;
}

std::vector<std::tuple<int, int, double>> Poda::getArestasPodadas(){
    return arvoreFinal;
}

std::vector<bool> Poda::getVerticesMantidos(){
    return mantido;
}
