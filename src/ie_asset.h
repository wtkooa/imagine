#ifndef IE_ASSETS_H
#define IE_ASSETS_H

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
    void setAssetId(unsigned int);
    void setName(std::string);
    unsigned int getAssetId(void);
    std::string getName(void);

    protected:
    unsigned int assetId;
    std::string name;
  };

}

#endif
