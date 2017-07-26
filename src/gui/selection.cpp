#include "src/gui/selection.h"

namespace GUI {

Selection::Selection() {
}

Selection::Selection(QFileInfo info) {
    _info = info;

    Common::UString nameAsUstr = Common::UString(_info.fileName().toStdString());

    if (_info.isDir()) {
        _source = Source::kSourceDirectory;
    }
    else {
        QString suffix = _info.suffix();
        _fileType = TypeMan.getFileType(nameAsUstr);
        switch (_fileType) {
            case Aurora::kFileTypeZIP:
            case Aurora::kFileTypeERF:
            case Aurora::kFileTypeMOD:
            case Aurora::kFileTypeNWM:
            case Aurora::kFileTypeSAV:
            case Aurora::kFileTypeHAK:
            case Aurora::kFileTypeRIM:
            case Aurora::kFileTypeKEY:
                _source = Source::kSourceArchiveFile;
            default:
                _source = Source::kSourceFile;
        }
    }
    _resourceType = TypeMan.getResourceType(nameAsUstr);
}

Selection::~Selection() {
}

Source Selection::getSource() {
    return _source;
}

Aurora::FileType Selection::getFileType() {
    return _fileType;
}

Aurora::ResourceType Selection::getResourceType() {
    return _resourceType;
}

qint64 Selection::size() {
    return _info.size();
}

QString Selection::fileName() {
    return _info.fileName();
}

} // namespace GUI
