#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QApplication>
#include <QProgressDialog>
#include <QStyledItemDelegate>

#include "Util.h"
#include "MachineCodeWidget.h"

namespace {
  class ItemDelegate : public QStyledItemDelegate {
  public:
    ItemDelegate(QTreeWidget *tree) : tree{tree} { }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const {
      int col = index.column();
      if (col != 1 && col != 2) {
        return nullptr;
      }

      QString text = index.data().toString().trimmed();
      if (text.isEmpty()) {
        return nullptr;
      }

      QString mask;
      int blocks = text.split(" ").size();
      for (int i = 0; i < blocks; i++) {
        mask += "HH ";
      }
      if (mask.endsWith(" ")) {
        mask.chop(1);
      }

      auto *edit = new QLineEdit(parent);
      edit->setInputMask(mask);
      edit->setText(text);
      return edit;
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const {
      auto *edit = qobject_cast<QLineEdit*>(editor);
      if (edit) {
        model->setData(index, edit->text().toUpper());
        auto *item = tree->topLevelItem(index.row());
        if (item) {
          int col = index.column();
          auto font = item->font(col);
          font.setBold(true);
          item->setFont(col, font);
          item->setForeground(col, Qt::red);
        }
      }
    }

  private:
    QTreeWidget *tree;
  };
}

MachineCodeWidget::MachineCodeWidget(BinaryObjectPtr obj, SectionPtr sec)
  : obj{obj}, sec{sec}, shown{false}
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  createLayout();
}

void MachineCodeWidget::showEvent(QShowEvent *event) {
  QWidget::showEvent(event);
  if (!shown) {
    shown = true;
    setup();
  }
}

void MachineCodeWidget::createLayout() {
  label = new QLabel;

  treeWidget = new QTreeWidget;
  treeWidget->setHeaderLabels(QStringList{tr("Address"), tr("Data Low"),
        tr("Data High"), tr("ASCII")});
  treeWidget->setColumnWidth(0, obj->getSystemBits() == 64 ? 110 : 70);
  treeWidget->setColumnWidth(1, 200);
  treeWidget->setColumnWidth(2, 200);
  treeWidget->setColumnWidth(3, 110);
  treeWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
  treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  treeWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
  treeWidget->setItemDelegate(new ItemDelegate(treeWidget));

  // Set fixed-width font.
  treeWidget->setFont(QFont("Courier"));

  auto *layout = new QVBoxLayout;
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(label);
  layout->addWidget(treeWidget);
  
  setLayout(layout);
}

void MachineCodeWidget::setup() {
  quint64 addr = sec->getAddress();
  const QByteArray &data = sec->getData();
  int len = data.size(), rows = len / 16;

  if (len == 0) {
    label->setText(tr("Defined but empty."));
    treeWidget->hide();
    return;
  }

  int padSize = obj->getSystemBits() / 8;
  label->setText(tr("Section size: %1, address %2 to %3")
                 .arg(Util::formatSize(len))
                 .arg(Util::padString(QString::number(addr, 16).toUpper(),
                                      padSize))
                 .arg(Util::padString(QString::number(addr + len, 16).toUpper(),
                                      padSize)));
  if (len % 16 > 0) rows++;

  QProgressDialog progDiag(this);
  progDiag.setLabelText(tr("Processing data.."));
  progDiag.setCancelButton(nullptr);
  progDiag.setRange(0, 100);
  progDiag.show();
  qApp->processEvents();

  for (int row = 0, byte = 0; row < rows; row++) {
    auto *item = new QTreeWidgetItem;
    item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setText(0, Util::padString(QString::number(addr, 16).toUpper(),
                                     obj->getSystemBits() / 8));

    QString code, ascii;
    for (int cur = 0; cur < 16 && byte < len; cur++, byte++) {
      QString hex =
        Util::padString(QString::number((unsigned char) data[byte], 16), 2);
      code += hex + " ";

      int ic = data[byte];
      ascii += (ic >= 32 && ic <= 126 ? (char) ic : '.');
    }
    if (code.endsWith(" ")) {
      code.chop(1);
    }
    code = code.toUpper();
    item->setText(1, code.mid(0, 8 * 3));
    item->setText(2, code.mid(8 * 3));
    item->setText(3, ascii);

    treeWidget->addTopLevelItem(item);
    addr += 16;

    static int lastPerc{0};
    int perc = (float) row / (float) rows * 100.0;
    if (perc > lastPerc || perc == 100) {
      lastPerc = perc;
      progDiag.setValue(perc);
      progDiag.setLabelText(tr("Processing data.. %1% (%2 of %3)")
                            .arg(perc)
                            .arg(Util::formatSize(byte))
                            .arg(Util::formatSize(len)));
      qApp->processEvents();
    }
  }
}