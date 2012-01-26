//------------------------------------------------------------------------------
#ifndef QTSERIALIZERWRAPPER_H
#define QTSERIALIZERWRAPPER_H
//------------------------------------------------------------------------------
#include <QStringList>
#include <QVector>
//------------------------------------------------------------------------------
class Serializer;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Wrapper for the Item data type (see recap-core/serializer.h) for use with
// Qt's model/view architecture.
//------------------------------------------------------------------------------
struct QtItemWrapper {
    int			id;
    QString 	title;
    QString 	content;
    QStringList tags;
};

//------------------------------------------------------------------------------
// Interface to convert data to and from QtItemWrappers and provide signal/slot
// based access to the serializers read/write functions. In essence, this class
// acts as the glue which binds the Qt UI code to the standard C++ serialization
// components in recap-core.
//------------------------------------------------------------------------------
class QtSerializerWrapper : public QObject {
    Q_OBJECT

    public:
        //----------------------------------------------------------------------
        // Initialises a DB connection and populates data caches.
        // @param db_filespec
        //		  The full path and filespec of the database to connect to.
        // @pre   The path in the filespec exists
        // @post  The database is created if it did not exist, and a connection.
        //		  to it is established.
        //		  All item tags are cached and can be queried with readTags()
        //		  Currently all items are cached too (this will be changed in
        //		  due course). The item cache is accessible via the items()
        //		  function.
        //----------------------------------------------------------------------
        QtSerializerWrapper(const char* db_filespec);

        //---------------------------------------------------------------------
        // Returns the currently cached list of tags.
        // @pre  A call has been made to the readTags() slot.
        // @post The cache is synced with the most recent tagset.
        //---------------------------------------------------------------------
        const QStringList& tags() const;

        //---------------------------------------------------------------------
        // Returns the currently cached items.
        // @pre  A call has been made to the read() slot.
        // @post The cache is synced with the most recent itemset.
        //---------------------------------------------------------------------
        const QVector<QtItemWrapper*>& items() const;

        ~QtSerializerWrapper();

    // TODO: Implement error handling on all of these (currently errors
    //		 propogate to the RecapApp class.
    public slots:
        //---------------------------------------------------------------------
        // Sends a request to retrieve all items associated with a set of tags.
        // @param tags
        //		  The tags associated with the items to be retreived.
        // @post  Notifies the operation's completion via readCompleted()
        //		  signal.
        //---------------------------------------------------------------------
        void read(const QStringList& tags);

        //---------------------------------------------------------------------
        // Writes an item to disk.
        // @param item
        //		  The item to be written.
        // @pre   If the item is new, its ID must be 0. If an existing item
        //		  is to be updated, its ID is assumed to be valid.
        // @post  The item is written to disk.
        //---------------------------------------------------------------------
        void write(const QtItemWrapper& item);

        //---------------------------------------------------------------------
        // Trashes the given item.
        // @pre  The item parameter has been written previously.
        // @post The item is trashed and will no longer be queryable.
        //---------------------------------------------------------------------
        void trash(const QtItemWrapper& item);

        //---------------------------------------------------------------------
        // Sends a request to update the tag cache.
        // @post Notifies via tagsUpdated signal.
        //---------------------------------------------------------------------
        void readTags();

    signals:
        //---------------------------------------------------------------------
        // Notifies when a request to read a set of items is complete.
        // @param items
        //        The retreived items.
        // @note  The items are allocated on the heap and must be cleaned up
        //		  by the client.
        //---------------------------------------------------------------------
        void readCompleted(const QVector<QtItemWrapper*>& items);

        //---------------------------------------------------------------------
        // Notifies when the tag cache has been updated.
        // @param tags
        //		  The most recent tagset available in the datastore.
        //---------------------------------------------------------------------
        void tagsUpdated(const QStringList&);

    private:
        Serializer* 			m_serializer;
        QStringList 			m_tagsCache;
        QVector<QtItemWrapper*> m_itemsCache;
};
#endif // QTSERIALIZERWRAPPER_H
