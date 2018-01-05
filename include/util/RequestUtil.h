/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   RequestUtil.h
 *
 */

#ifndef REQUESTUTIL_H
#define REQUESTUTIL_H

#include <map>
#include <string>
using namespace std;

#include "Poco/Net/HTTPServerRequest.h"
using namespace Poco::Net;

class CServiceRequestUtil{
    
public:
    
    static bool GetRequestParameters(HTTPServerRequest & request,
                                     std::map<std::string, std::string> & mapParameters);
    
    static bool GetRequestParameters(HTTPServerRequest & request, string & postBody);
    
    static std::string GetRequestParameter(const std::map<std::string, std::string> & mapParameters,
                                           std::string key, std::string defaultValue);
    
    static bool HttpPost(std::string url, std::string postValue, std::string & result);
    
    static bool HttpGet(std::string url, std::string & result);
    
    static bool HttpPost(std::string url, const std::map<std::string, std::string> & formValues, std::string & result);
    
    static bool HttpPostGetBinary(std::string url, const std::map<std::string, std::string> & formValues,char ** result, size_t * imageSize);
    
    static bool HttpPostSetBinary(std::string url, string filename, const std::map<std::string, std::string> & formValues, std::string & result);
    
    static bool HttpGetBinary(std::string url, char ** result, size_t * imageSize);
    
};



#endif /* REQUESTUTIL_H */


