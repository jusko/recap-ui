#ifndef TAGLINEEDIT_H
#define TAGLINEEDIT_H
//------------------------------------------------------------------------------
#include <QLineEdit>
//------------------------------------------------------------------------------
class TagCompleter;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Line edit providing smarter autocompletion for tags.
//------------------------------------------------------------------------------
class TagLineEdit : public QLineEdit {
    Q_OBJECT

    public:
        //---------------------------------------------------------------------
        // Creates a standard QLineEdit with autocompletion tweaked to
        // adapt itself when a new tag is being entered.
        // @param tags
        //		  The set of tags to use as a wordlist for the autocompleter
        //---------------------------------------------------------------------
        TagLineEdit(const QStringList& tags, QWidget *parent = 0);


        //---------------------------------------------------------------------
        // Query whether text is entered in the line edit.
        // @return true if no text is entered.
        // @return false if text is present.
        //---------------------------------------------------------------------
        bool isEmpty() const;

        //---------------------------------------------------------------------
        // Returns a list of the tags typed into the line edit.
        // @pre  The line edit is not empty
        // @post A list of tags is produced by splitting the text on
        //		 TagSeparators.
        //---------------------------------------------------------------------
        const QStringList& tags() const;

        //---------------------------------------------------------------------
        // Tags are space separated for now (this might become more
        // configurable).
        //---------------------------------------------------------------------
        static const char* TagSeparator;

    public slots:
        //---------------------------------------------------------------------
        // Appends a tag to the line edit
        // @pre    The tag is not already present in the line edit
        // @post   The tag is appended to the text in the line edit separated
        //		   with a separator
        // @return true
        //		   If a new tag was added
        // @return false
        //		   If the tag was already present
        //---------------------------------------------------------------------
        bool addTag(const QString& tag);

    private:
        TagCompleter* m_tagCompleter;

    private slots:
        void complete(const QString& tag);
};
#endif // TAGLINEEDIT_H
