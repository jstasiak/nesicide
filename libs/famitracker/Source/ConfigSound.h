/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2012  Jonathan Liss
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
*/

#pragma once

#include "cqtmfc.h"
#include "resource.h"

// CConfigSound dialog

class CConfigSound : public CPropertyPage
{
   Q_OBJECT
   // Qt interfaces
public slots:
   void devices_currentIndexChanged(int value);
   void sampleRate_currentIndexChanged(int value);
   void sampleSize_currentIndexChanged(int value);
   void bufLength_valueChanged(int value);
   void bassFreq_valueChanged(int value);
   void trebleFreq_valueChanged(int value);
   void trebleDamp_valueChanged(int value);
   void volume_valueChanged(int value);
public: // For some reason MOC doesn't like the protection specification inside DECLARE_DYNAMIC

	DECLARE_DYNAMIC(CConfigSound)

public:
	CConfigSound();
	virtual ~CConfigSound();

// Dialog Data
	enum { IDD = IDD_CONFIG_SOUND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnApply();
	afx_msg void OnCbnSelchangeSampleRate();
	afx_msg void OnCbnSelchangeSampleSize();
	afx_msg void OnCbnSelchangeDevices();
};
