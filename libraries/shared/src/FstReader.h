//
//  FstReader.h
//  hifi
//
//  Created by Clément Brisset on 3/4/14.
//  Copyright (c) 2014 High Fidelity, Inc. All rights reserved.
//
//

#ifndef __hifi__FstReader__
#define __hifi__FstReader__

class TemporaryDir;
class QHttpMultiPart;

class FstReader : public QObject {
public:
    FstReader(bool isHead);
    ~FstReader();
    
    bool zip();
    bool send();
    
private:
    TemporaryDir* _zipDir;
    int _lodCount;
    int _texturesCount;
    int _totalSize;
    bool _isHead;
    bool _readyToSend;
    
    QHttpMultiPart* _dataMultiPart;
    
    
    bool addTextures(const QFileInfo& texdir);
    bool compressFile(const QString& inFileName, const QString& outFileName);
    bool addPart(const QString& path, const QString& name);
};

#endif /* defined(__hifi__FstReader__) */
