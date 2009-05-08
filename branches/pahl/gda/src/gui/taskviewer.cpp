/***************************************************************************
                          taskviewer.cpp  -  description
                             -------------------
    begin                : Thu Nov 15 2001
    copyright            : (C) 2001 by Martin pahl
    email                : pahl@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * $Source: /data/cvs/gda/gda/semnetgui/taskviewer.cpp,v $
 * $Revision: 1.6 $
 * $Date: 2004/02/27 10:30:17 $
 * $Author: mueller $
 * $Locker:  $
 */

#include "taskviewer.h"
#include "task.h"
#include "qregexp.h"

TaskViewer::TaskViewer(QWidget * parent)
:QTreeWidget(parent)
{
  connect(&taskTable, SIGNAL(newProcess(ProcessEntry *)),
          this, SLOT(newProcess(ProcessEntry *)));
  connect(&taskTable,
          SIGNAL(stateChanged(QTreeWidgetItem *, int, QString, QString,float)), this,
          SLOT(stateChanged(QTreeWidgetItem *, int, QString, QString,float)));
  connect(&taskTable, SIGNAL(processFinished(QTreeWidgetItem *)), this,
          SLOT(processFinished(QTreeWidgetItem *)));
}

TaskViewer::~TaskViewer()
{
}

/** register a new process */
void
TaskViewer::newProcess(ProcessEntry * p)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(this);
  addTopLevelItem(item);
  taskTable.setGuiPtr((Task::ProcessEntry *) p, item);
}

/** remove taskItem from the TaskViewer */
void
TaskViewer::processFinished(QTreeWidgetItem * taskItem)
{
  if (taskItem)
    delete taskItem;
}

/** Update taskItem */
void TaskViewer::stateChanged(QTreeWidgetItem * taskItem, int pid, QString name,
                              QString cmd, float load)
{
  emit systemLoad(load);
  if (!taskItem)
    return;
  taskItem->setText(0, QString().sprintf("%5d", pid));
  taskItem->setText(1, name);
  taskItem->setText(2, cmd.replace(QRegExp("\n")," "));
}
