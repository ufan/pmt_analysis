/*
 T his library is free s*oftware; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License version 2 as published by the Free Software Foundation.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.
 
 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
 
 ---
 Copyright (C) 2015, ufan <>
 */
#ifndef _PTAnaAlgorithm_h_
#define _PTAnaAlgorithm_h_

#include "PTAnaPMTPedestals.h"
#include "TFile.h"

PTAnaPMTPedestals* fit_pedestal(const char* parentdir,const char* testdir,const char* date,int testid);
void fit_pedestal_batch(const char* outfile,const char* filelist);
void check_pedestal(const char* pardir,const char* testdir);
void fit_testingdata(const char* outfile,const char* pardir,const char* testdir,int testid,const char* testdate);
void fit_testingdata_batch(const char* outfile,const char* filelist);
void check_testdata(const char* infile,const char* serialno);
void get_configurations(TFile* infile);
void get_references(TFile* filein);
#endif // _PTAnaAlgorithm_h_
