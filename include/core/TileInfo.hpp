//
//  TileInfo.hpp
//  getTiles
//
//  Created on 2018/1/5.
//
//

#ifndef TileInfo_hpp
#define TileInfo_hpp

#include <string>
#include <sstream>

using std::string;
using std::stringstream;

class TileInfor{
public:
    TileInfor(){}
    TileInfor(int x, int y, int z) : x_(x), y_(y), z_(z){
        stringstream ss;
        ss<<z<<"/"<<x/10<<"/"<<y/10<<"/"<<x<<"_"<<y<<".png";
        key_ = ss.str();
    }
    //
    int getX(){
        return x_;
    }
    //
    int getY(){
        return y_;
    }
    //
    int getZ(){
        return z_;
    }
    //
    string getKey(){
        return key_;
    }
    //
    bool endFlag = false;
private:
    int x_;
    int y_;
    int z_;
    string key_;
};


#endif /* TileInfo_hpp */
