#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include "OperatorList"
#include "Exception"

/*!
 * \brief constructor
 */
MainWindow::MainWindow(int,char**,QWidget* parent)
  : QMainWindow(parent)
{
  setupUi(this);
  taskViewerDock_->hide();
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
 try {
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
   attributeEditor_->setItemDelegate(&attributeValueDelegate_);
   //  treeEditor->rootNode(semNet.rootNode());
 }
 catch ( const Exception& err ) {
   QMessageBox::information(this,tr("Info"),err.what());

 }

}
 
/*!
 * \brief load project
 */
void MainWindow::on_actionProjectLoad_triggered()
{
 try {
#ifdef DEBUGMSG
   qDebug("Project File Load");
#endif
   //  propertyEditor->itemSelected(0);
   QString fname=QFileDialog::getOpenFileName(this,tr("Load Project"),"","*.gap");
   if (fname.isEmpty()) return;
   project_.setFilename(fname);
   project_.read();
 }
 catch ( const Exception& err ) {
   QMessageBox::information(this,tr("Info"),err.what());

 }

}
 

void MainWindow::on_actionStart_triggered()
{
  taskViewerDock_->show();
} 
 
