#include "GoogleProjection.hpp"
#include <algorithm>
#include <vector>
#include <utility>
#include <math.h>
#include <iostream>

const double pi = 3.1415926;
double DEG_TO_RAD = pi/180.0;
double RAD_TO_DEG = 180.0/pi;

double minmax_mapnik(double a, double b, double c)
{
    a = std::max(a, b);
    a = std::min(a, c);
    return a;
}

std::string GoogleProjection::ItoS(int x)
{
    stringstream s;
    s<<x;
    string str = s.str();
    return str;
}

std::string GoogleProjection::zeroPadder(std::string str, int order)
{
    if(std::strlen(str.c_str()) >= order)
        return str;
    std::string data = "";
    for(int i=0; i<(order - std::strlen(str.c_str())); i++)
    {
        data += std::string("0");
        
    }
    int length = std::strlen(str.c_str()) - 1;
    int j = 0;
    while (j <= length)
    {
        data += str[j];
        j ++;
    }
    return data;
}

int GoogleProjection::hex(std::string x)
{
    char* p = const_cast<char*>(x.c_str());
    char str_hex[10];
    sprintf(str_hex,"%x",p);
    return atoi(str_hex);
}

GoogleProjection::GoogleProjection(int levels)
{
    c = 256;
    for(int d=0; d<levels; ++d)
    {
        e = c/2.0;
        Bc.push_back(c / 360.0);
        Cc.push_back(c / (2*pi));
        zc.push_back( std::make_pair(e,e) );
        Ac.push_back(c);
        c *= 2;
    }
}

pdd GoogleProjection::fromLLtoPixel(pdd  ll, int zoom)
{
    pdd d = zc[zoom];
    double e = round( d.first + ll.first * Bc[zoom] );
    double f = minmax_mapnik( sin(DEG_TO_RAD * ll.second), -0.9999, 0.9999 );
    double g = round( d.second + 0.5 * log((1+f) / (1-f))*((-1.0) * Cc[zoom]) );
    return std::make_pair(e, g);
}

pdd GoogleProjection::fromPixelToLL(pdd  px, int zoom)
{
    pdd e = zc[zoom];
    double f = (px.first - e.first) / Bc[zoom];
    double g = (px.second - e.second)/((-1.0) * Cc[zoom]);
    double h = RAD_TO_DEG * (2*atan(exp(g)) -0.5 * pi);
    return std::make_pair(f, h);
}

