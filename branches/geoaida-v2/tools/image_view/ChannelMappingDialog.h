/***************************************************************************
                          ChannelMappingDialog.h  -
                             -------------------
    begin                : Wed Jul 16 2008
    copyright            : (C) 2008 TNT, Uni Hannover
    authors              : Karsten Vogt

    email                : vogt@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _CHANNELMAPPINGDIALOG_H_
#define _CHANNELMAPPINGDIALOG_H_

#include <QDialog>
#include <QGroupBox>
#include <QSpinBox>

class ChannelMappingDialog : public QDialog
{
	Q_OBJECT

	public:
		ChannelMappingDialog(int mode, int mapping1, int mapping2, int mapping3, int channelCount, QWidget *parent=0);

		int mode();
		int mapping(int nr);

	protected:

	public slots:

	private slots:
		void oneChannelGroupClicked(bool status=false);
		void threeChannelGroupClicked(bool status=false);

	private:
		QGroupBox *_oneChannelGroup;
		QGroupBox *_threeChannelGroup;

		QSpinBox *_oneChannel1Spin;
		QSpinBox *_threeChannel1Spin;
		QSpinBox *_threeChannel2Spin;
		QSpinBox *_threeChannel3Spin;
};

#endif
