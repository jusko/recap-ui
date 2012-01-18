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

    private:
        TagCompleter* m_tagCompleter;

    private slots:
        void addTag(const QString&);
};

#endif // TAGLINEEDIT_H
