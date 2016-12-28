#include <dm/dm.hpp>

using namespace std;

namespace dm {


// From the standard: http://netpbm.sourceforge.net/doc/ppm.html
bool PPMImage::loadFromFile(const string &filepath) {

    cout << "Loading `" << filepath << "'... ";
    FILE *file = fopen(filepath.c_str(), "rb");
    if(!file) {
        cerr << "failed to open file!" << endl;
        return false;
    }

    char car;
    unsigned variant, max_val;
    unsigned w, h;
    unsigned step;
    for(step=0 ; step<3 ; )  {
        car = fgetc(file);
        if(car == EOF) {
            cerr << "ill-formed file!" << endl;
            return false;
        }
        if(car == '#') {
            do car = fgetc(file); while(car != '\n' && car!=EOF);
            if(car == EOF) {
                cerr << "ill-formed file!" << endl;
                return false;
            }
            continue;
        }
        if(!isdigit(car))
            continue;
        fseek(file, -1, SEEK_CUR);
        switch(step) {
        case 0: if(fscanf(file, "%u", &variant) <= 0) { cerr << "ill-formed file!" << endl; return false; } ++step; break;
        case 1: if(fscanf(file, "%u", &w)       <= 0) { cerr << "ill-formed file!" << endl; return false; } ++step; break;
        case 2: if(fscanf(file, "%u", &h)       <= 0) { cerr << "ill-formed file!" << endl; return false; } ++step; break;
        case 3: if(fscanf(file, "%u", &max_val) <= 0) { cerr << "ill-formed file!" << endl; return false; } fgetc(file); break;
        }
    }

    if(variant != 6) {
        cerr << "not a RGB PPM!" << endl;
        return false;
    }
    fseek(file, 5, SEEK_CUR); // Dunno why, we need to do this.

    PPMImage img;
    img.resize(w, h);

    if(fread(img.pixels, 1, 3*w*h, file) != 3*w*h) {
        cerr << "Image size is not as advertised!" << endl;
        return false;
    }
    fclose(file);

    cout << "done." << endl;
    return true;
}


} // namespace dm
