#include <iostream>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <vector>
#include <sstream>
#include <algorithm>

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

    Triangle(Vecteur _v1, Vecteur _v2, Vecteur _v3) : v1(_v1), v2(_v2), v3(_v3) {
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