#include <iostream>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include <math.h>

struct Vecteur {
    float xyz[3]; // les composantes
    Vecteur() {
        xyz[0] = 0.0;
        xyz[1] = 0.0;
        xyz[2] = 0.0;
    }

    Vecteur(float x, float y, float z) { // constructeur
        xyz[0] = x;
        xyz[1] = y;
        xyz[2] = z;
    }

    // Retourne le vecteur dont les composantes sont les minima des
    // composantes de soi-même et de other.
    Vecteur inf(const Vecteur &other) const {
        return Vecteur(std::min(xyz[0], other.xyz[0]), std::min(xyz[1], other.xyz[1]), std::min(xyz[2], other.xyz[2]));

    }

    // Retourne le vecteur dont les composantes sont les maxima des
    // composantes de soi-même et de other.
    Vecteur sup(const Vecteur &other) const {
        return Vecteur(std::max(xyz[0], other.xyz[0]), std::max(xyz[1], other.xyz[1]), std::max(xyz[2], other.xyz[2]));
    }

    Vecteur normaliser(){
        float norme = sqrt(xyz[0]*xyz[0] + xyz[1]*xyz[1] + xyz[2]*xyz[2]);
        if (norme != 0){
            float x = - xyz[0] / norme;
            float y = - xyz[1] / norme;
            float z = - xyz[2] / norme;
            return Vecteur(x, y, z);
        }
        return Vecteur(xyz[0], xyz[1], xyz[2]);
    }

    Vecteur cross( const Vecteur& v ) const{
        float x = xyz[1]*v.xyz[2] - xyz[2]*v.xyz[1];
        float y = xyz[2]*v.xyz[0] - xyz[0]*v.xyz[2];
        float z = xyz[0]*v.xyz[1] - xyz[1]*v.xyz[0];
        return Vecteur(x, y, z);
    }

    float operator[](int i) const {      // accesseur en lecture
        assert(i >= 0 || i <= 2);
        return xyz[i];
    }

    float &operator[](int i) {           // accesseur en ecriture
        assert(i >= 0 || i <= 2);
        return xyz[i];
    }
};

inline std::ostream &operator<<(std::ostream &out, Vecteur v) {
    out << v[0] << " " << v[1] << " " << v[2];
    return out;
}


inline std::istream &operator>>(std::istream &in, Vecteur &v) {
    in >> v[0] >> v[1] >> v[2];
    return in;
}

struct Triangle {
    Vecteur v1, v2, v3;
    Triangle(Vecteur _v1, Vecteur _v2, Vecteur _v3) : v1(_v1), v2(_v2), v3(_v3) {}
    Vecteur normal() const{
        Vecteur ac = Vecteur(v3.xyz[0] - v1.xyz[0], v3.xyz[1] - v1.xyz[1], v3.xyz[2] - v1.xyz[2]);
        Vecteur ab = Vecteur(v2.xyz[0] - v1.xyz[0], v2.xyz[1] - v1.xyz[1], v2.xyz[2] - v1.xyz[2]);
        return ac.cross(ab).normaliser();
    }
};

struct TriangleSoup {
    std::vector <Triangle> triangles; // les triangles
    TriangleSoup() {}

    void boundingBox(Vecteur &low, Vecteur &up) const {
        low = triangles[0].v1.inf(triangles[0].v2.inf(triangles[0].v3));
        up = triangles[0].v1.sup(triangles[0].v2.sup(triangles[0].v3));
        for (auto triangle : triangles) {
            low = low.inf(triangle.v1.inf(triangle.v2.inf(triangle.v3)));
            up = up.sup(triangle.v1.sup(triangle.v2.sup(triangle.v3)));
        }
    }

    void read(std::istream &input) {
        // teste si tout va bien
        if (!input.good()) {
            std::cerr << "Le fichier donner n'est pas valide !";
            return;
        }

        std::string line;
        // Parcoure du fichier
        while (!input.eof()) {
            std::getline(input, line);
            if (!line.empty() && line[0] != '#') {
                Vecteur v1;
                Vecteur v2;
                Vecteur v3;
                std::stringstream ss(line);
                // on recupere les trois sommets du triangle
                ss >> v1 >> v2 >> v3;
                // on ajoute le triangle à notre liste de triangles
                triangles.push_back(Triangle(v1, v2, v3));
            }
        }
    }
};

/// Définit un index sur 3 entiers. Toutes les opérations usuelles
/// sont surchargées (accès, comparaisons, égalité).
struct Index {
    int idx[ 3 ];
    Index() {}
    Index( int i0, int i1, int i2 ) {
        idx[0] = i0;
        idx[1] = i1;
        idx[2] = i2;
    }
    Index( int indices[] ) {
        idx[0] = indices[0];
        idx[1] = indices[1];
        idx[2] = indices[2];
    }
    int  operator[]( int i ) const { return idx[ i ]; }
    int& operator[]( int i )       { return idx[ i ]; }
    bool operator<( const Index& other ) const {
        return (idx[0] < other.idx[0])
               || ((idx[0] == other.idx[0])
                    && ((idx[1] < other.idx[1])
                         || ((idx[1] == other.idx[1])
                              && (idx[2] < other.idx[2]))));
    }
    bool operator==( const Index& other ) const {
        return (idx[0] == other.idx[0] && idx[1] == other.idx[1] && idx[2] == other.idx[2]);
    }
};

struct TriangleSoupZipper {
    Vecteur low, up;
    Index cellSize;
    /// Construit le zipper avec une soupe de triangle en entrée \a
    /// anInput, une soupe de triangle en sortie \a anOutput, et un index \a size
    /// qui est le nombre de cellules de la boîte découpée selon les 3 directions.
    TriangleSoupZipper( const TriangleSoup& anInput, TriangleSoup& anOuput, Index size ) {
        anInput.boundingBox(low, up);
        int sizeX = (up[0] - low[0]) / size[0];
        int sizeY = (up[1] - low[1]) / size[1];
        int sizeZ = (up[2] - low[2]) / size[2];
        cellSize = Index(sizeX, sizeY, sizeZ);
        zip(anInput, anOuput);
    }
    /// @return l'index de la cellule dans laquelle tombe \a p.
    Index index( const Vecteur& p ) const {
        int x = static_cast<int>((p[0] - low[0]) / cellSize[0]);
        int y = static_cast<int>((p[1] - low[1]) / cellSize[1]);
        int z = static_cast<int>((p[2] - low[2]) / cellSize[2]);
        return Index(x, y, z);
    }
    /// @return le centroïde de la cellule d'index \a idx (son "centre").
    Vecteur centroid( const Index& idx ) const {
        float x = ((idx[0] + 1) * cellSize[0]) * 0.5;
        float y = ((idx[1] + 1) * cellSize[1]) * 0.5;
        float z = ((idx[2] + 1) * cellSize[2]) * 0.5;
        return Vecteur(x, y, z);
    }
    void zip(const TriangleSoup& anInput, TriangleSoup& anOutput) {
        for (auto triangle : anInput.triangles){
            Index indexA = index(triangle.v1);
            Index indexB = index(triangle.v2);
            Index indexC = index(triangle.v3);
            if(!(indexA == indexB && indexA == indexC)){
                anOutput.triangles.push_back(Triangle(centroid(indexA), centroid(indexB), centroid(indexC)));
            }
        }
    }
};