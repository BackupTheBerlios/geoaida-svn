#ifndef AttributeEditor_h
#define AttributeEditor_h
#include <QTreeView>

class AttributeEditor : public QTreeView
{
Q_OBJECT
 public:
  AttributeEditor(QWidget* parent = 0);
    void reset();

};


#endif
