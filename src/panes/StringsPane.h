#ifndef BMOD_STRINGS_PANE_H
#define BMOD_STRINGS_PANE_H

#include "Pane.h"
#include "../Section.h"
#include "../BinaryObject.h"

class QTreeWidget;

class StringsPane : public Pane {
public:
  StringsPane(BinaryObjectPtr obj, SectionPtr sec);

protected:
  void showEvent(QShowEvent *event);

private:
  void createLayout();
  void setup();

  BinaryObjectPtr obj;
  SectionPtr sec;

  bool shown;
  QTreeWidget *treeWidget;
};

#endif // BMOD_STRINGS_PANE_H