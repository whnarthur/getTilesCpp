//
//  main.cpp
//  getTiles
//
//  Created on 2018/1/4.
//
//

#include <iostream>
#include <thread>

#include "GetTiles.h"

using namespace std;

int main(int argc, const char * argv[]) {
    double minx = 108.790841;
    double miny = 24.636323;
    double maxx = 114.261265;
    double maxy = 30.126363;
    int minz = 10;
    int maxz = 10;
    string tarPath = "./hunan_yingxiang_10.tar";
    string baseUrl = "http://mt1.google.cn/maps/vt?lyrs=s%40748&hl=zh-CN&gl=CN";
//    string baseUrl = "http://webst04.is.autonavi.com/appmaptile?style=6";
//    string baseUrl = "http://mt0.google.cn/maps/vt?hl=zh-CN&gl=CN";
//    string baseUrl = "http://ws1.sinaimg.cn/large/006LI11xgy1fn4yqfswx9j30lv0csgnn.jpg";
    //
    GetTiles getTiles(minx, miny, maxx, maxy, minz, maxz, tarPath, baseUrl);
    getTiles.execute();
    return 0;
}
