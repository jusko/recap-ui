//------------------------------------------------------------------------------
// Glue binding the Qt UI code to the standard C++ serialization components.
//------------------------------------------------------------------------------
#ifndef QTSERIALIZERWRAPPER_H
#define QTSERIALIZERWRAPPER_H

#include <QStringList>
#include <QVector>
class Serializer;

//------------------------------------------------------------------------------
// Wraps Items to use QStrings for model/view compatibility.
//------------------------------------------------------------------------------
struct QtItemWrapper {
    int			id;
    QString 	title;
    QString 	content;
    QStringList tags;
};

//------------------------------------------------------------------------------
// Converts data to and from QtItemWrappers and provides a signal/slot interface
// to the read/write base class functions.
//------------------------------------------------------------------------------
class QtSerializerWrapper : public QObject {
    Q_OBJECT

    public:
        QtSerializerWrapper(const char* db_filespec);
        ~QtSerializerWrapper();

        //---------------------------------------------------------------------
        // Returns the currently cached list of tags.
        //---------------------------------------------------------------------
        const QStringList& tags() const;

        //---------------------------------------------------------------------
        // Returns the currently cached items.
        //---------------------------------------------------------------------
        const QVector<QtItemWrapper*>& items() const;

    public slots:
        //---------------------------------------------------------------------
        // Request to retrieve all Items associated with tags.
        // Notifies via readCompleted signal.
        //---------------------------------------------------------------------
        void read(const QStringList& tags);

        //---------------------------------------------------------------------
        // Request to write an Item to disk.
        //---------------------------------------------------------------------
        void write(const QtItemWrapper& item);

        //---------------------------------------------------------------------
        // Request to update the tag cache.
        // Notifies via tagsUpdated signal.
        //---------------------------------------------------------------------
        void readTags();

    signals:
        //---------------------------------------------------------------------
        // Notifies that Items have been read.
        //---------------------------------------------------------------------
        void readCompleted(const QVector<QtItemWrapper*>&);

        //---------------------------------------------------------------------
        // Notifies that tag cache has been updated.
        //---------------------------------------------------------------------
        void tagsUpdated(const QStringList&);

    private:
        Serializer* 			m_serializer;
        QStringList 			m_tagsCache;
        QVector<QtItemWrapper*> m_itemsCache;
};
#endif // QTSERIALIZERWRAPPER_H
