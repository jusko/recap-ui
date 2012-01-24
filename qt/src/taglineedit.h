#ifndef TAGLINEEDIT_H
#define TAGLINEEDIT_H

#include <QLineEdit>
class TagCompleter;

//------------------------------------------------------------------------------
// Line edit providing smarter autocompletion hints for tags
//------------------------------------------------------------------------------
class TagLineEdit : public QLineEdit {
    Q_OBJECT

    public:
        TagLineEdit(const QStringList& tags, QWidget *parent = 0);

        const QStringList& tags() const;

        static const char* TagSeparator;

    public slots:
        //---------------------------------------------------------------------
        // Appends a tag to the line edit
        // @pre  The tag is not already present in the line edit
        // @post The tag is appended to the text in the line edit
        // @return true  : if a new tag was added
        //		   false : if the tag was already present
        //---------------------------------------------------------------------
        bool addTag(const QString& tag);

    private:
        TagCompleter* m_tagCompleter;

    private slots:
        void complete(const QString& tag);
};

#endif // TAGLINEEDIT_H
