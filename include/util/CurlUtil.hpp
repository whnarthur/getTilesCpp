//
//  CurlUtil.hpp
//  getTiles
//
//  Created by Wei Hainan on 2018/1/8.
//
//

#ifndef CurlUtil_hpp
#define CurlUtil_hpp

#include <string>
#include "curl/curl.h"

using std::string;

class CurlUtil{
public:
    /**
     下载指定url的内容到savePath
     @param downloadUrl 要下载的url地址
     @param savePath 要保存的路径
     @return 成功返回true
     */
    static bool download(const string& downloadUrl, const string& savePath);
};
#endif /* CurlUtil_hpp */
