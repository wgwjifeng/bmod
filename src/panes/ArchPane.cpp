#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>

#include "../Util.h"
#include "ArchPane.h"

ArchPane::ArchPane(FormatType type, BinaryObjectPtr obj)
  : Pane(Kind::Arch), type{type}, obj{obj}
{
  createLayout();
}

void ArchPane::createLayout() {
  auto *gridLayout = new QGridLayout;
  gridLayout->setContentsMargins(0, 0, 0, 0);

  gridLayout->addWidget(new QLabel(tr("Format type:")), 0, 0);
  gridLayout->addWidget(new QLabel(Util::formatTypeString(type)), 0, 1);

  gridLayout->addWidget(new QLabel(tr("System bits:")), 1, 0);
  gridLayout->addWidget(new QLabel(QString::number(obj->getSystemBits())),
                        1, 1);

  gridLayout->addWidget(new QLabel(tr("Little endian:")), 2, 0);
  gridLayout->addWidget(new QLabel(obj->isLittleEndian() ?
                                   tr("Yes") : tr("No")), 2, 1);

  gridLayout->addWidget(new QLabel(tr("CPU type:")), 3, 0);
  gridLayout->addWidget(new QLabel(Util::cpuTypeString(obj->getCpuType())),
                        3, 1);

  gridLayout->addWidget(new QLabel(tr("CPU sub type:")), 4, 0);
  gridLayout->addWidget(new QLabel(Util::cpuTypeString(obj->getCpuSubType())),
                        4, 1);

  gridLayout->addWidget(new QLabel(tr("File type:")), 5, 0);
  gridLayout->addWidget(new QLabel(Util::fileTypeString(obj->getFileType())),
                        5, 1);

  gridLayout->addWidget(new QLabel(tr("Sections:")), 6, 0);
  gridLayout->addWidget(new QLabel(QString::number(obj->getSections().size())),
                        6, 1);

  auto *w = new QWidget;
  w->setMaximumWidth(300);
  w->setLayout(gridLayout);
  
  auto *layout = new QVBoxLayout;
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(w);
  layout->addStretch();

  setLayout(layout);
}
