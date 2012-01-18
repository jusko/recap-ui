#include "recallview.h"
#include "taglineedit.h"
#include "tagshortcutdialog.h"
#include "globals.h"

#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QListView>
#include <QDockWidget>
#include <QTextEdit>

//RESUME: Reconsider this main window approah => streamline with a dialog
//------------------------------------------------------------------------------
// ItemModel implementation Ctor
//------------------------------------------------------------------------------
class ItemModel : public QListView {
};

//------------------------------------------------------------------------------
// ItemFinder impelmentation: Ctor
//------------------------------------------------------------------------------
class ItemFinder : public QDockWidget {
    Q_OBJECT

    public:
        ItemFinder(const QStringList& tags,
                   QWidget* parent,
                   Qt::WindowFlags flags = 0)

            : QDockWidget(parent, flags),
              m_itemModel(0),
              m_itemView(0),
              m_tagEdit(0),
              m_tagsBox(0),
              m_tagShortcutButton(0),
              m_tagShortcutDialog(0) {

              QWidget* widget = new QWidget;
              QGridLayout* layout = new QGridLayout(widget);
              setWidget(widget);

              //
              QLabel* tagEditLabel = new QLabel(tr("T&ags"));
              layout->addWidget(tagEditLabel, 0, 0);

              m_tagEdit = new TagLineEdit(tags, this);
              layout->addWidget(m_tagEdit, 0, 1, 1, 2);

              tagEditLabel->setBuddy(m_tagEdit);


              m_tagsBox = new QComboBox;
              m_tagsBox->addItems(tags);
              layout->addWidget(m_tagsBox, 0, 3);


              m_tagShortcutButton = new QPushButton(QIcon(TAG_ICON), "");
              layout->addWidget(m_tagShortcutButton, 0, 4);


              m_itemView = new QListView;
              layout->addWidget(m_itemView, 1, 0, 1, 5);


              m_tagShortcutDialog = new TagShortcutDialog(tags, this);

              connect(m_tagsBox, SIGNAL(activated(QString)),
                      this, SLOT(on_tagsBox_activated(QString)));

              connect(m_tagShortcutButton, SIGNAL(clicked(bool)),
                      this, SLOT(on_tagShortcutButton_clicked(bool)));
        }

    private:
        ItemModel*         m_itemModel;
        QListView*         m_itemView;
        TagLineEdit*       m_tagEdit;
        QComboBox*         m_tagsBox;
        QPushButton*       m_tagShortcutButton;
        TagShortcutDialog* m_tagShortcutDialog;

    private slots:
        //---------------------------------------------------------------------
        // Add a new tag to the line edit when combo box selection is made
        //---------------------------------------------------------------------
        void on_tagsBox_activated(const QString& tag) {
            g_addTag(*m_tagEdit, tag);
        }
        //---------------------------------------------------------------------
        // Displays the tag shortcut manager dialog.
        //---------------------------------------------------------------------
        void on_tagShortcutButton_clicked(bool) {
            m_tagShortcutDialog->show();
        }
};

#include <QFrame>
//------------------------------------------------------------------------------
// NotesEdit Implementation: Ctor
//------------------------------------------------------------------------------
class NotesEdit : public QFrame {
    Q_OBJECT

    public:
        NotesEdit(QWidget* parent = 0,
                  Qt::WindowFlags flags = 0)

            : QFrame(parent, flags),
              m_textEdit(0) {

            QGridLayout* layout = new QGridLayout(this);
            QLabel* l = new QLabel(tr("&Notes"));
            layout->addWidget(l, 0, 2);

            m_textEdit = new QTextEdit;
            layout->addWidget(m_textEdit, 1, 0, 1, 6);

            l->setBuddy(m_textEdit);
        }

    private:
        QTextEdit* m_textEdit;
};

#include "recallview.moc"

//------------------------------------------------------------------------------
// RecallView Implementation: Ctor
//------------------------------------------------------------------------------
RecallView::RecallView(const QStringList& tags, QWidget *parent)
    : QMainWindow(parent),
      m_itemFinder(0),
      m_notesEdit(0) {

      setWindowTitle(tr("Recap - Recall Mode"));

      m_itemFinder = new ItemFinder(tags, this);
      addDockWidget(Qt::LeftDockWidgetArea, m_itemFinder);

      m_notesEdit = new NotesEdit;
      setCentralWidget(m_notesEdit);
}
