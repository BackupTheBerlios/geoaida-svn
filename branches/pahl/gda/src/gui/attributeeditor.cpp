#include "attributeeditor.h"

AttributeEditor::AttributeEditor(QWidget* parent)
    : QTreeView(parent) 
{
}

void AttributeEditor::reset()
{
  qDebug("AttributeEditor::reset");
  QTreeView::reset();
  expandAll();
}




