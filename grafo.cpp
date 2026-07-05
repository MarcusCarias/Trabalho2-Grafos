#include "grafo.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

Grafo::Grafo(){}

void Grafo::carregaArquivo(std::string caminho)
{
    std::ifstream arquivo(caminho);

    if (!arquivo.is_open())
    {
        throw std::runtime_error("nao foi possivel abrir a instancia: " + caminho);
    }

    std::string linha;
    while (std::getline(arquivo, linha))
    {
        std::istringstream ss(linha);
        std::string chave;
        ss >> chave;

        if (chave == "Nodes")
        {
            ss >> numVertices;
            premio.assign(numVertices + 1, 0.0);
            adjacencia.assign(numVertices + 1, std::vector<std::pair<int, double>>());
        }
        else if (chave == "E")
        {
            int ver1, ver2;
            double custo;
            ss >> ver1 >> ver2 >> custo;
            arestas.push_back({ver1, ver2, custo});
            adjacencia[ver1].push_back({ver2, custo});
            adjacencia[ver2].push_back({ver1, custo});
        }
        else if (chave == "TP")
        {
            int vertice;
            double prem;
            ss >> vertice >> prem;
            premio[vertice] = prem;
        }
       
    }
}

int Grafo::getNumVertices() { return numVertices; }

double Grafo::getPremio(int vertice) { return premio[vertice]; }

std::vector<std::pair<int, double>> Grafo::getVizinhos(int vertice) { return adjacencia[vertice]; }

std::vector<std::tuple<int, int, double>> Grafo::getArestas() { return arestas; }