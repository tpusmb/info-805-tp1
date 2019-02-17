#include <qapplication.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Viewer.h"

using namespace std;

int main(int argc, char **argv) {
    // Read command lines arguments.
    QApplication application(argc, argv);

    TriangleSoup iSoup;
    TriangleSoup iSoupZip;
    ifstream input(argv[1]);
    iSoup.read(input);
    input.close();
    TriangleSoupZipper(iSoup, iSoupZip, Index(10, 10, 10));
    // Instantiate the viewer. le faire sous cette forme car le contructeur a ete redefini
    Viewer viewer(&iSoupZip);
    // Give a name
    viewer.setWindowTitle("Viewer triangle soup");
    // Make the viewer window visible on screen.
    viewer.show();
    // Run main loop.
    application.exec();
    return 0;
}
