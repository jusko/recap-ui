#include "recallview.h"
#include "taglineedit.h"
#include "tagshortcutdialog.h"
#include "globals.h"

#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QListView>
#include <QAbstractListModel>
#include <QDockWidget>
#include <QTextEdit>
#include <QSplitter>
#include <QVariant>

//------------------------------------------------------------------------------
// RecallView Implementation: Ctor
//------------------------------------------------------------------------------
RecallView::RecallView(const QStringList& tags,
                       ItemModel& model,
                       QWidget *parent)
    : QDialog(parent),
      m_itemModel(&model),
      m_itemView(0),
      m_tagEdit(0),
      m_tagsBox(0),
      m_tagShortcutButton(0),
      m_tagShortcutDialog(0) {

      // TODO: Refactor
      setWindowTitle(tr("Recap - Recall Mode"));
      setGeometry(0, 0, 800, 600);

      QGridLayout* layout = new QGridLayout(this);
      QSplitter* splitter = new QSplitter;
      layout->addWidget(splitter, 0, 0);

      QFrame* 	   l_frame  = new QFrame;
      QGridLayout* l_layout = new QGridLayout(l_frame);
      splitter->addWidget(l_frame);
      l_frame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

      QFrame*      r_frame  = new QFrame;
      QGridLayout* r_layout = new QGridLayout(r_frame);
      splitter->addWidget(r_frame);
      r_frame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

      QLabel* tagEditLabel = new QLabel(tr("T&ags"));
      l_layout->addWidget(tagEditLabel, 0, 0);

      m_tagEdit = new TagLineEdit(tags, this);
      l_layout->addWidget(m_tagEdit, 0, 1);

      tagEditLabel->setBuddy(m_tagEdit);

      m_tagsBox = new QComboBox;
      m_tagsBox->addItems(tags);
      l_layout->addWidget(m_tagsBox, 0, 3);

      m_tagShortcutButton = new QPushButton(QIcon(TAG_ICON), "");
      l_layout->addWidget(m_tagShortcutButton, 0, 4);

      m_itemView = new QListView;
      l_layout->addWidget(m_itemView, 1, 0, 1, 5);

      QLabel* notesLabel = new QLabel(tr("&Notes"));
      r_layout->addWidget(notesLabel, 0, 0);

      m_contentEdit = new QTextEdit;
      r_layout->addWidget(m_contentEdit, 1, 0);

      notesLabel->setBuddy(m_contentEdit);

      m_tagShortcutDialog = new TagShortcutDialog(tags, this);

      connect(m_tagsBox, SIGNAL(activated(QString)),
              this, SLOT(on_tagsBox_activated(QString)));

      connect(m_tagShortcutButton, SIGNAL(clicked(bool)),
              this, SLOT(on_tagShortcutButton_clicked(bool)));
}

//---------------------------------------------------------------------
// Add a new tag to the line edit when combo box selection is made
//---------------------------------------------------------------------
void RecallView::on_tagsBox_activated(const QString& tag) {
    g_addTag(*m_tagEdit, tag);
}
//---------------------------------------------------------------------
// Displays the tag shortcut manager dialog.
//---------------------------------------------------------------------
void RecallView::on_tagShortcutButton_clicked(bool) {
    m_tagShortcutDialog->show();
}
