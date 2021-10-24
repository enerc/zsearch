#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {
    fstream fi,fo;
    fi.open("taxis.json",ios::in);
    fo.open("taxis_below100.json",ios::out);
    string tp;
    const char *k="\n";
    while(getline(fi, tp)){
        size_t i = tp.find("total_amount");
        i+= 12;
        while (tp.at(i) != ':') i++;
        i++;
        double g;
        sscanf(&tp.at(i),"%lf",&g);
        if (g < 100) {
            fo.write(tp.c_str(),tp.size());
            fo.write(k,1);
        }

    }
    fi.close();
    fo.close();
}