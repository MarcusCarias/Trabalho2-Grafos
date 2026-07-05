#include "mst.hpp"

#include <vector>
#include <tuple>
#include <algorithm>

static bool sortTerceiro(const std::tuple<int, int, double> &a,
                         const std::tuple<int, int, double> &b)
{
    return (std::get<2>(a) < std::get<2>(b));
}

MST::MST(Grafo *g) {

    grafo = g;

    construirMST();
}


MST::MST(Grafo *g, std::vector<bool> verticesPermitidos) {

    grafo = g;
    permitidos = verticesPermitidos;

    construirMST();
}

void MST::construirMST() {

    std::vector<std::tuple<int, int, double>> arestasGrafo = grafo->getArestas();

    int numVertices = grafo->getNumVertices();

    pai.resize(numVertices + 1);
    rank.assign(numVertices + 1, 0);

    for(int i = 1; i <= numVertices; i++) {
        pai[i] = i;
    }

    std::sort(arestasGrafo.begin(), arestasGrafo.end(), sortTerceiro); // ordeno as arestas pelo custo crescente

    arestasMST.clear();

    for (const auto& aresta : arestasGrafo) {
        int u = std::get<0>(aresta);
        int v = std::get<1>(aresta);

        if (!permitidos.empty() && (!permitidos[u] || !permitidos[v])) {
            continue;
        }

        if (find(u) != find(v)) {
            unir(u, v);
            arestasMST.push_back(aresta);
        }
    }
}

int MST::find(int v){

    if (pai[v] != v) {
        pai[v] = find(pai[v]);
    }
    return pai[v];

}

void MST::unir(int a, int b){

    int raizA = find(a);
    int raizB = find(b);

    if (raizA != raizB) {
        if (rank[raizA] < rank[raizB]) {
            pai[raizA] = raizB;
        } else if (rank[raizA] > rank[raizB]) {
            pai[raizB] = raizA;
        } else {
            pai[raizB] = raizA;
            rank[raizA]++;
        }
    }

}


double MST::getCustoTotal(){

    double custoTotal = 0.0;

    for (const auto& aresta : arestasMST) {
        custoTotal += std::get<2>(aresta);
    }

    return custoTotal;


}

std::vector<std::tuple<int, int, double>> MST::getArestasMST(){
    
    return arestasMST;
}