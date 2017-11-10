#ifndef IE_ASSET_H
#define IE_ASSET_H

//___|"ie_asset.h"|_____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <string>

namespace ie
{

  class Asset
  {

    public:
    Asset();

    void setAssetId(unsigned int);
    void setName(std::string);
    void setFilename(std::string);
    void setFilepath(std::string);
    unsigned int getAssetId(void);
    std::string getName(void);
    std::string getFilename(void);
    std::string getFilepath(void);

    virtual void reset(void);

    protected:
    unsigned int assetId;
    std::string name;
    std::string filename; 
    std::string filepath;

  };

  

}

#endif
