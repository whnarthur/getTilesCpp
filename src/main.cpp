//
//  main.cpp
//  getTiles
//
//  Created on 2018/1/4.
//
//

#include <iostream>
#include <thread>
#include "curl/curl.h"
#include "util/CurlUtil.hpp"
#include "GetTiles.h"
#include "fstream"
#include "Eigen/Eigen"

using namespace std;

long writer(void *data, int size, int nmemb, string &content)
{
    long sizes = size * nmemb;
    string temp((char *)data,sizes);
    content += temp;
    return sizes;
}

bool CurlInit(CURL *&curl, const char* url,string &content)
{
    CURLcode code;
    string error;
    curl = curl_easy_init();
    if (curl == NULL)
    {
        printf( "Failed to create CURL connection\n");
        return false;
    }
    code = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error.c_str());
    if (code != CURLE_OK)
    {
        printf( "Failed to set error buffer [%d]\n", code );
        return false;
    }
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    code = curl_easy_setopt(curl, CURLOPT_URL, url);
    if (code != CURLE_OK)
    {
        printf("Failed to set URL [%s]\n", error.c_str());
        return false;
    }
    code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    if (code != CURLE_OK)
    {
        printf( "Failed to set redirect option [%s]\n", error.c_str() );
        return false;
    }
    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
    if (code != CURLE_OK)
    {
        printf( "Failed to set writer [%s]\n", error.c_str());
        return false;
    }
    code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
    if (code != CURLE_OK)
    {
        printf( "Failed to set write data [%s]\n", error.c_str() );
        return false;
    }
    return true;
}



bool GetURLDataBycurl(const char* URL,  string &content)
{
    CURL *curl = NULL;
    CURLcode code;
    string error;
    
    code = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (code != CURLE_OK)
    {
        printf( "Failed to global init default [%d]\n", code );
        return false;
    }
    
    if ( !CurlInit(curl,URL,content) )
    {
        printf( "Failed to global init default [%d]\n" );
        return false;
    }
    code = curl_easy_perform(curl);
    if (code != CURLE_OK)
    {
        printf( "Failed to get '%s' [%s]\n", URL, error.c_str());
        return false;
    }
    long retcode = 0;
    code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode);
    if ( (code == CURLE_OK) && retcode == 200 )
    {
        double length = 0;
        code = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD , &length);
        printf("%d",retcode);
        FILE * file = fopen("1.gif","wb");
        fseek(file,0,SEEK_SET);
        fwrite(content.c_str(),1,length,file);
        fclose(file);
        
        //struct curl_slist *list;
        //code = curl_easy_getinfo(curl,CURLINFO_COOKIELIST,&list);
        //curl_slist_free_all (list);
        
        return true;
    }
    else
    {
        //    debug1( "%s \n ",getStatusCode(retcode));
        return false;
    }
    curl_easy_cleanup(curl);
    return false;
}


int main(int argc, const char * argv[]) {
    Eigen::Vector3d v2(116.51200787337888,39.82185131243076,0.1);
    Eigen::Vector3d v1(-0.00005171780156842942,-0.000023449875790504393,0);
    Eigen::Vector3d corsscheck = v2.cross(v1);
    cout<<corsscheck.normalized();
    
    double minx = 108.790841;
    double miny = 24.636323;
    double maxx = 114.261265;
    double maxy = 30.126363;
    int minz = 10;
    int maxz = 10;
    string tarPath = "./hunan_yingxiang_10.tar";
//    string baseUrl = "http://mt1.google.cn/maps/vt?lyrs=s%40748&hl=zh-CN&gl=CN";
    string baseUrl = "http://khms0.googleapis.com/kh?v=744&hl=zh-CN";
//    string baseUrl = "https://a.tiles.mapbox.com/v4/digitalglobe.316c9a2e/17/107937/49547.png?access_token=pk.eyJ1IjoiZGlnaXRhbGdsb2JlIiwiYSI6ImNqOGRmNXltOTBucm0yd3BtY3E5czl6NmYifQ.qJJsPgCjyzMCm3YG3YWQBQ";
//    string baseUrl = "http://webst04.is.autonavi.com/appmaptile?style=6";
//    string baseUrl = "http://mt0.google.cn/maps/vt?hl=zh-CN&gl=CN";
//    string baseUrl = "http://ws1.sinaimg.cn/large/006LI11xgy1fn4yqfswx9j30lv0csgnn.jpg";
    //
    GetTiles getTiles(minx, miny, maxx, maxy, minz, maxz, tarPath, baseUrl, true);
//    getTiles.execute();
    
    char *url ="http://mt0.google.cn/maps/vt?lyrs=s%40748&hl=zh-CN&gl=CN&x=26&y=10&z=5";
    string content;
//    if ( GetURLDataBycurl(url,content))
//    {
//        ofstream fp;
//        fp.open("1.png");
//        fp<<content;
//        fp.close();
//        printf("%s\n",content.c_str());
//        
//    }
    CurlUtil::download(url, "2.png");
    
    return 0;
}
