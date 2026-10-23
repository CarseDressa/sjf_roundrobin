#include <algorithm>
#include <iomanip>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

struct Tramo {
    std::string nombre;
    int desde;
    int hasta;
};

class Cliente {
private:
    std::string nombre;
    int llegada;
    int articulos;
    int restante;
    int primerInicio = -1;
    int fin = 0;

public:
    Cliente(std::string nombre, int llegada, int articulos)
        : nombre(nombre), llegada(llegada), articulos(articulos), restante(articulos) {}

    int ejecutar(int momento, int quantum) {
        if (primerInicio == -1) primerInicio = momento;
        int usado = std::min(quantum, restante);
        restante -= usado;
        return usado;
    }

    void terminar(int momento) { fin = momento; }

    std::string getNombre() const { return nombre; }
    int getLlegada() const { return llegada; }
    bool terminado() const { return restante == 0; }
    int retorno() const { return fin - llegada; }
    int espera() const { return retorno() - articulos; }
    int respuesta() const { return primerInicio - llegada; }

    void mostrar() const {
        std::cout << std::left << std::setw(9) << nombre << std::setw(9) << llegada << std::setw(11) << articulos
                  << std::setw(8) << fin << std::setw(9) << retorno() << std::setw(8) << espera() << respuesta()
                  << "\n";
    }
};

class Caja {
private:
    std::vector<Cliente> clientes;
    std::queue<int> cola;
    std::vector<Tramo> lineaTiempo;
    int quantum;
    size_t siguiente = 0;

    void admitirLlegadas(int tiempo) {
        while (siguiente < clientes.size() && clientes[siguiente].getLlegada() <= tiempo) {
            cola.push(static_cast<int>(siguiente));
            siguiente++;
        }
    }

public:
    explicit Caja(int quantum) : quantum(quantum) {}

    void llegar(const Cliente &cliente) { clientes.push_back(cliente); }

    void atenderTodos() {
        std::stable_sort(clientes.begin(), clientes.end(),
                         [](const Cliente &a, const Cliente &b) { return a.getLlegada() < b.getLlegada(); });

        int tiempo = 0;
        size_t terminados = 0;

        while (terminados < clientes.size()) {
            admitirLlegadas(tiempo);
            if (cola.empty()) {
                tiempo = clientes[siguiente].getLlegada();
                continue;
            }

            int i = cola.front();
            cola.pop();

            int desde = tiempo;
            tiempo += clientes[i].ejecutar(tiempo, quantum);
            lineaTiempo.push_back({clientes[i].getNombre(), desde, tiempo});

            admitirLlegadas(tiempo);

            if (clientes[i].terminado()) {
                clientes[i].terminar(tiempo);
                terminados++;
            } else {
                cola.push(i);
            }
        }
    }

    void mostrarResultados() const {
        std::cout << "Quantum = " << quantum << "\n\nLinea de tiempo:\n";
        for (const Tramo &t : lineaTiempo) {
            std::cout << t.nombre << "[" << t.desde << "-" << t.hasta << "] ";
        }
        std::cout << "\n\n";

        std::cout << std::left << std::setw(9) << "Cliente" << std::setw(9) << "Llegada" << std::setw(11)
                  << "Articulos" << std::setw(8) << "Final" << std::setw(9) << "Retorno" << std::setw(8) << "Espera"
                  << "Respuesta\n";

        double sumaEspera = 0;
        double sumaRetorno = 0;
        double sumaRespuesta = 0;
        for (const Cliente &cliente : clientes) {
            cliente.mostrar();
            sumaEspera += cliente.espera();
            sumaRetorno += cliente.retorno();
            sumaRespuesta += cliente.respuesta();
        }

        double n = static_cast<double>(clientes.size());
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\nEspera promedio:    " << sumaEspera / n << "\n";
        std::cout << "Retorno promedio:   " << sumaRetorno / n << "\n";
        std::cout << "Respuesta promedio: " << sumaRespuesta / n << "\n";
        std::cout << "Cambios de contexto: " << lineaTiempo.size() - 1 << "\n";
    }
};

int main() {
    Caja caja(3);

    caja.llegar(Cliente("P1", 0, 8));
    caja.llegar(Cliente("P2", 1, 4));
    caja.llegar(Cliente("P3", 2, 9));
    caja.llegar(Cliente("P4", 3, 5));

    caja.atenderTodos();
    caja.mostrarResultados();

    return 0;
}