//------------------------------------------------------------------------------
#include "qtserializerwrapper.h"
#include "sqlite3_serializer.h"
//------------------------------------------------------------------------------
#include <string>
#include <vector>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Conversion Helpers
//------------------------------------------------------------------------------

// Converts a standard vector of standard strings into a QStringList.
void wrapTagList(const std::vector<std::string>& inList,
                 QStringList& outList) {

    foreach(const std::string& s, inList) {
        outList.push_back(s.c_str());
    }
}

// Converts a QStringList to a standard vector of standard strings.
void unwrapTagList(const QStringList& inList,
                   std::vector<std::string>& outList) {

    foreach(const QString& s, inList) {
        outList.push_back(s.toStdString());
    }
}

// Converts an Item to a QtItemWrapper (on the heap).
QtItemWrapper* wrapItem(const Item& item) {
    QStringList wrappedTags;
    wrapTagList(item.tags, wrappedTags);

    QtItemWrapper* i = new QtItemWrapper;
    i->id      = item.id;
    i->title   = item.title.c_str();
    i->content = item.content.c_str();
    i->tags    = wrappedTags;
    return i;
}

// Converts a QtItemWrapper to an Item.
Item unwrapItem(const QtItemWrapper& item) {
    std::vector<std::string> unwrappedTags;
    unwrapTagList(item.tags, unwrappedTags);

    Item rv = {
        item.id,
        item.title.toStdString(),
        item.content.toStdString(),
        unwrappedTags
    };
    return rv;
}

//------------------------------------------------------------------------------
// Ctor
// TODO: Error handling
//------------------------------------------------------------------------------
QtSerializerWrapper::QtSerializerWrapper(const char *db_filespec)
    : m_serializer(new SQLite3_Serializer(db_filespec)) {

    readTags();
    read(m_tagsCache);
}

//------------------------------------------------------------------------------
// Dtor
//------------------------------------------------------------------------------
QtSerializerWrapper::~QtSerializerWrapper() {
    if (m_serializer) {
        delete m_serializer;
        m_serializer = 0;
    }
}

//------------------------------------------------------------------------------
const QStringList& QtSerializerWrapper::tags() const {
    return m_tagsCache;
}

//------------------------------------------------------------------------------
const QVector<QtItemWrapper*>& QtSerializerWrapper::items() const {
    return m_itemsCache;
}

//------------------------------------------------------------------------------
// TODO: Error handling
//------------------------------------------------------------------------------
void QtSerializerWrapper::read(const QStringList& tags) {

    m_itemsCache.clear();

    std::vector<std::string> requestTagList;
    std::vector<Item*>       requestItemList;

    unwrapTagList(tags, requestTagList);
    m_serializer->read(requestTagList, requestItemList);

    foreach(Item* item, requestItemList) {
        m_itemsCache.push_back(wrapItem(*item));
        delete item;
        item = 0;
    }
    emit readCompleted(m_itemsCache);
}

//------------------------------------------------------------------------------
// TODO: Error handling
//------------------------------------------------------------------------------
void QtSerializerWrapper::write(const QtItemWrapper &item) {
    m_serializer->write(unwrapItem(item));
}

//------------------------------------------------------------------------------
// TODO: Error handling
//------------------------------------------------------------------------------
void QtSerializerWrapper::readTags() {
    std::vector<std::string> tags;

    m_serializer->tags(tags);

    wrapTagList(tags, m_tagsCache);

    emit tagsUpdated(m_tagsCache);
}
