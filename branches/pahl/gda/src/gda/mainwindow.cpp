#include "mainwindow.h"
#include "OperatorList"

#include <QFileDialog>
/*!
 * \brief constructor
 */
MainWindow::MainWindow(int,char**,QWidget* parent)
  : QMainWindow(parent)
{
  setupUi(this);

}
	
/*!
 * \brief desctructor
 */
MainWindow::~MainWindow()
{
}

/*!
 * \brief quit the application
 */
void MainWindow::on_actionQuit_triggered()
{
  qApp->quit();
}

/*!
 * \brief load semantic net
 */
void MainWindow::on_actionNetLoad_triggered()
{
#ifdef DEBUGMSG
  qDebug("Sem. Net File Load");
#endif
  //  propertyEditor->itemSelected(0);
  QString fname=QFileDialog::getOpenFileName(this,tr("Load Net"),"","*.net");
  if (fname.isEmpty()) return;
  fileSemanticNet_=fname;
  //  readyToAna(TRUE);  //enable the analysis buttons XXX only for testing
  //  treeEditor->rootNode(0);
  semNet_.read(fileSemanticNet_);
  semNetEditor_->setModel(&semNet_);
  connect(semNetEditor_,SIGNAL(clicked(const QModelIndex &)),
	  &attributeModel_,SLOT(setNode(const QModelIndex &)));
  qDebug("MainWindow::on_actionNetLoad_triggered: connect");
  connect(&attributeModel_,SIGNAL(modelReset()),
	  attributeEditor_,SLOT(expandAll()));
				
  attributeEditor_->setModel(&attributeModel_);
  //  treeEditor->rootNode(semNet.rootNode());
}

