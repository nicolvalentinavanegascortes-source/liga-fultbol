#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>
#include <windows.h>
#include <thread>
#include <chrono>
#include <ctime>

// ===================== ESTRUCTURAS =====================

struct ConfigLiga {
    std::string nombreLiga;
    int victoria, empate, derrota;
    std::vector<std::string> equipos;
};

struct Partido {
    std::string fecha, local, visitante;
    int gl, gv;
};

struct Equipo {
    std::string nombre;
    int PJ=0, PG=0, PE=0, PP=0;
    int GF=0, GC=0, DG=0, pts=0;
};

// ===================== VISUAL =====================

void color(int c){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void pausa(){
    std::cout << "\nPresione ENTER para continuar...";
    std::cin.ignore();
    std::cin.get();
}

void loading(){
    std::cout << "\nCargando";
    for(int i=0;i<4;i++){
        std::cout << ".";
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    std::cout << "\n";
}

// ===================== AUX =====================

std::vector<std::string> split(std::string linea, char delim){
    std::vector<std::string> v;
    std::stringstream ss(linea);
    std::string temp;
    while(getline(ss,temp,delim)) v.push_back(temp);
    return v;
}

void limpiarEntrada(){
    std::cin.clear();
    std::cin.ignore(1000,'\n');
}

// ===================== VALIDACIONES =====================

bool existeEquipo(const std::vector<std::string>& equipos, const std::string& nombre){
    for(const auto& e: equipos)
        if(e == nombre) return true;
    return false;
}

bool partidoExiste(const std::vector<Partido>& partidos, const Partido& p){
    for(const auto& x: partidos){
        if(x.local == p.local &&
           x.visitante == p.visitante &&
           x.fecha == p.fecha){
            return true;
        }
    }
    return false;
}

// ===================== ARCHIVOS =====================

bool leerConfig(ConfigLiga &config){
    std::ifstream f("data/config.txt");

    if(!f.is_open()){
        std::cout << "Error: no se pudo abrir config.txt\n";
        return false;
    }

    std::string l;

    while(getline(f,l)){
        if(l.empty() || l[0]=='#') continue;

        auto p = split(l,'=');
        if(p.size()!=2) continue;

        if(p[0]=="liga") config.nombreLiga=p[1];
        else if(p[0]=="victoria") config.victoria=stoi(p[1]);
        else if(p[0]=="empate") config.empate=stoi(p[1]);
        else if(p[0]=="derrota") config.derrota=stoi(p[1]);
        else if(p[0]=="equipo") config.equipos.push_back(p[1]);
    }

    f.close();
    return true;
}

std::vector<Partido> leerPartidos(){
    std::vector<Partido> v;
    std::ifstream f("data/partidos.txt");

    if(f.is_open()){
        std::string l;
        while(getline(f,l)){
            auto p = split(l,'|');
            if(p.size()!=5) continue;

            Partido x{p[0],p[1],p[2],stoi(p[3]),stoi(p[4])};
            v.push_back(x);
        }
        f.close();
    } else {
        std::cout << "Advertencia: no se pudo abrir partidos.txt\n";
    }

    return v;
}

void guardarPartido(const Partido &p){
    std::ofstream f("data/partidos.txt", std::ios::app);

    if(f.is_open()){
        f << p.fecha << "|" << p.local << "|" << p.visitante
          << "|" << p.gl << "|" << p.gv << "\n";
        f.close();
    }
}

// ===================== TABLA =====================

void actualizarEquipo(Equipo* e,int gf,int gc,const ConfigLiga& c){
    e->PJ++;
    e->GF += gf;
    e->GC += gc;
    e->DG = e->GF - e->GC;

    if(gf > gc){
        e->PG++;
        e->pts += c.victoria;
    } else if(gf == gc){
        e->PE++;
        e->pts += c.empate;
    } else {
        e->PP++;
        e->pts += c.derrota;
    }
}

std::vector<Equipo> construirTabla(const std::vector<Partido>& partidos,const ConfigLiga& c){
    std::vector<Equipo> t;

    for(const auto& n: c.equipos){
        Equipo e;
        e.nombre = n;
        t.push_back(e);
    }

    for(const auto& p: partidos){
        for(auto& e: t){
            if(e.nombre == p.local)
                actualizarEquipo(&e,p.gl,p.gv,c);

            if(e.nombre == p.visitante)
                actualizarEquipo(&e,p.gv,p.gl,c);
        }
    }

    return t;
}

void ordenarTabla(std::vector<Equipo>& t){
    std::sort(t.begin(),t.end(),[](const Equipo&a,const Equipo&b){
        if(a.pts != b.pts) return a.pts > b.pts;
        if(a.DG != b.DG) return a.DG > b.DG;
        return a.GF > b.GF;
    });
}

// ===================== VISUAL =====================

void mostrarTabla(const std::vector<Equipo>& t){
    std::cout << "\n# Equipo        PJ PG PE PP GF GC DG PTS\n";

    for(const auto& e: t){
        std::cout << std::setw(15) << std::left << e.nombre
                  << e.PJ << " " << e.PG << " " << e.PE << " "
                  << e.PP << " " << e.GF << " " << e.GC << " "
                  << e.DG << " " << e.pts << "\n";
    }

    if(!t.empty()){
        std::cout << "\n🏆 Campeon: " << t[0].nombre << "\n";
    }
}

// ===================== ESTADÍSTICAS =====================

void mostrarEstadisticas(const std::vector<Partido>& partidos){
    int total = 0;

    for(const auto& p: partidos){
        total += p.gl + p.gv;
    }

    std::cout << "\nPartidos: " << partidos.size();
    std::cout << "\nGoles totales: " << total;

    if(!partidos.empty()){
        std::cout << "\nPromedio: " << (float)total / partidos.size();
    }

    std::cout << "\n";
}

// ===================== GRÁFICAS =====================

void graficaGoles(const std::vector<Equipo>& t){
    std::cout << "\n📊 GOLES POR EQUIPO\n\n";

    for(const auto& e: t){
        std::cout << std::setw(12) << std::left << e.nombre << " | ";

        for(int i=0; i<e.GF/2; i++){
            std::cout << "█";
        }

        std::cout << " (" << e.GF << ")\n";
    }
}

// ===================== RANKING =====================

void guardarRanking(const std::vector<Equipo>& t){
    std::ofstream f("data/ranking.txt");

    if(f.is_open()){
        int pos = 1;

        for(const auto& e: t){
            int score = e.pts*3 + e.GF;

            f << pos++ << ". "
              << e.nombre << " - "
              << score << " pts\n";
        }

        f.close();
    }
}

void mostrarRanking(){
    std::ifstream f("data/ranking.txt");

    if(!f.is_open()){
        std::cout << "No hay ranking disponible\n";
        return;
    }

    std::string l;
    std::cout << "\n🏆 RANKING FIFA\n\n";

    while(getline(f,l)){
        std::cout << l << "\n";
    }

    f.close();
}

// ===================== CSV =====================

void exportarCSV(const std::vector<Equipo>& t){
    std::ofstream f("data/tabla.csv");

    if(f.is_open()){
        f << "Equipo,PJ,PG,PE,PP,GF,GC,DG,PTS\n";

        for(const auto& e: t){
            f << e.nombre << ","
              << e.PJ << ","
              << e.PG << ","
              << e.PE << ","
              << e.PP << ","
              << e.GF << ","
              << e.GC << ","
              << e.DG << ","
              << e.pts << "\n";
        }

        f.close();
    }
}

// ===================== PARTIDOS =====================

void mostrarPartidos(const std::vector<Partido>& p){
    std::cout << "\n=== PARTIDOS ===\n";

    for(const auto& x: p){
        std::cout << x.fecha << " | "
                  << x.local << " " << x.gl
                  << " - " << x.gv << " "
                  << x.visitante << "\n";
    }
}

// ===================== REGISTRAR =====================

void registrarPartido(const ConfigLiga& c){
    Partido p;
    auto partidos = leerPartidos();

    std::cout << "Fecha: "; std::cin >> p.fecha;
    std::cout << "Local: "; std::cin >> p.local;
    std::cout << "Visitante: "; std::cin >> p.visitante;

    if(!existeEquipo(c.equipos,p.local) ||
       !existeEquipo(c.equipos,p.visitante)){
        std::cout << "Equipo invalido\n";
        return;
    }

    std::cout << "Goles local: "; std::cin >> p.gl;
    std::cout << "Goles visitante: "; std::cin >> p.gv;

    if(p.gl < 0 || p.gv < 0){
        std::cout << "Error goles\n";
        return;
    }

    if(partidoExiste(partidos,p)){
        std::cout << "Partido duplicado\n";
        return;
    }

    guardarPartido(p);
    std::cout << "Partido guardado\n";
}

// ===================== SIMULACIÓN =====================

void simularLiga(const ConfigLiga& c){
    std::ofstream f("data/partidos.txt");

    if(f.is_open()){
        for(int i=0;i<c.equipos.size();i++){
            for(int j=i+1;j<c.equipos.size();j++){
                f << "AUTO|" << c.equipos[i] << "|"
                  << c.equipos[j] << "|"
                  << rand()%5 << "|" << rand()%5 << "\n";
            }
        }
        f.close();
    }
}

// ===================== MENU =====================

int menu(const std::string& liga){
    system("cls");

    color(11);
    std::cout << "==== " << liga << " ====\n";

    color(10);
    std::cout << "1. Ver tabla\n";
    std::cout << "2. Registrar partido\n";
    std::cout << "3. Ver partidos\n";
    std::cout << "4. Estadisticas\n";
    std::cout << "5. Grafica\n";
    std::cout << "6. Ranking FIFA\n";
    std::cout << "7. Simular liga\n";
    std::cout << "8. Exportar CSV\n";
    std::cout << "9. Salir\n";

    color(7);

    int op;
    std::cin >> op;

    return op;
}

// ===================== MAIN =====================

int main(){

    srand(time(0));

    ConfigLiga c;

    if(!leerConfig(c)) return 1;

    while(true){

        int op = menu(c.nombreLiga);

        auto partidos = leerPartidos();
        auto tabla = construirTabla(partidos,c);

        ordenarTabla(tabla);

        if(op==1){ loading(); mostrarTabla(tabla); pausa(); }
        else if(op==2){ registrarPartido(c); pausa(); }
        else if(op==3){ mostrarPartidos(partidos); pausa(); }
        else if(op==4){ mostrarEstadisticas(partidos); pausa(); }
        else if(op==5){ graficaGoles(tabla); pausa(); }
        else if(op==6){ guardarRanking(tabla); mostrarRanking(); pausa(); }
        else if(op==7){ simularLiga(c); std::cout<<"Liga simulada\n"; pausa(); }
        else if(op==8){ exportarCSV(tabla); std::cout<<"CSV generado\n"; pausa(); }
        else if(op==9) break;
    }
}
