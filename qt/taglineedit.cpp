#include "taglineedit.h"

#include <QStringListModel>
#include <QCompleter>

// TODO: Consider configuring multiple separators
const char SEP = ',';

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

    public slots:

        // Removes the tags in the list from the completer model and
        // readjusts the current completion prefix.
        void setTagCompletion(const QString& text) {

            // TODO: - Removing tags from the model works only after the second ',' => fix
            //		 - Modifiy to rebuild model tags when text is deleted => currently once they're removed they never come back
            if (text.contains(SEP)) {

                QString prefix = text.section(SEP, -1).simplified();
                if (!prefix.simplified().isEmpty()) {

                    QStringList tags = text.split(SEP, QString::SkipEmptyParts,
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
#include "tagcontrol.moc"


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
            this, SLOT(addTag(const QString&)));

}

//------------------------------------------------------------------------------
// Adds a tag each time an autocompletion option is selected
//------------------------------------------------------------------------------
void TagLineEdit::addTag(const QString& tag) {
    QString currentText(text());
    int i = currentText.lastIndexOf(SEP);
    if (i > -1) {
        currentText.truncate(i);
        setText(QString("%1, %2").arg(currentText).arg(tag));
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
   tags = text().split(SEP, QString::SkipEmptyParts);

   int i = 0;
   foreach(const QString& tag, tags) {
        tags[i++] = tag.trimmed();
   }
}
