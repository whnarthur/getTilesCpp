//
//  GetTiles.h
//  getTiles
//
//
//

#ifndef GetTiles_h
#define GetTiles_h

#include "TileInfo.hpp"
#include "util/ConcurrentBlockingQueue.h"

#include <utility>
#include <string>

using std::pair;
using std::string;

class GetTiles{
public:
    GetTiles(double minx, double miny, double maxx, double maxy, int minz, int maxz, string tarPath, string baseUrl) : minx_(minx),
                   miny_(miny),
                   maxx_(maxx),
                   maxy_(maxy),
                   minz_(minz),
                   maxz_(maxz),
                   tarPath_(tarPath),
                   baseUrl_(baseUrl){}
    void execute();
protected:
    void createGenDownloadTilesThread(double minx, double miny, double maxx, double maxy, int minz, int maxz);
    void createDownloadThread();
    void createWrite2TarThread(string tarPath);
    
private:
    double minx_, miny_, maxx_, maxy_;
    int minz_, maxz_;
    string tarPath_;
    string baseUrl_;
    ConcurrentBlockingQueue<TileInfor> download_queue_;
    ConcurrentBlockingQueue<pair<TileInfor, string>> write_queue_;
};

#endif /* GetTiles_h */
