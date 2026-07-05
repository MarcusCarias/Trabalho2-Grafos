#ifndef GRAFO_HPP
#define GRAFO_HPP

#include <string>
#include <vector>
#include <utility>
#include <tuple>


class Grafo
{
    int numVertices = 0;
    std::vector<double> premio;
    std::vector<std::vector<std::pair<int, double>>> adjacencia;
    std::vector<std::tuple<int, int, double>> arestas;

public:
    Grafo();

    void carregaArquivo(std::string caminho);

    int getNumVertices();
    double getPremio(int vertice);
    std::vector<std::pair<int, double>> getVizinhos(int vertice);
    std::vector<std::tuple<int, int, double>> getArestas();

};

#endif