#ifndef SELECTION_H
#define SELECTION_H

#include <QFileInfo>

#include "src/aurora/util.h"

enum Source {
    kSourceDirectory= 0,
    kSourceFile = 1,
    kSourceArchive = 2,
    kSourceArchiveFile = 3
};

namespace GUI {

class Selection {
private:
    QFileInfo _info;
    Source _source;
    Aurora::FileType _fileType;
    Aurora::ResourceType _resourceType;
public:
    Selection();
    Selection(QFileInfo info);
    ~Selection();
    Source getSource();
    Aurora::FileType getFileType();
    Aurora::ResourceType getResourceType();
    qint64 size();
    QString fileName();
};

} // namespace GUI

#endif // SELECTION_H
