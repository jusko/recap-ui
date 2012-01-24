#include "taglineedit.h"

#include <QStringListModel>
#include <QCompleter>

#include <iostream>
using namespace std;
//------------------------------------------------------------------------------
// Custom completer for more intelligent autocompletion suggestions.
//------------------------------------------------------------------------------
class TagCompleter : public QCompleter {
    Q_OBJECT

    public:
        TagCompleter(const QStringList& tags, QObject* parent = 0)
            : QCompleter(tags, parent) {}

        // TODO: Implement wordlist update slot -> connect serializer signal to
        //       update it

    public slots:

        // Removes the tags in the list from the completer model and
        // readjusts the current completion prefix.
        void setTagCompletion(const QString& text) {

            // TODO: - Removing tags from the model works only after the second ',' => fix
            //		 - Modifiy to rebuild model tags when text is deleted => currently once they're removed they never come back
            if (text.contains(TagLineEdit::TagSeparator)) {

                QString prefix = text.section(TagLineEdit::TagSeparator, -1).simplified();
                if (!prefix.simplified().isEmpty()) {

                    QStringList tags = text.split(TagLineEdit::TagSeparator, QString::SkipEmptyParts,
                                                       Qt::CaseInsensitive);
                    tags.removeLast();

                    QStringList model =
                        qobject_cast<QStringListModel*>(this->model())->stringList();

                    if (tags.size() > 1) {
                        int i = 0;
                        foreach(const QString& tag, tags) {
                            tags[i++] =  tag.trimmed();
                        }
                        foreach(const QString& tag, tags) {
                            if (model.contains(tag)) {
                                model.removeAll(tag);
                            }
                        }
                        this->setModel(new QStringListModel(model));
                    }
                    if (!model.contains(prefix)) {
                        setCompletionPrefix(prefix);
                        complete();
                    }
                }
            }
            else {
                setCompletionPrefix(text);
                complete();
            }
        }
};
#include "taglineedit.moc"


//------------------------------------------------------------------------------
// Class static data
//------------------------------------------------------------------------------
const char* TagLineEdit::TagSeparator = " ";

//------------------------------------------------------------------------------
// Ctor
//------------------------------------------------------------------------------
TagLineEdit::TagLineEdit(const QStringList &tags, QWidget *parent)
    : QLineEdit(parent) {

    m_tagCompleter = new TagCompleter(tags);
    m_tagCompleter->setWidget(this);

    connect(this, SIGNAL(textEdited(const QString&)),
            m_tagCompleter, SLOT(setTagCompletion(const QString&)));

    connect(m_tagCompleter, SIGNAL(activated(const QString&)),
            this, SLOT(complete(const QString&)));

}

//------------------------------------------------------------------------------
bool TagLineEdit::addTag(const QString& tag) {
    QString currTags = text();
    if (!currTags.contains(tag)) {
        if (currTags.isEmpty()) {
            setText(tag);
        }
        else {
            setText(currTags + TagSeparator + tag);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
// Complete the currently entered text (if any) with the signalled tag.
//------------------------------------------------------------------------------
void TagLineEdit::complete(const QString &tag) {
    QString currText = text();
    if (currText.contains(TagSeparator)) {
        setText(currText.left(currText.lastIndexOf(TagSeparator) + 1) + tag);
    }
    else {
        setText(tag);
    }
}

//------------------------------------------------------------------------------
// Tokenize the currently entered text into tags.
//------------------------------------------------------------------------------
const QStringList& TagLineEdit::tags() const {
    static QStringList tags;
    tags = text().split(TagSeparator, QString::SkipEmptyParts);

    int i = 0;
    foreach(const QString& tag, tags) {
         tags[i++] = tag.trimmed();
    }
    return tags;
}
