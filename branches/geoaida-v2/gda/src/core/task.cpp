// GAD - Library 
// Copyright (C) 2000 Martin Pahl, Universitšt Hannover
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

/*
 * $Source: /data/cvs/gda/gda/core/task.cpp,v $
 * $Revision: 1.20 $
 * $Date: 2004/02/27 10:25:48 $
 * $Author: mueller $
 * $Locker:  $
 */

#include "task.h"
#include <sys/types.h>
#ifndef WIN32
#include <sys/wait.h>
#endif
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <QTimer>
#include <QProcess>
#include <QApplication>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <process.h>
#include <windows.h>
#include <string>
#include <tchar.h>
#include <stdio.h>
#undef WIN32_LEAN_AND_MEAN
#include <qmessagebox.h>
#endif

//#define DEBUG_MSG

Task::Task(unsigned int maxJobs)
{
  timer_ = new QTimer(this);
  connect(timer_, SIGNAL(timeout()), this, SLOT(check()));
  jid_ = 0;
#ifdef WIN32
  process_count= -1;
  if (timer_ == 0){
    //    cout << "Out of memory..20";
    exit(1);
  }
#endif
  maxJobs_ = maxJobs;
  timerRunning_ = false;
  random_=true;
}

Task::~Task()
{
}

int Task::execNext()
{
  if (jobQueue_.isEmpty())
    return 0;
  if (random_) {
#ifdef WIN32
	srand( (unsigned)time( NULL ) );
	int count=rand()%jobQueue_.count();
#else
    int count=random()%jobQueue_.count();
#endif
    qDebug("Task::execNext(): Using %d. entry",count);
    for (int i=0; i<count; i++) {
      ProcessEntry* p=jobQueue_.dequeue();
      jobQueue_.enqueue(p);
    }
  }
  ProcessEntry *p = jobQueue_.dequeue();
  Q_ASSERT(p);
  QString command = p->cmd_;
#ifdef DEBUG_MSG
  qDebug("Comando na fila::  %s\n", p->cmd_.toLatin1().constData());
#endif
  int pid = start(p->cmd_);
  p->pid_ = pid;
  emit stateChanged(p->guiPtr_, p->pid_, p->node_->name(), p->cmd_,
                    systemLoad_);
  job_.insert(p->jid_, p);
  if (pid <= 0)
    failedJobs_.enqueue(p);
  else
    //#ifdef WIN32
    //    process_[++process_count]=*p;
    //#else
  process_.insert(pid,p);
  //#endif
#ifdef DEBUG_MSG
  qDebug("Task::execNext(%d): %s\n", pid, command.toLatin1().constData());
#endif
  if (!timerRunning_) {
    timer_->start(2000);
    timerRunning_ = true;
  }
  return pid;
}

#ifdef WIN32
int Task::start(QString command)
{	int pid;
	if (command.isEmpty())
    return 0;
#ifdef DEBUGMSG
	qDebug("Task::start: %s\n",command.latin1());
#endif

	//alterado por Dario
	  //pid = fork();

	QStringList auxiliar;
//
//	command.replace('\'','\"');
//	//modificado por Dario
//
	command = command.simplified();
	QStringList command_list;

	for (int i = 0; i< command.length(); )
	{
		QString str_auxiliar = "";
		if (command[i]=='\"')
		{
			i++;
			while ((command[i]!='\"')&&(i<command.length()))
			{
				str_auxiliar = str_auxiliar + command[i];
				i++;
			}
			i++;
			command_list << str_auxiliar;
		}
		else if (command[i]==' ')
		{
			i++;
//			while ((command[i]!=' ')&&(i<command.length()))
//			{
//				str_auxiliar = str_auxiliar + command[i];
//				i++;
//			}
//			i++;
//			command_list << str_auxiliar;
		}
		else
		{
			while ((command[i]!=' ')&&(i<command.length()))
			{
				str_auxiliar = str_auxiliar + command[i];
				i++;
			}
			//i++;
			command_list << str_auxiliar;
		}
	}

	//auxiliar = QStringList::split( " ", command );
	QString command_aux = (command_list.at(0));
	command_list.removeAt(0);
	command_list.removeAt(command_list.count()-1);
	//cout << "numero de argumentos " << command_list.count() << endl;
	for (int i= 0; i<command_list.count();i++)
	{
//		if(command_list.at(i)=="")
//		{
//			command_aux = command_aux + " \"\"";
//		}
//		else
//		{
//			command_aux = command_aux + " " + command_list.at(i).latin1();
//		}

		//cout << "argumento " << i << ": " << command_list.at(i).latin1()  <<  endl;
	}

#ifdef DEBUGMSG
	//cout << "finalmente o comando!! " << command.latin1()  << " # primeiro: " << command_aux.latin1() << endl;
#endif


	        QProcess *myProcess = new QProcess(this);
	        //myProcess->setStandardOutputFile(QString("ga_bu.txt"));
	        //myProcess->start(command_aux);
	        myProcess->start(command_aux,command_list);

			pid = (long)myProcess;
			
	  if (pid == -1)
	  { QString Fehler= strerror(errno);
	    Fehler.append('\n');
	    Fehler.append(command);
		#ifdef WIN32
	    	QMessageBox::information(0,"Task::start:Fehler",Fehler,1);
		#endif
	  }
	  if (pid == 0) {
	    //system(command.latin1());
	    exit(127);
	  }

	//pid = system(command.latin1());
#ifdef DEBUG_MSG
	  //cout << "comando executado: " << command.latin1() << endl;
#endif
//if (pid == -1)
//  { QString Fehler= strerror(errno);
//    Fehler.append('\n');
//    Fehler.append(command);
//#ifdef WIN32
//    QMessageBox::information(0,"Task::start:Fehler",Fehler,1);
//#endif
//  }
  //return pid;
  return (long) myProcess;

}
#else
int Task::start(QString command)
{
  int pid;
  extern char **environ;

  if (command.isEmpty())
    return 0;
#ifdef DEBUG_MSG
  qDebug("Task::start: %s\n", command.toLatin1().constData());
#endif
  pid = fork();
  if (pid == -1)
    return -1;
  if (pid == 0) {
    char *  argv[4];
    argv[0] = qstrdup("sh");
    argv[1] = qstrdup("-c");
    argv[2] = qstrdup(command.toLatin1().constData());
    argv[3] = 0;
    execve("/bin/sh", argv, environ);
    exit(127);
  }
  return pid;
}
#endif //TASK::start

#ifdef WIN32
void Task::kill(int pid)
{
	TerminateProcess((HANDLE)pid,127);
}
#else
void Task::kill(int pid)
{
  ::kill(pid,SIGTERM);
}
#endif

/** push a command in the job queue. job id is returned */
int Task::queue(QString command, INode * obj)
{
  ProcessEntry *p = new ProcessEntry;
#ifdef WIN32
    if (p == 0){
      //cout << "Out of memory..20";
      exit(1);
    }
#endif
#ifdef WIN32
  command.replace('/','\\');
#endif
#ifdef DEBUG_MSG
  qDebug("Comando antes de criar processo:  %s\n", command.toLatin1().constData());
#endif
  p->cmd_ = command;
  p->jid_ = ++jid_;
  p->pid_ = 0;
  p->node_ = obj;
  p->guiPtr_ = 0;
  p->status_ = 0;
  jobQueue_.enqueue(p);
  emit newProcess(p);
  emit stateChanged(p->guiPtr_, p->pid_, p->node_->name(), p->cmd_,
                    systemLoad_);
#ifdef DEBUG_MSG
  //cout << "Mandou o sinal " << endl;
#endif
/* alte Version
#ifdef WIN32
	if (!maxJobs_ || process_count<maxJobs_)
	{	execNext();
	}
#else
	if (!maxJobs_ || process_.count()<maxJobs_) {
		execNext();
	}
#endif
*/
  if (!systemLoad())
    execNext();

  return jid_;
}

void Task::add(int pid, INode * obj)
{
#if 0
  process_.insert(pid, obj);
  if (process_.count() == 1) {
    timer_->start(200);
  }
#endif
}

void Task::remove(INode * obj)
{
  /*! */
  qDebug("Task::remove not implemented\n");
}

void Task::check()
{
#ifdef DEBUG_MSG
    qDebug("Task::check(%d)\n",process_.count());
#endif
  int status;
  while (!failedJobs_.isEmpty()) {
    ProcessEntry *p = failedJobs_.dequeue();
    INode *iNode = p->node_;
    job_.remove(p->jid_);
    if (iNode)
      iNode->taskFinished(p->pid_, p->status_, p->cmd_);
    delete p;
  }
#ifdef WIN32
  QHash <int,ProcessEntry*>::const_iterator it = job_.constBegin();
  unsigned long stWord;
  //LPDWORD stWord;
  int pid=0;
  int flag=0;
  status=-1;
  //while(1)
  //{
#ifdef DEBUG_MSG
//cout << "contagem de jobs : " << it.count() << endl;
#endif
  if (job_.count()>0)
  {
	  it = job_.constBegin();
  }
  else
  {
	  return;
  }
  while (it != job_.constEnd()) {
	  //cout << "pid no check " << it.current()->pid_ << endl;
	  //cout << "status no check " << ((QProcess*)it.current()->pid_)->state() << endl;
	  if (((QProcess*)(*it)->pid_)->state()==0) //if the process ended
	  //if (GetExitCodeProcess(((QProcess*)it.current()->pid_)->pid()->hProcess,&stWord)) //if the process ended
	  {
		  #ifdef DEBUG_MSG
		  //cout << "valor do stWORD :::: " << stWord << endl;
		  //cout << "valor do Still Active :::: " << STILL_ACTIVE << endl;
		  #endif
		  //if ((unsigned long) stWord != (unsigned long) STILL_ACTIVE)
		  //{
			  pid=(*it)->pid_;

			  status=0;
			  flag=1;

			  break;
		  //}
	  }
//	  if (GetExitCodeProcess(&(it.current()->pid_),stWord)) //if the process ended
//	  {
//		  if (*stWord != STILL_ACTIVE)
//		  {
//			status=0;
//			pid=it.current()->pid_;
//			flag=1;
//			break;
//		  }
//	  }

	  ++it;
  }


  //adicionado por Dario (como colocar o status dependente do erro?)

  //int pid = WaitForSingleObject((HANDLE)job_[jid_]->pid_, 5000);

  //int pid=_cwait(&status,_getpid() , _WAIT_CHILD);

  //int pid=_cwait(&status,job_[jid_]->pid_ , _WAIT_CHILD);

  //int pid = waitpid(-1, &status, WNOHANG);
#else //WIN32
  int pid = waitpid(-1, &status, WNOHANG);
#endif
  switch (pid) {
  case -1:
#ifdef DEBUG_MSG
    qDebug("Task::check error\n");
#endif
    break;
  case 0:
#ifdef DEBUG_MSG
    qDebug("Task::check no child exited\n");
#endif
    break;
  default:
#ifdef DEBUG_MSG
   qDebug("Task::check: ********* pid %d ", pid);
#endif
   ProcessEntry * p = process_[pid];
    if (!p) {
      qDebug("Task::check: pid %d not found in table", pid);
      return;
    }
    p->status_=status;
    INode *iNode = p->node_;
    process_.remove(p->pid_);
    job_.remove(p->jid_);
#ifdef DEBUG_MSG
    qDebug("Task::child(%d) child %d exited - %d left\n", p->jid_, p->pid_,
           process_.count());
#endif
    if (!systemLoad())
      execNext();
    else
      emit stateChanged(0, 0, "", "", systemLoad_);
    if (process_.count() == 0) {
      timer_->stop();
      timerRunning_ = false;
      emit stateChanged(0, 0, "", "", systemLoad_);
    }
    if (iNode) {
#ifdef DEBUG_MSG
    	//cout << "Task::check: STATUS DO PROCESSO %d = %d " << p->pid_ << p->status_ << endl;
#endif
      iNode->taskFinished(p->pid_, p->status_, p->cmd_);
    }
    emit processFinished(p->guiPtr_);
    delete p;
    break;
  }
}

/** Wait until process pid finished   */
void Task::wait(int jid)
{
#ifdef DEBUG_MSG
  qDebug("Task::wait(%d)\n", jid);
#endif
#ifdef WIN32
  if (jid==0)
    while (job_.count()!=0)
      qApp->processEvents(QEventLoop::AllEvents,400);
    else
      while (job_[jid]!=0)
        qApp->processEvents(QEventLoop::AllEvents,400);
#else
  if (jid == 0)
    while (process_.count() != 0)
      qApp->processEvents(QEventLoop::AllEvents,400);
  else
    while (job_[jid] != 0)
      qApp->processEvents(QEventLoop::AllEvents,400);
#endif

#ifdef DEBUG_MSG
  qDebug("Task::wait(%d) done\n", jid);
#endif
}

Task taskTable;

/** set the maximum number of jobs running parallel */
void Task::setMaxJobs(int maxJobs)
{
  maxJobs_ = maxJobs;
}

/** No descriptions */
#ifdef INTERIMAGE
void Task::setGuiPtr(ProcessEntry * pEntry, Q3ListViewItem * ptr)
#else
void Task::setGuiPtr(ProcessEntry * pEntry, QTreeWidgetItem * ptr)
#endif
{
  pEntry->guiPtr_ = ptr;
}

/** return the system load on Linux-Systems */
bool Task::systemLoad()
{
#ifdef linux
  static int num_cpus = 0;
  if (process_.count() == 0)
    return false;
  if (!num_cpus) {
    QProcess script;
    QString program = "/bin/sh";
    QStringList arguments;
    arguments << "-c" << "cat /proc/cpuinfo | grep processor | wc -l";
    script.setReadChannel(QProcess::StandardOutput);
    script.start(program,arguments);
    if (script.waitForFinished()) {
      QString s = script.readLine();
      num_cpus = s.toInt();
      if (num_cpus==0) num_cpus=1;
    }
    else
      num_cpus = 1;

#if 0
    system("cat /proc/cpuinfo | grep processor | wc -l");
    num_cpus = system("exit `cat /proc/cpuinfo | grep processor | wc -l`");
#endif
  }
#if 0
  FILE *systemLoadFp = fopen("/proc/loadavg", "r");
  if (systemLoadFp) {
    rewind(systemLoadFp);
    fscanf(systemLoadFp, "%f", &systemLoad_);
    fclose(systemLoadFp);
    qDebug("Task::systemLoad %d processes running %d max %f num_cpus=%d\n",
           process_.count(), maxJobs_, systemLoad_, num_cpus);
    systemLoad_ /= num_cpus;
    if (maxJobs_ && (process_.count() >= maxJobs_ * num_cpus))
      return true;
    return (systemLoad_ > 1.5);
#endif
	FILE *systemLoadFp=fopen("/proc/stat","r");;
  if (systemLoadFp) {
  	static unsigned int ouser=0;
		static unsigned int onice=0;
		static unsigned int osystem=0;
		static unsigned long oidle=0;
		unsigned int user, nice, system;
		unsigned long idle;
		fscanf(systemLoadFp,"%*s %u %u %u %lu",&user,&nice,&system,&idle);
  	fclose(systemLoadFp);
		float busy=system-osystem+user-ouser;
	  systemLoad_=busy/(idle-oidle+busy);
#ifdef DEBUG_MSG
	qDebug("Task::systemLoad: %f (%u %u %u %lu %d %d %d %ld)",systemLoad_,user,nice,system,idle,
	       user-ouser,nice-onice,system-osystem,idle-oidle);
#endif	
		osystem=system;
		ouser=user;
		onice=nice;
		oidle=idle;
    if (maxJobs_ && (process_.count() >= maxJobs_ * num_cpus))
      return true;
    return (systemLoad_ > 0.9);
  }
#endif
#ifdef WIN32
#ifdef DEBUG_MSG
  if ((int)process_count >= (int)maxJobs_)
    QMessageBox::information(0,"", "process_count >= maxJobs_",1);
#endif //DEBUG_MSG
  return ((int)process_count >= (int)maxJobs_);
#else //WIN32
  return (process_.count() >= maxJobs_);
#endif
}
