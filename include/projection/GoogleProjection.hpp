#ifndef GoogleProjection_HPP
#define GoogleProjection_HPP

#include <utility>
#include <vector>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
using namespace std;
using std::vector;
using std::pair;

typedef pair<double, double> pdd;
typedef vector<pdd, pdd > pvdd;


double minmax_mapnik(double a, double b, double c);



class GoogleProjection
{
public:
    GoogleProjection(int levels=18);
    
    pdd fromLLtoPixel(pdd  ll, int zoom);//由经纬度坐标转换为投影坐标
    pdd fromPixelToLL(pdd  px, int zoom);//由投影坐标转换为经纬度坐标
    std::string zeroPadder(std::string str, int order);
    int hex(std::string x);
    std::string ItoS(int x);
    private :
    vector<double> Bc, Cc, Ac;
    vector< pair<double, double> >  zc;
    int c;
    double e;
};



#endif

