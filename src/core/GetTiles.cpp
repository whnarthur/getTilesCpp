//
//  GetTiles.cpp
//  getTiles
//
//
//
#include "core/GetTiles.h"
#include "projection/GoogleProjection.hpp"
#include "util/RequestUtil.h"

#include <math.h>

#include "Poco/File.h"
#include "Poco/Path.h"

#include <archive.h>
#include <archive_entry.h>

typedef pair<double, double> pdd;

void GetTiles::execute(){
    unsigned int core_nums = 1;
//    unsigned int core_nums = std::thread::hardware_concurrency();
    std::thread genDownLoadTiles_thread(&GetTiles::createGenDownloadTilesThread, this, minx_, miny_, maxx_, maxy_, minz_, maxz_);
    //
    std::thread *download_threads = new std::thread[core_nums];
    for (int i = 0; i < core_nums; ++i) {
        download_threads[i] = std::thread(&GetTiles::createDownloadThread, this);
    }
    //
    std::thread write2tar_thread(&GetTiles::createWrite2TarThread, this, tarPath_);
    //
    genDownLoadTiles_thread.join();
    //
    for (int i = 0; i < core_nums; ++i) {
        TileInfor tileInforEnd(1, 1, 1);
        tileInforEnd.endFlag = true;
        download_queue_.push(tileInforEnd);
    }
    for (int i = 0; i < core_nums; ++i) {
        download_threads[i].join();
    }
    //
    pair<TileInfor, string> result;
    result.second = "";
    write_queue_.push(result);
    write2tar_thread.join();
}

void GetTiles::createGenDownloadTilesThread(double minx, double miny, double maxx, double maxy, int minz, int maxz){
    GoogleProjection gprj(maxz + 1);
    //
    pdd ll0;
    pdd ll1;
    ll0.first = minx;
    ll0.second = maxy;
    ll1.first = maxx;
    ll1.second = miny;
    //
    for (int z = minz; z <= maxz; ++z)
    {
        pdd px0 = gprj.fromLLtoPixel(ll0, z);
        pdd px1 = gprj.fromLLtoPixel(ll1, z);
        //
        int minx_range = int(px0.first / 256.0);
        int maxx_range = int(px1.first / 256.0) + 1;
        int miny_range = int(px0.second / 256.0);
        int maxy_range = int(px1.second / 256.0) + 1;
        //
        for (int x = minx_range; x < maxx_range; ++x)
        {
            if ( x<0 || x>=pow(2,z) )
                continue;
            //
            for (int y = miny_range; y < maxy_range; ++y)
            {
                if ( y<0 || y>=pow(2,z) )
                    continue;
                TileInfor tileInfor(x, y, z);
                download_queue_.push(tileInfor);
            }
        }
    }
}

void GetTiles::createDownloadThread(){
    CServiceRequestUtil requestUtil;
    while (true) {
        TileInfor tileInfor;
        download_queue_.pop(tileInfor);
        //
        if (tileInfor.endFlag) {
            break;
        }
        //
        char *data;
        size_t dataSize=0;
        char szUrl[256] = {0};
        snprintf(szUrl, 256, "%s&z=%d&x=%d&y=%d", baseUrl_.c_str(), tileInfor.getZ(), tileInfor.getX(), tileInfor.getY() );
//        snprintf(szUrl, 256, "%s", baseUrl_.c_str());
        if (requestUtil.HttpGetBinary(szUrl, &data, &dataSize)) {
            string pngData(data, dataSize);
            write_queue_.push(make_pair(tileInfor, pngData));
            delete data;
        }else{
            cout<<szUrl<<endl;
        }
    }
}

void GetTiles::createWrite2TarThread(string tarPath){
    struct archive *tarfile;
    struct archive_entry *entry;
    tarfile = archive_write_new();
    archive_write_set_format_pax_restricted(tarfile);
    archive_write_open_filename(tarfile, tarPath.c_str());
    //
    while (true) {
        pair<TileInfor, string> result;
        write_queue_.pop(result);
        //
        if (result.second=="") {
            break;
        }
        //
        char szUrl[256] = {0};
        TileInfor tile = result.first;
        int z = tile.getZ();
        int x = tile.getX();
        int y = tile.getY();
        //
        if(isWriteTmpFile_){
            snprintf(szUrl, 256, "./%d/%d/%d/", z, x/10, y/10);
            Poco::File outDir(szUrl);
            outDir.createDirectories();
            //
            snprintf(szUrl, 256, "%d/%d/%d/%d_%d.png", z, x/10, y/10, x, y);
            ofstream fp;
            fp.open(szUrl);
            fp<<result.second;
            fp.close();
        }
        //
        snprintf(szUrl, 256, "%d/%d/%d/%d_%d.png", z, x/10, y/10, x, y);
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        entry = archive_entry_new();
        archive_entry_set_pathname(entry, szUrl);
        archive_entry_set_size(entry, result.second.size());
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_mtime(entry, ts.tv_sec, ts.tv_nsec);
        archive_entry_set_perm(entry, 0644);
        archive_write_header(tarfile, entry);
        archive_write_data(tarfile, result.second.c_str(), result.second.size());
        archive_entry_free(entry);
    }
    //
    archive_write_close(tarfile);
    archive_write_free(tarfile);
}
