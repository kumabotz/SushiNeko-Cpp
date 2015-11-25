//
//  PieceReader.hpp
//  SushiNeko
//
//  Created by Tery Lim on 25/11/15.
//
//

#ifndef PieceReader_hpp
#define PieceReader_hpp

#include "cocos2d.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"

class PieceReader : public cocostudio::NodeReader
{
public:
    static PieceReader* getInstance();
    static void purge();
    cocos2d::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions);
};

#endif /* PieceReader_hpp */
