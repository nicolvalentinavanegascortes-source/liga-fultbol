#include <iostream>   // para usar cout y cin
#include <fstream>    // para leer y escribir archivos
#include <sstream>    // para dividir texto (split)
#include <vector>     // para usar vectores
#include <algorithm>  // para usar sort y find
#include <string>     // para manejar texto
#include <iomanip>    // para alinear la tabla
#include <ctime>      // para números aleatorios (simulación)

// ===================== ESTRUCTURAS =====================

// aquí guardo la configuración general de la liga (lo que viene del config.txt)
struct ConfigLiga {
    std::string nombreLiga;
    int victoria, empate, derrota;
    std::vector<std::string> equipos;
};

// esta estructura representa un partido
struct Partido {
    std::string fecha, local, visitante;
    int gl, gv; // goles local y visitante
};

// esta estructura representa cada equipo en la tabla
struct Equipo {
    std::string nombre;
    int PJ=0, PG=0, PE=0, PP=0; // partidos jugados, ganados, etc
    int GF=0, GC=0, DG=0, pts=0; // goles y puntos
};

// ===================== FUNCIONES BASE =====================

// esta función sirve para dividir texto usando un carácter (por ejemplo "|" o "=")
std::vector<std::string> split(const std::string& linea, char delim){
    std::vector<std::string> partes;
    std::stringstream ss(linea);
    std::string temp;

    while(getline(ss,temp,delim)){
        partes.push_back(temp);
    }
    return partes;
}

// esto simula limpiar la pantalla (no es perfecto pero sirve)
void limpiarPantalla(){
    for(int i=0;i<30;i++) std::cout<<"\n";
}

// pausa para que el usuario pueda ver lo que salió antes de volver al menú
void pausa(){
    std::cin.ignore(1000,'\n');
    std::cout<<"\nPresione ENTER para seguir...";
    std::cin.get();
}

// ===================== VALIDACIONES =====================

// revisa si el equipo existe dentro de la lista
bool existeEquipo(const std::vector<std::string>& equipos,const std::string& nombre){
    return std::find(equipos.begin(),equipos.end(),nombre)!=equipos.end();
}

// revisa si el partido ya existe para no repetirlo
bool partidoExiste(const std::vector<Partido>& lista,const Partido& p){
    for(const auto& x:lista){
        if(x.fecha==p.fecha && x.local==p.local && x.visitante==p.visitante){
            return true;
        }
    }
    return false;
}

// ===================== ARCHIVOS =====================

// esta función lee el archivo config.txt y guarda los datos en la estructura
bool leerConfig(ConfigLiga &c){
    std::ifstream f("data/config.txt");

    if(!f.is_open()){
        std::cout<<"No se pudo abrir config.txt\n";
        return false;
    }

    std::string linea;

    while(getline(f,linea)){
        if(linea.empty() || linea[0]=='#') continue;

        auto p = split(linea,'=');
        if(p.size()!=2) continue;

        if(p[0]=="liga") c.nombreLiga=p[1];
        else if(p[0]=="victoria") c.victoria=std::stoi(p[1]);
        else if(p[0]=="empate") c.empate=std::stoi(p[1]);
        else if(p[0]=="derrota") c.derrota=std::stoi(p[1]);
        else if(p[0]=="equipo") c.equipos.push_back(p[1]);
    }

    return true;
}

// lee todos los partidos guardados en el archivo
std::vector<Partido> leerPartidos(){
    std::vector<Partido> lista;
    std::ifstream f("data/partidos.txt");

    if(f.is_open()){
        std::string linea;

        while(getline(f,linea)){
            auto p = split(linea,'|');
            if(p.size()!=5) continue;

            lista.push_back({p[0],p[1],p[2],std::stoi(p[3]),std::stoi(p[4])});
        }
        f.close();
    }

    return lista;
}

// guarda un partido nuevo en el archivo
void guardarPartido(const Partido &p){
    std::ofstream f("data/partidos.txt",std::ios::app);

    if(!f.is_open()){
        std::cout<<"Error guardando\n";
        return;
    }

    f<<p.fecha<<"|"<<p.local<<"|"<<p.visitante<<"|"<<p.gl<<"|"<<p.gv<<"\n";
}

// ===================== TABLA =====================

// esta función actualiza los datos de un equipo (uso puntero para modificar directo)
void actualizar(Equipo *e,int gf,int gc,const ConfigLiga &c){

    e->PJ++;
    e->GF+=gf;
    e->GC+=gc;
    e->DG=e->GF-e->GC;

    // aquí se suman los puntos dependiendo del resultado
    if(gf>gc){
        e->PG++;
        e->pts+=c.victoria;
    }
    else if(gf==gc){
        e->PE++;
        e->pts+=c.empate;
    }
    else{
        e->PP++;
        e->pts+=c.derrota;
    }
}

// construye la tabla completa a partir de los partidos
std::vector<Equipo> armarTabla(const std::vector<Partido>& partidos,const ConfigLiga &c){

    std::vector<Equipo> tabla;

    // primero creo todos los equipos
    for(const auto& nombre:c.equipos){
        Equipo e;
        e.nombre=nombre;
        tabla.push_back(e);
    }

    // luego recorro los partidos y actualizo estadísticas
    for(const auto& p:partidos){
        for(auto& e:tabla){

            if(e.nombre==p.local)
                actualizar(&e,p.gl,p.gv,c);

            if(e.nombre==p.visitante)
                actualizar(&e,p.gv,p.gl,c);
        }
    }

    return tabla;
}

// ordena la tabla según puntos, diferencia de gol y goles
void ordenar(std::vector<Equipo>& tabla){
    std::sort(tabla.begin(),tabla.end(),[](const Equipo&a,const Equipo&b){
        if(a.pts!=b.pts) return a.pts>b.pts;
        if(a.DG!=b.DG) return a.DG>b.DG;
        return a.GF>b.GF;
    });
}

// ===================== FUNCIONES =====================

// muestra la tabla en consola
void mostrarTabla(const std::vector<Equipo>& tabla){
    std::cout<<"\nEquipo        PJ PG PE PP GF GC DG PTS\n";

    for(const auto& e:tabla){
        std::cout<<std::setw(12)<<std::left<<e.nombre
                 <<e.PJ<<" "<<e.PG<<" "<<e.PE<<" "<<e.PP<<" "
                 <<e.GF<<" "<<e.GC<<" "<<e.DG<<" "<<e.pts<<"\n";
    }

    if(!tabla.empty()){
        std::cout<<"\nCampeon: "<<tabla[0].nombre<<"\n";
    }
}

// muestra todos los partidos
void mostrarPartidos(const std::vector<Partido>& lista){
    std::cout<<"\nPARTIDOS:\n";

    for(const auto& p:lista){
        std::cout<<p.fecha<<" "<<p.local<<" "<<p.gl<<"-"<<p.gv<<" "<<p.visitante<<"\n";
    }
}

// calcula estadísticas generales
void estadisticas(const std::vector<Partido>& lista){
    int total=0;

    for(const auto& p:lista){
        total+=p.gl+p.gv;
    }

    std::cout<<"\nPartidos: "<<lista.size();
    std::cout<<"\nGoles: "<<total;

    if(!lista.empty()){
        std::cout<<"\nPromedio: "<<(float)total/lista.size();
    }

    std::cout<<"\n";
}

// muestra una gráfica sencilla en consola
void grafica(const std::vector<Equipo>& tabla){

    std::cout<<"\n--- GOLES POR EQUIPO ---\n\n";

    int maxGoles=0;
    for(const auto& e:tabla){
        if(e.GF>maxGoles) maxGoles=e.GF;
    }

    int escala = (maxGoles>30)? (maxGoles/30+1):1;

    for(const auto& e:tabla){

        std::cout<<std::setw(12)<<std::left<<e.nombre<<" | ";

        for(int i=0;i<e.GF/escala;i++){
            std::cout<<"#";
        }

        std::cout<<" ("<<e.GF<<")\n";
    }

    std::cout<<"\nEscala: "<<escala<<"\n";
}

// crea y muestra el ranking
void ranking(const std::vector<Equipo>& tabla){

    std::ofstream f("data/ranking.txt");

    int pos=1;
    for(const auto& e:tabla){
        int score=e.pts*3+e.GF;
        f<<pos++<<". "<<e.nombre<<" "<<score<<"\n";
    }
    f.close();

    std::ifstream r("data/ranking.txt");
    std::string l;

    std::cout<<"\nRANKING:\n";
    while(getline(r,l)){
        std::cout<<l<<"\n";
    }
}

// exporta la tabla a CSV
void exportarCSV(const std::vector<Equipo>& tabla){
    std::ofstream f("data/tabla.csv");

    f<<"Equipo,PJ,PTS\n";

    for(const auto& e:tabla){
        f<<e.nombre<<","<<e.PJ<<","<<e.pts<<"\n";
    }
}

// simula partidos automáticamente
void simular(const ConfigLiga& c){

    std::ofstream f("data/partidos.txt",std::ios::app);

    for(size_t i=0;i<c.equipos.size();i++){
        for(size_t j=i+1;j<c.equipos.size();j++){
            f<<"AUTO|"<<c.equipos[i]<<"|"<<c.equipos[j]<<"|"
             <<rand()%5<<"|"<<rand()%5<<"\n";
        }
    }

    std::cout<<"Liga simulada\n";
}

// ===================== MENU =====================

// muestra el menú principal
int menu(const std::string& liga){

    limpiarPantalla();

    std::cout<<"==== "<<liga<<" ====\n\n";

    std::cout<<"1. Ver tabla\n";
    std::cout<<"2. Registrar partido\n";
    std::cout<<"3. Ver partidos\n";
    std::cout<<"4. Estadisticas\n";
    std::cout<<"5. Grafica\n";
    std::cout<<"6. Ranking\n";
    std::cout<<"7. Simular\n";
    std::cout<<"8. Exportar CSV\n";
    std::cout<<"9. Salir\n";

    int op;
    std::cin>>op;
    return op;
}

// ===================== MAIN =====================

// aquí arranca todo el programa
int main(){

    srand(time(0)); // para números aleatorios

    ConfigLiga c;

    // si no carga config, se cierra
    if(!leerConfig(c)) return 1;

    while(true){

        int op=menu(c.nombreLiga);

        auto partidos=leerPartidos();
        auto tabla=armarTabla(partidos,c);
        ordenar(tabla);

        if(op==1){ mostrarTabla(tabla); pausa(); }
        else if(op==2){
            Partido p;
            std::cin>>p.fecha>>p.local>>p.visitante>>p.gl>>p.gv;

            if(!existeEquipo(c.equipos,p.local) || !existeEquipo(c.equipos,p.visitante))
                std::cout<<"Equipo invalido\n";
            else if(partidoExiste(partidos,p))
                std::cout<<"Duplicado\n";
            else{
                guardarPartido(p);
                std::cout<<"Guardado\n";
            }
            pausa();
        }
        else if(op==3){ mostrarPartidos(partidos); pausa(); }
        else if(op==4){ estadisticas(partidos); pausa(); }
        else if(op==5){ grafica(tabla); pausa(); }
        else if(op==6){ ranking(tabla); pausa(); }
        else if(op==7){ simular(c); pausa(); }
        else if(op==8){ exportarCSV(tabla); pausa(); }
        else if(op==9) break;
    }

    return 0;
}
