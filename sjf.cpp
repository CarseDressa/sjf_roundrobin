#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

class Cliente {
private:
    std::string nombre;
    int llegada;
    int articulos;
    int inicio = 0;
    int fin = 0;
    bool atendido = false;

public:
    Cliente(std::string nombre, int llegada, int articulos)
        : nombre(nombre), llegada(llegada), articulos(articulos) {}

    void atender(int momento) {
        inicio = momento;
        fin = inicio + articulos;
        atendido = true;
    }

    std::string getNombre() const { return nombre; }
    int getLlegada() const { return llegada; }
    int getArticulos() const { return articulos; }
    int getFin() const { return fin; }
    bool estaAtendido() const { return atendido; }
    int espera() const { return inicio - llegada; }
    int retorno() const { return fin - llegada; }

    void mostrar() const {
        std::cout << std::left << std::setw(9) << nombre << std::setw(9) << llegada << std::setw(11) << articulos
                  << std::setw(8) << inicio << std::setw(8) << fin << std::setw(9) << retorno() << espera() << "\n";
    }
};

class Caja {
private:
    std::vector<Cliente> clientes;
    std::vector<std::string> ordenAtencion;

    int elegirMasCorto(int tiempo) const {
        int elegido = -1;
        for (size_t i = 0; i < clientes.size(); i++) {
            if (clientes[i].estaAtendido() || clientes[i].getLlegada() > tiempo) continue;
            if (elegido == -1 || clientes[i].getArticulos() < clientes[elegido].getArticulos()) {
                elegido = static_cast<int>(i);
            }
        }
        return elegido;
    }

    int proximaLlegada() const {
        int proxima = -1;
        for (const Cliente &c : clientes) {
            if (!c.estaAtendido() && (proxima == -1 || c.getLlegada() < proxima)) proxima = c.getLlegada();
        }
        return proxima;
    }

public:
    void llegar(const Cliente &cliente) { clientes.push_back(cliente); }

    void atenderTodos() {
        int tiempo = 0;
        size_t pendientes = clientes.size();

        while (pendientes > 0) {
            int elegido = elegirMasCorto(tiempo);
            if (elegido == -1) {
                tiempo = proximaLlegada();
                continue;
            }
            clientes[elegido].atender(tiempo);
            tiempo = clientes[elegido].getFin();
            ordenAtencion.push_back(clientes[elegido].getNombre());
            pendientes--;
        }
    }

    void mostrarResultados() const {
        std::cout << "Orden de atencion: ";
        for (size_t i = 0; i < ordenAtencion.size(); i++) {
            std::cout << ordenAtencion[i] << (i + 1 < ordenAtencion.size() ? " -> " : "\n\n");
        }

        std::cout << std::left << std::setw(9) << "Cliente" << std::setw(9) << "Llegada" << std::setw(11)
                  << "Articulos" << std::setw(8) << "Inicio" << std::setw(8) << "Final" << std::setw(9) << "Retorno"
                  << "Espera\n";

        double sumaEspera = 0;
        double sumaRetorno = 0;
        for (const Cliente &cliente : clientes) {
            cliente.mostrar();
            sumaEspera += cliente.espera();
            sumaRetorno += cliente.retorno();
        }

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\nEspera promedio:  " << sumaEspera / clientes.size() << "\n";
        std::cout << "Retorno promedio: " << sumaRetorno / clientes.size() << "\n";
    }
};

int main() {
    Caja caja;

    caja.llegar(Cliente("P1", 0, 8));
    caja.llegar(Cliente("P2", 1, 4));
    caja.llegar(Cliente("P3", 2, 9));
    caja.llegar(Cliente("P4", 3, 5));

    caja.atenderTodos();
    caja.mostrarResultados();

    return 0;
}