/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <iostream>
#include <Poco/Net/FilePartSource.h>

#include "util/RequestUtil.h"

#include "Poco/URI.h"
#include "Poco/Net/HTTPClientSession.h"

#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/StreamCopier.h"
#include "Poco/Logger.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/JSONException.h"
#include "Poco/Stopwatch.h"

using namespace Poco;
using namespace Poco::JSON;
using namespace Poco::Dynamic;

bool CServiceRequestUtil::GetRequestParameters(HTTPServerRequest& request, std::map<std::string, std::string> & mapParameters) {

    std::string strURI = request.getURI();
    Poco::URI uri(strURI);
    string strPath = uri.getPath();

    Poco::URI::QueryParameters params = uri.getQueryParameters();
    size_t param_count = params.size();

    for (int i = 0; i < param_count; i++) {
        mapParameters.insert(std::pair<string, string>(params.at(i).first, params.at(i).second));
    }

    return true;
}

bool CServiceRequestUtil::GetRequestParameters(HTTPServerRequest & request, string & postBody) {
    
    char *Recv = NULL;
    
    //get post body
    size_t post_len = request.getContentLength();
    std::istream& post_body = request.stream();
    Recv = new char[post_len + 1];
    post_body.read(Recv, post_len);
    Recv[post_len] = '\0';
    
    postBody = Recv;

    //write log
    if (Recv) {
        delete []Recv;
        Recv = NULL;
    }
	return true;
}

std::string CServiceRequestUtil::GetRequestParameter(const std::map<std::string, std::string> & mapParameters, std::string key, std::string defaultValue) {
    std::map<std::string, std::string>::const_iterator l_it = mapParameters.find(key);
    if (l_it == mapParameters.end())
        return defaultValue;
    else
        return l_it->second;
}

bool CServiceRequestUtil::HttpPost(std::string url, std::string postValue, std::string & result) {
    try {
        Poco::URI uri(url);

        HTTPClientSession session(uri.getHost(), uri.getPort());

        HTTPResponse response;
        Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);
        // Send the request. 
        req.setContentType("application/json");
        req.setContentLength(postValue.length());

        std::ostream& ostr = session.sendRequest(req);
        ostr.write(postValue.c_str(), postValue.length());

        std::istream& rs = session.receiveResponse(response);
        if (response.getStatus() == HTTPResponse::HTTP_OK) {
            StreamCopier::copyToString(rs, result);
            return true;
        }
    } catch (Exception &e) {
        std::cout << e.className() << " : " << e.message() << std::endl;
        return false;
    }

    return false;
}

bool CServiceRequestUtil::HttpGet(std::string url, std::string & result) {
	try {
		Poco::URI uri(url);
		HTTPClientSession session(uri.getHost(), uri.getPort());
		
		string path(uri.getPathAndQuery());
		if (path.empty()) path = "/";
		
		HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPRequest::HTTP_1_1);
		//req.setContentType("application/json;charset=GBK");
		req.setContentType("application/json");
		//req.setTransferEncoding("UTF-8");
		session.sendRequest(req);

		HTTPResponse res;
		istream & is = session.receiveResponse(res);
		StreamCopier::copyToString(is, result);
	}
	catch (Exception &e) {
		std::cout << e.className() << " : " << e.message() << std::endl;
		return false;
	}
	
	return true;
}

bool CServiceRequestUtil::HttpPost(std::string url, const std::map<std::string, std::string> & formValues, std::string & result) {

    try {
        URI uri(url);

        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

        Poco::Net::HTTPResponse response;
        Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);

        Poco::Net::HTMLForm form;
        std::map<std::string, std::string>::const_iterator l_it;
        for (l_it = formValues.begin(); l_it != formValues.end(); l_it++) {
            form.add(l_it->first.c_str(), l_it->second.c_str());
        }

        form.prepareSubmit(req);
        std::ostream& ostr = session.sendRequest(req);
        form.write(ostr); //提交post的键值信息 

        std::istream& rs = session.receiveResponse(response);

        if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) //获得请求状态
        {
            StreamCopier::copyToString(rs, result);
            return true;
        }
    } catch (Exception &e) {
        std::cout << e.className() << " : " << e.message() << std::endl;
        return false;
    }

    return false;
}

bool CServiceRequestUtil::HttpPostGetBinary(std::string url, const std::map<std::string, std::string> & formValues, char ** result, size_t * imageSize) {
    try {

        URI uri(url);

        HTTPClientSession session(uri.getHost(), uri.getPort());

        HTTPResponse response;
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);

        Poco::Net::HTMLForm form;
        std::map<std::string, std::string>::const_iterator l_it;
        for (l_it = formValues.begin(); l_it != formValues.end(); l_it++) {
            form.add(l_it->first.c_str(), l_it->second.c_str());
        }

        form.prepareSubmit(request);
        std::ostream& ostr = session.sendRequest(request);
        form.write(ostr);

        std::istream& rs = session.receiveResponse(response);
		
        size_t image_size = 0;
		char * image_data = NULL;
        if (response.getStatus() == HTTPResponse::HTTP_OK) {
            image_size = response.getContentLength();
            if (image_size == Poco::Net::HTTPMessage::UNKNOWN_CONTENT_LENGTH) {
                *imageSize = 0;
                *result = NULL;
            } else {
                //
                image_data = new char[image_size + 1];
                rs.read(image_data, image_size);
                image_data[image_size] = '\0';
                *result = image_data;
                *imageSize = image_size;

            }
        }
    } catch (Exception &e) {
        std::cout << e.className() << " : " << e.message() << std::endl;
        return false;
    }

    return false;
}

bool CServiceRequestUtil::HttpPostSetBinary(std::string url, string filename, const std::map<std::string, std::string> & formValues, std::string & result ) {
    try {
        URI uri(url);
        std::string path(uri.getPathAndQuery());
        if (path.empty()) path = "/";
        HTTPClientSession session(uri.getHost(), uri.getPort());

        HTTPResponse response;
        Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1);
        req.setKeepAlive(true);

        Poco::Net::HTMLForm form("multipart/form-data");
        form.setEncoding(Poco::Net::HTMLForm::ENCODING_MULTIPART);
        form.addPart("file", new FilePartSource(filename));

        std::map<std::string, std::string>::const_iterator l_it;
        for (l_it = formValues.begin(); l_it != formValues.end(); l_it++) {
            form.add(l_it->first.c_str(), l_it->second.c_str());
        }

        // Send the request.
        form.prepareSubmit(req);
        std::ostream& ostr = session.sendRequest(req);
        form.write(ostr); //提交post的键值信息

        if (response.getStatus() == HTTPResponse::HTTP_OK) {
            std::istream& rs = session.receiveResponse(response);

            StreamCopier::copyToString(rs, result);

            Poco::Stopwatch sw;
            Poco::JSON::Parser parser;
            sw.start();
            Poco::Dynamic::Var resultJson = parser.parse(result.c_str());
            sw.stop();

            Poco::JSON::Object::Ptr obj;
            if (resultJson.type() == typeid (Poco::JSON::Object::Ptr))
                obj = resultJson.extract<Poco::JSON::Object::Ptr>();

            std::string code = obj->getValue<std::string>("code");
            if (strcasecmp(code.c_str(), "0") != 0) {
                printf("%d-%s\n", response.getStatus(), result.c_str());
                return false;
            }
            return true;
        } else {
            return false;
        }
    } catch (Exception &e) {
        std::cout << e.className() << " : " << e.message() << std::endl;
        return false;
    }
}

bool CServiceRequestUtil::HttpGetBinary(std::string url, char ** result, size_t * imageSize) {
    try {
        URI uri(url);

        HTTPClientSession session(uri.getHost(), uri.getPort());

        HTTPResponse response;
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.getPath(), Poco::Net::HTTPMessage::HTTP_1_1);
        //伪造浏览器头
        request.set("User-Agent","Mozilla/4.0 (compatible; MSIE 5.5; Windows NT)");
//        request.set("User-Agent","Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.84 Safari/537.36");
//        request.set("Connection","keep-alive");
//        request.set("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8");
        session.sendRequest(request);

        std::istream& rs = session.receiveResponse(response);

        size_t image_size = 0;
        char * image_data = NULL;
        if (response.getStatus() == HTTPResponse::HTTP_OK) {
            image_size = response.getContentLength();
            image_data = new char[image_size + 1];
            rs.read(image_data, image_size);
            image_data[image_size] = '\0';
            *result = image_data;
            *imageSize = image_size;
            string strResult(image_data,image_size);
            if( strResult.find("error") != string::npos ){
                return false;
            }
            return true;
        }
    } catch (Exception &e) {
        std::cout << e.className() << " : " << e.message() << std::endl;
        return false;
    }

    return false;
}
