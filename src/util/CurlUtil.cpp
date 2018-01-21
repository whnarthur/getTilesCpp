//
//  CurlUtil.cpp
//  getTiles
//
//  Created by Wei Hainan on 2018/1/8.
//
//

#include "CurlUtil.hpp"
bool CurlUtil::download(const string& downloadUrl, const string& savePath){
    CURL* curl;
    CURLcode res;
    
    curl = curl_easy_init();
    
    FILE* fp = fopen(savePath.c_str(),"wb");
    res = curl_easy_setopt(curl, CURLOPT_URL, downloadUrl.c_str());
    if(res != CURLE_OK)
    {
        curl_easy_cleanup(curl);
        return false;
    }
    
    res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    res = curl_easy_perform(curl);
    fclose(fp);
    curl_easy_cleanup(curl);
    return true;
}
