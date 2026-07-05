#ifndef MST_HPP
#define MST_HPP

#include "grafo.hpp"
#include <vector>
#include <tuple>

class MST
{
    Grafo *grafo;
    std::vector<int> pai;
    std::vector<int> rank;
    std::vector<bool> permitidos;
    std::vector<std::tuple<int, int, double>> arestasMST;



public:
    MST(Grafo *grafo);
    MST(Grafo *grafo, std::vector<bool> verticesPermitidos);

    void construirMST();
    int find(int v);
    void unir(int a, int b);

    double getCustoTotal();
    std::vector<std::tuple<int, int, double>> getArestasMST();
};

#endif