/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#ifndef _PreComp_
# include <qapplication.h>
# include <qaction.h>
# include <qbitmap.h>
# include <qbuttongroup.h>
# include <qcursor.h>
# include <qfiledialog.h>
# include <qlayout.h>
# include <qlineedit.h>
# include <qmessagebox.h>
# include <qprocess.h>
# include <qdragobject.h>
# include <qstylesheet.h>
# include <qtoolbutton.h>
# include <qtooltip.h>
# include <qthread.h>
# include <qurl.h>
# include <qvaluestack.h>
# include <qwhatsthis.h>
# include <ctype.h>
# ifdef FC_OS_WIN32
#   include <windows.h>
# endif
#endif

#include "HtmlView.h"
#include "Process.h"
#include "Application.h"
#include "CustomWidgets.h"
#include "Tools.h"
#include "WhatsThis.h"
#include "../Base/Interpreter.h"
#include "../Base/Exception.h"
#include "../Base/Documentation.h"
#ifndef FC_OS_LINUX
# include <direct.h>
#endif

#ifdef FC_OS_LINUX
# include<unistd.h> //for chdir
# include<stdlib.h> //for system
# ifdef _chdir
#   warning _chdir already defined, cross thumbs
# endif
# define _chdir chdir
#endif

using namespace Gui;

/* XPM */
/* Drawn  by Mark Donohoe for the K Desktop Environment */
/* See http://www.kde.org */
static const char* back_pixmap[]={
"22 22 68 2",
".# c None",
"Qt c None",
".a c #000000",
".r c #060402",
".d c #100a04",
".E c #150d06",
".K c #160d04",
".g c #181008",
".k c #1c1208",
".b c #22180c",
".x c #23180c",
"#b c #713600",
".T c #763b00",
".X c #793b00",
".1 c #7b3b00",
".8 c #7c3b00",
"## c #7d3b00",
".P c #7f4308",
".h c #936738",
".s c #966739",
".c c #9d5f12",
".e c #a4733f",
".F c #af743f",
".w c #b16000",
".l c #b17a43",
".y c #b37943",
".D c #b46000",
".7 c #b75800",
".J c #b76000",
".O c #ba6000",
".S c #bd6000",
".W c #c06000",
".0 c #c36000",
".6 c #c66000",
".5 c #c76100",
"#. c #c96100",
"#a c #cb6100",
".f c #cd7300",
".j c #d37500",
".i c #d78d40",
".4 c #d86900",
".q c #d97600",
".o c #db7800",
".n c #de821a",
".v c #df7800",
".u c #e07e12",
".t c #e1903e",
".m c #e29340",
".C c #e37800",
".B c #e37902",
".p c #e39644",
".A c #e4821a",
".I c #e67800",
".H c #e77e12",
".z c #e99442",
".N c #ea7800",
".G c #ea913c",
".Q c #eb7700",
".M c #eb7f13",
".U c #ee7700",
".R c #ee7800",
".L c #ee913c",
".V c #f17800",
".Y c #f37700",
".Z c #f57800",
".2 c #f67700",
".9 c #f87700",
".3 c #f87800",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQt.#.a.#.#.#QtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQt.#.b.c.a.#.#QtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQt.#.d.e.f.a.#.#QtQtQtQtQtQtQtQt",
"QtQtQtQtQtQt.#.g.h.i.j.a.a.a.a.a.a.a.a.#.#Qt",
"QtQtQtQtQt.#.k.l.m.n.o.n.p.p.p.p.p.q.a.#.#Qt",
"QtQtQtQt.#.r.s.t.u.v.v.v.v.v.v.v.v.w.a.#.#Qt",
"QtQtQt.#.x.y.z.A.B.C.C.C.C.C.C.C.C.D.a.#.#Qt",
"QtQt.#.E.F.G.H.I.I.I.I.I.I.I.I.I.I.J.a.#.#Qt",
"QtQt.#.K.L.M.N.N.N.N.N.N.N.N.N.N.N.O.a.#.#Qt",
"QtQtQt.a.P.Q.R.R.R.R.R.R.R.R.R.R.R.S.a.#.#Qt",
"QtQtQt.#.a.T.U.V.V.V.V.V.V.V.V.V.V.W.a.#.#Qt",
"QtQtQt.#.#.a.X.Y.Z.Z.Z.Z.Z.Z.Z.Z.Z.0.a.#.#Qt",
"QtQtQt.#.#.#.a.1.2.3.4.5.6.6.6.6.6.7.a.#.#Qt",
"QtQtQtQt.#.#.#.a.8.9#..a.a.a.a.a.a.a.a.#.#Qt",
"QtQtQtQtQt.#.#.#.a###a.a.#.#.#.#.#.#.#.#.#Qt",
"QtQtQtQtQtQt.#.#.#.a#b.a.#.#.#.#.#.#.#.#.#Qt",
"QtQtQtQtQtQtQt.#.#.#.a.#.#.#QtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQt.#.#.#.#.#.#QtQtQtQtQtQtQtQt"};

/* XPM */
/* Drawn  by Mark Donohoe for the K Desktop Environment */
/* See http://www.kde.org */
static const char* forward_pixmap[]={
"22 22 67 2",
".# c None",
"Qt c None",
".a c #000000",
".w c #060200",
".f c #0f0800",
".L c #120a00",
".G c #130a00",
".j c #160c00",
".r c #190d00",
".c c #211200",
".B c #221200",
".V c #582c00",
".0 c #5a2c00",
".6 c #5b2c00",
".9 c #5c2c00",
"## c #5e2c00",
".Q c #5e2f00",
"#a c #773900",
".i c #8b4e00",
".v c #8e4d00",
".e c #975500",
".F c #995000",
".b c #a16b2d",
".q c #a75b00",
".A c #ad5c00",
".K c #b85f00",
".P c #bd6000",
".U c #bf6000",
".Z c #c26000",
".5 c #c56000",
".2 c #c66000",
".8 c #c76000",
".h c #cd7200",
".u c #d97400",
".J c #da7100",
".o c #db7800",
".p c #db7902",
".O c #dd7000",
".T c #df7000",
".t c #df7800",
".d c #df9645",
".n c #e08d34",
".g c #e19646",
".y c #e37800",
".z c #e37902",
".l c #e39644",
".m c #e39647",
".Y c #e47000",
".E c #e47700",
".s c #e59644",
".k c #e5a058",
".D c #e67800",
".3 c #e77000",
".x c #e99644",
".I c #ea7800",
".C c #eb9644",
".N c #ee7800",
".H c #ee9644",
".S c #f17800",
".M c #f19644",
".R c #f39644",
".1 c #f57600",
".X c #f57800",
".7 c #f77600",
".W c #f79644",
".4 c #f87800",
"#. c #f97600",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQt.#.a.#.#.#QtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQt.a.b.c.#.#.#QtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQt.a.d.e.f.#.#.#QtQtQtQtQt",
"QtQtQt.a.a.a.a.a.a.a.a.g.h.i.j.#.#.#QtQtQtQt",
"QtQtQt.a.k.l.l.l.l.l.m.n.o.p.q.r.#.#.#QtQtQt",
"QtQtQt.a.s.t.t.t.t.t.t.t.t.t.u.v.w.#.#.#QtQt",
"QtQtQt.a.x.y.y.y.y.y.y.y.y.y.y.z.A.B.#.#.#Qt",
"QtQtQt.a.C.D.D.D.D.D.D.D.D.D.D.D.E.F.G.#.#Qt",
"QtQtQt.a.H.I.I.I.I.I.I.I.I.I.I.I.J.K.L.#.#Qt",
"QtQtQt.a.M.N.N.N.N.N.N.N.N.N.N.O.P.Q.a.#.#Qt",
"QtQtQt.a.R.S.S.S.S.S.S.S.S.S.T.U.V.a.#.#.#Qt",
"QtQtQt.a.W.X.X.X.X.X.X.X.X.Y.Z.0.a.#.#.#.#Qt",
"QtQtQt.a.1.2.2.2.2.2.3.4.3.5.6.a.#.#.#.#.#Qt",
"QtQtQt.a.a.a.a.a.a.a.a.7.8.9.a.#.#.#.#.#QtQt",
"QtQtQt.#.#.#.#.#.#.#.a#.##.a.#.#.#.#.#QtQtQt",
"QtQtQt.#.#.#.#.#.#.#.a#a.a.#.#.#.#.#QtQtQtQt",
"QtQtQtQtQtQtQtQtQtQt.#.#.#.#.#.#.#QtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQt.#.#.#.#.#.#QtQtQtQtQtQt"};

/* XPM */
/* Drawn  by Mark Donohoe for the K Desktop Environment */
/* See http://www.kde.org */
static const char* home_pixmap[]={
"22 22 167 2",
".# c None",
"Qt c None",
".b c #000000",
".0 c #000402",
".L c #000f08",
"aG c #00100a",
".y c #002a18",
"aH c #002d19",
"#m c #002f1a",
".K c #00301a",
".f c #00311c",
".m c #00331d",
".Z c #004326",
"aE c #005732",
".J c #005a33",
"aj c #005e34",
".x c #005f36",
"aD c #006137",
".w c #006338",
"as c #00673a",
".Y c #006d3e",
"aC c #006f3f",
".l c #007342",
"#H c #020000",
"#b c #04301d",
".h c #060200",
".G c #081910",
"#a c #087a47",
"aA c #0a1912",
"ar c #0a7a47",
".a c #0c1c13",
"aB c #0c633b",
".v c #0c7c4a",
"#j c #124b30",
".I c #127e4d",
".V c #131e19",
".s c #163023",
"#9 c #163727",
"#k c #16804f",
".7 c #190a00",
"#w c #19392a",
"#q c #1d0c00",
"#7 c #21362b",
"#v c #21523a",
".3 c #220d00",
"#u c #230f00",
"ag c #25211c",
"#I c #25372d",
".p c #260f00",
"#Q c #271000",
"#l c #2b895c",
"#U c #2c1200",
".D c #2f1300",
".c c #317354",
".t c #327355",
".S c #331500",
".H c #337455",
"#G c #341604",
".i c #385045",
"#e c #3a1800",
"ai c #3a8c65",
"#C c #3b1900",
".d c #3b5246",
"#i c #3c1900",
".u c #3c9169",
"#c c #3f1900",
"#W c #3f2618",
"#V c #406151",
"#Y c #411a00",
".g c #422a1d",
".k c #43956e",
".X c #44966f",
"ap c #46725c",
"#d c #494338",
".2 c #4b1e00",
".e c #4b9873",
"#Z c #4c463a",
"aq c #4c9973",
"#. c #4e8269",
"#8 c #52856d",
"a. c #555044",
"## c #58a07d",
".n c #5f4436",
".M c #61493b",
".z c #614a3c",
"#6 c #672a00",
".8 c #6c2727",
"ah c #6eaa8d",
".P c #712d00",
".W c #71ac8f",
".j c #77b195",
".6 c #793100",
".C c #7c3300",
"#R c #7d3300",
".9 c #7e0000",
"#F c #7e3300",
"#P c #817761",
"#t c #823400",
"#5 c #823600",
"#r c #843702",
".U c #850000",
"#f c #853600",
".F c #860000",
"#X c #893800",
"#p c #8a8069",
".1 c #8c3900",
"#h c #8e3a02",
"af c #8f846b",
"#D c #913a00",
".4 c #924210",
"#T c #943c00",
"#n c #94886f",
"#x c #958970",
"#E c #963e00",
".O c #973e00",
".R c #98420a",
".o c #9b5831",
"a# c #9d9175",
"#B c #a09377",
"#s c #a14100",
"aK c #a2000d",
"aw c #a41019",
".B c #a54506",
".r c #a60000",
"#4 c #a6997c",
"#S c #a74608",
"#g c #a84502",
".5 c #ac4f16",
".Q c #ad5725",
".T c #b24141",
".E c #b74343",
"ao c #b9aa89",
"#J c #ba7045",
"aJ c #bbac8c",
".A c #bc734c",
".N c #bd764f",
"aF c #be000f",
"aI c #bf000f",
".q c #c15858",
"#o c #c2b291",
"az c #c7b694",
"ad c #cbba97",
"ac c #cdbc99",
"#y c #cfbf9b",
"#K c #d0bf9c",
"aa c #d3c19e",
"ak c #d9ccae",
"ab c #ddcba5",
"al c #dfcea7",
"#z c #e6d4ac",
"#L c #ead7af",
"#A c #ecd9b1",
"ae c #edd9b2",
"#O c #f3dfb5",
"at c #f3e3c2",
"an c #f4e0b7",
"au c #f6e2b8",
"#M c #f8e4ba",
"#0 c #f9e5ba",
"#N c #fbe6bb",
"av c #fce7bc",
"#3 c #fde8bd",
"ay c #fde9be",
"#1 c #fee9be",
"#2 c #ffeabf",
"am c #ffecc3",
"ax c #ffeecc",
"QtQtQt.#.a.#.#.#QtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQt.#.b.c.b.#.#.#QtQtQt.#.#.#.#QtQtQtQtQtQt",
"Qt.#.#.d.e.f.#.#.#.#.#.#.g.h.#.#.b.b.b.b.#.#",
"Qt.#.i.j.k.l.m.#.#.#.b.n.o.p.#.#.b.q.r.b.#.#",
".#.s.t.u.v.w.x.y.#.b.z.A.B.C.D.b.b.E.F.b.#.#",
".#.#.G.H.I.J.K.L.b.M.N.O.P.Q.R.S.b.T.U.b.#.#",
".#.#.V.W.X.Y.Z.0.M.A.1.2.3.4.5.6.7.8.9.b.#.#",
"Qt.b#.##.k#a#b.M.A.1#c#d.h#e#f#g#h#i.h.b.#.#",
"Qt.##j#k#l#m.M.A.1#c#d#n#o#p#q#r#s#t#u.b.#.#",
"Qt.#.##v#w.M.A.1#c#d#x#y#z#A#B#C#D#E#F#G#H.#",
"Qt.#.b#I.n#J.1#c#d#x#K#L#M#N#O#P#Q#R#S#T#U.b",
"Qt.b#V#W.5#X#Y#Z#x#K#L#0#1#2#3#O#4#U#5#X#6.b",
".##7#8#9.b.ba.a#aa#L#0abacadae#N#Oafag.b.b.b",
".#.Vahaiaj.bakalae#0#1ac.b.b.bam#3anao.b.#.#",
".#apaqaras.batauav#1#2ad.baw.bax#2ayaz.b.#.#",
"aAaBaCaDaE.bax#2#2#2#2ad.baF.bax#2#2ad.b.#.#",
".#.baG.faH.bax#2#2#2#2ad.baI.bax#2#2ad.b.#.#",
".#.#.#.#.#.bavadadadadaJ.baK.bavadadaJ.b.#.#",
".#.#.#.#.#.b.b.b.b.b.b.b.b.b.b.b.b.b.b.b.#.#",
"QtQt.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#",
"QtQtQtQtQt.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt"};

/* XPM */
static const char *open_pixmap[] = {
"22 22 236 2",
".# c None",
"Qt c None",
".a c #000000",
"bP c #060806",
"#9 c #080a08",
"bK c #0a0c0a",
"bt c #0f100f",
"a6 c #121510",
"bD c #121612",
"bi c #161a16",
".y c #191d19",
"aQ c #1c201c",
"bJ c #1c211a",
"#. c #272b27",
".S c #281515",
".d c #282a28",
"#a c #2c1919",
"#t c #2c1a1a",
"#4 c #2d1e1d",
"bC c #2f392d",
"#3 c #322826",
"#s c #372d2c",
"ay c #373e36",
"bE c #384137",
"#2 c #3a4239",
"bL c #3a4339",
"aj c #3b2d2d",
".g c #3e473c",
"ai c #413a38",
".R c #413c3c",
"#h c #424a41",
"#r c #424b42",
"an c #424c41",
"bs c #425241",
"#L c #443e3a",
"ag c #444e43",
"bj c #444f43",
"#T c #444f44",
"bI c #455143",
".9 c #465146",
"bq c #475446",
"#J c #495349",
"bu c #4a5549",
"ax c #4a554a",
"## c #4c4141",
".O c #4c554b",
"ah c #4f554b",
".o c #4f5a4e",
"aR c #4f5b4f",
".I c #515251",
"#1 c #525f51",
"#b c #544141",
".v c #545454",
"#A c #545b54",
".T c #554242",
"#K c #555a52",
"a7 c #556354",
"bh c #556a54",
"#q c #576456",
"#M c #584646",
"#u c #5a4747",
"bf c #5a6456",
".0 c #5b625b",
".z c #5b6759",
"#B c #5b6d5a",
".k c #5c5c5c",
"#5 c #604e4e",
"a2 c #60705f",
"aM c #616a60",
".Z c #626462",
"az c #635e5a",
"bM c #637061",
"a3 c #676361",
".H c #676867",
"#l c #677666",
"a5 c #678066",
".l c #6c6c6c",
".B c #707b6f",
"bO c #71806f",
".c c #737373",
"#g c #737973",
".1 c #737d73",
"#7 c #738372",
".C c #756c6c",
".m c #757775",
"bB c #758074",
"bF c #768675",
".u c #777777",
"al c #778176",
"bv c #778876",
"#R c #778c75",
"aP c #789576",
".D c #7c6f6f",
".Q c #7c8c7b",
"aA c #7d847d",
".U c #807272",
"#c c #817474",
"aw c #839381",
"be c #839581",
"#v c #857979",
"aO c #858884",
"bA c #869684",
".J c #878f87",
"#N c #887c7c",
"bN c #889786",
"a8 c #889986",
"aB c #88a886",
"bH c #899a88",
"bg c #8a8a8a",
"bk c #8a9b89",
"#0 c #8b9e8a",
"br c #8c8d8c",
"a4 c #8d8d8d",
".8 c #8d9f8c",
"aC c #8da08c",
"bG c #8e9f8c",
"a. c #8ea18c",
"#D c #8ea18d",
"#i c #8fa28e",
"bz c #90a18f",
"#6 c #918787",
"aL c #91a18f",
"#p c #91a28f",
".x c #939e93",
"aN c #948e8d",
"bp c #94a491",
"by c #94a593",
"#I c #95a693",
"af c #95a894",
".n c #969f95",
"bo c #97a995",
"bx c #97a996",
"am c #99bd97",
".w c #9a9d9a",
"bd c #9aab98",
"bn c #9aac98",
"bw c #9bad99",
".5 c #9bc099",
".p c #9c9999",
"a1 c #9daf9b",
"bc c #9eb09c",
"aS c #9eb19c",
".E c #a09797",
"ak c #a09898",
".e c #a1a4a1",
".A c #a1aba1",
"a0 c #a1b29f",
"bb c #a1b4a0",
"bm c #a1b4a1",
".V c #a39c9c",
".b c #a3a3a3",
"aK c #a3b6a2",
"#d c #a49e9e",
"aZ c #a4b7a2",
"ba c #a4b8a3",
"bl c #a5b8a3",
"#f c #a6a7a6",
"aJ c #a7baa5",
"#w c #a8a2a2",
".t c #a8a8a8",
"aY c #a8baa6",
"b# c #a8bba6",
"#8 c #a9d1a6",
"#O c #aaa3a3",
"aI c #aabea9",
"aX c #aabfaa",
"b. c #abbfaa",
"av c #adc1ab",
"aH c #adc1ac",
"aW c #aec2ac",
"a9 c #afc2ad",
"#j c #afc5ad",
"au c #b0c4ae",
".h c #b1b0b0",
"aG c #b1c5af",
"aV c #b1c6b0",
".P c #b2c7b0",
".N c #b3bbb2",
"at c #b3c7b2",
"aF c #b4c8b2",
"aU c #b4c9b2",
"ae c #b6cbb4",
"as c #b7ccb5",
".G c #b8b9b8",
"aT c #b8cdb6",
"#S c #b8e4b5",
".q c #b9b6b6",
".Y c #b9bab9",
"ad c #b9ceb8",
".F c #bab7b7",
"ar c #bacfb9",
".6 c #bad0b9",
".W c #bcbaba",
"ac c #bdd3bb",
"aq c #bdd4bc",
"#x c #bebcbc",
"#z c #bebebe",
".K c #bec7bd",
"aE c #bed4bc",
"#P c #c0bdbd",
"#Z c #c0d5be",
".2 c #c1d1c1",
"ab c #c1d7bf",
"ap c #c1d7c0",
"aD c #c1d8c1",
".3 c #c3d1c2",
"#Y c #c3d9c2",
"aa c #c4dac2",
"#C c #c4f3c1",
".j c #c5c5c5",
".4 c #c6cfc5",
".r c #c7c6c6",
"#e c #c7c7c7",
"#X c #c7ddc5",
"a# c #c7dec6",
"ao c #c7dfc6",
".X c #c8c7c7",
"#y c #c9c8c8",
"#Q c #c9c9c9",
".M c #c9d0c8",
"#H c #c9e0c7",
"#W c #cae1c8",
"#o c #cae2c9",
".i c #cbcbcb",
".s c #cccccc",
"#U c #cce3ca",
"#G c #cce4cb",
"#V c #cde4cb",
"#k c #cdfeca",
"#F c #cfe7ce",
".L c #d2d7d2",
"#E c #d2ead0",
".f c #d4d4d4",
".7 c #d4ebd2",
"#m c #d4ecd2",
"#n c #d4edd3",
"QtQtQtQt.#.#.#.#.#.#.#.#.#.#.#.#.#.#QtQtQtQt",
"QtQtQt.#.a.a.#.a.a.a.#.#.#.#.#.#.#.#.#.#.#.#",
".#.#.#.a.b.c.d.e.f.g.a.a.#.#.#.#.#.#.#.#.#.#",
".#.#.#.a.h.i.j.k.l.m.n.o.a.a.#.a.a.#.#.#.#.#",
".#.#.#.a.p.q.r.s.t.u.v.w.x.y.z.A.B.a.a.#.#.#",
".a.a.a.a.C.D.E.F.r.i.G.H.I.J.K.L.M.N.O.a.a.#",
".a.P.Q.y.R.S.T.U.V.W.X.s.Y.Z.0.1.2.3.4.5.a.#",
".a.6.7.8.9#.###a#b#c#d.W#e.i#f#g#h#i#j#k.a.#",
".##l#m#n#o#p#q#r#s#t#u#v#w#x#y.s#z#A#B#C.a.#",
".#.a#D#E#F#G#H#I#J#K#L#a#M#N#O#P#Q.s#R#S.a.#",
".#.##T#U#V#W#X#Y#Z#0#1#2#3#4#5#6.W.i#7#8.a.#",
".#.##9a.#oa#aaabacadaeafagahaiajak.ralam.a.#",
".#.#.#anaoaaapaqarasatauavawaxayaz.FaAaB.a.#",
".#.#.#.aaCaDaE.6asaFaGaHaIaJaKaLaMaNaOaP.a.#",
".#.#.#.#aQaRaSaTaUaVaWaXaYaZa0a1a2a3a4a5.a.#",
".#.#.#.#.#.#a6a7a8a9b.b#babbbcbdbebfbgbh.a.#",
".#.#.#.#.#.#.#.abibjbkblbmaSbnbobpbqbrbs.a.#",
"Qt.#.#.#.#.#.#.#.#.#btbubvbwbxbybzbAbBbC.a.#",
"QtQt.#.#.#.#.#.#.#.#.#.abDbEbF#pbGbHbIbJ.a.#",
"QtQtQt.#.#.#.#.#.#.#.#.#.#.abKbLbMbNbObP.a.#",
"QtQtQtQtQt.#.#.#.#.#.#.#.#.#.#.a.a.a.a.a.a.#",
"QtQtQtQtQtQtQt.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#"};

/*
// looks like MS cursor for hyperlinks
#define cb_width  32
#define cb_height 32
static unsigned char cb_bits[] = {    // cursor bitmap
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x48,0x00,0x00,0x00,0x48,
 0x00,0x00,0x00,0x48,0x00,0x00,0x00,0x48,0x00,0x00,0x00,0xc8,0x01,0x00,0x00,
 0x48,0x0e,0x00,0x00,0x48,0x32,0x00,0x00,0x48,0x52,0x00,0x80,0x4b,0x92,0x00,
 0x80,0x0c,0x90,0x00,0x80,0x08,0x80,0x00,0x00,0x09,0x80,0x00,0x00,0x0a,0x80,
 0x00,0x00,0x02,0x80,0x00,0x00,0x04,0x80,0x00,0x00,0x04,0x40,0x00,0x00,0x08,
 0x40,0x00,0x00,0x08,0x40,0x00,0x00,0x10,0x20,0x00,0x00,0x10,0x20,0x00,0x00,
 0xf0,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

#define cm_width  32
#define cm_height 32
static unsigned char cm_bits[] = {    // cursor bitmap mask
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x78,
 0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0xf8,0x01,0x00,0x00,
 0xf8,0x0f,0x00,0x00,0xf8,0x3f,0x00,0x00,0xf8,0x7f,0x00,0x80,0xfb,0xff,0x00,
 0x80,0xff,0xff,0x00,0x80,0xff,0xff,0x00,0x00,0xff,0xff,0x00,0x00,0xfe,0xff,
 0x00,0x00,0xfe,0xff,0x00,0x00,0xfc,0xff,0x00,0x00,0xfc,0x7f,0x00,0x00,0xf8,
 0x7f,0x00,0x00,0xf8,0x7f,0x00,0x00,0xf0,0x3f,0x00,0x00,0xf0,0x3f,0x00,0x00,
 0xf0,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
*/
// --------------------------------------------------------------------------

class FCTextBrowserPrivate
{
public:
  enum TMode {Backward, Forward, None};

  FCTextBrowserPrivate();

  QValueStack<QString> fdStack;
  QValueStack<QString> bdStack;

  bool getType (const QString& url, TDocType type);
  TMode tMode;
  int minWidth;
};

FCTextBrowserPrivate::FCTextBrowserPrivate()
{
  tMode = FCTextBrowserPrivate::None;
  minWidth = 1000;
}

bool FCTextBrowserPrivate::getType(const QString& url, TDocType type)
{
  if (url.find(".html", 0, false) != -1)
  {
    type = Html;
    return true;
  }
  else if (url.find(".tex", 0, false) != -1)
  {
    type = Tech; // LaTeX
    return true;
  }
  else if(url.find(".script", 0, false) != -1)
  {
    type = Script;
    return true;
  }

  return false;
}

// --------------------------------------------------------------------------

class FCBrowserFactoryData
{
private:
  FCBrowserFactoryData();
  ~FCBrowserFactoryData();
  static FCBrowserFactoryData *_pcSingleton;

public:
  static void Destruct(void);
  static FCBrowserFactoryData &Instance();
  QStringList mPaths;
  QStringList mRoots;
};

FCBrowserFactoryData * FCBrowserFactoryData::_pcSingleton = 0;

void FCBrowserFactoryData::Destruct(void)
{
  assert(_pcSingleton);
  delete _pcSingleton;
}

FCBrowserFactoryData & FCBrowserFactoryData::Instance()
{
  if(!_pcSingleton)
  {
    _pcSingleton = new FCBrowserFactoryData;
  }

  return *_pcSingleton;
}

FCBrowserFactoryData::FCBrowserFactoryData()
{
}

FCBrowserFactoryData::~FCBrowserFactoryData()
{
}

// --------------------------------------------------------------------------

class FCDocumentationSource : public QStoredDrag
{
public:
  FCDocumentationSource( const char * mimeType, QString path, TDocType type )
      : QStoredDrag(mimeType, 0L, 0), mPath(path), mType(type)
  {
    QStringList paths = FCBrowserFactoryData::Instance().mPaths;
    QStringList roots = FCBrowserFactoryData::Instance().mRoots;
    QString s;
    unsigned int i = 0;
    for (QStringList::Iterator it = paths.begin(); it!=paths.end(); ++it, ++i)
    {
      s = *it; s = s.left(s.length()-1);
      if (mPath.startsWith(s))
      {
        mPath = mPath.right(mPath.length() - s.length() - 1);
        mRoot = *roots.at(i);
        break;
      }
    }
  }

  QByteArray encodedData (const char* data) const
  {
    QString fn = QString("%1%2").arg(mRoot).arg(mPath);
    int pos = fn.findRev('.'); fn = fn.left(pos);

    std::string text = GetDocumentationManager().Retrive(fn.latin1(), mType );
    QCString test = text.c_str();

    if (test.isEmpty())
    {
      test = QString(
               "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">"
               "<html>"
               "<head>"
               "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\">"
               "<title>FreeCAD Main Index</title>"
               "</head>"
               "<body bgcolor=\"#ffffff\">"
               "<table cellpadding=2 cellspacing=1 border=0  width=100% bgcolor=#E5E5E5 >"
               "<tr>"
               "<th bgcolor=#FFCC66 width=33%%>"
               "<h1>:-(</h1>"
               "<h2>Sorry, but cannot load the file because the doc manager failed to convert it into HTML.</h2>"
               "<h1>:-(</h1>"
               "</th>"
               "</tr>"
               "</table>"
               "</body></html>");
    }

    return test;
  }

private:
  QString  mRoot;
  QString  mPath;
  TDocType mType;
};

// --------------------------------------------------------------------------

class FCHtmlViewPrivate
{
public:
  FCHtmlViewPrivate();

  std::string aStrGroupPath;
  QString        m_FCdoc, m_FCext, m_FCscript;
  std::map<int, QString> mHistory, mBookmarks;
  bool bBackward, bForward;
  bool bHistory, bBookm;
  int  iMaxHist, iMaxBookm;
  QString        selectedURL;
  QString        m_strDocDir;
  QString        m_strCaption;
  Gui::Process   m_Process;
};

FCHtmlViewPrivate::FCHtmlViewPrivate()
    :  bBackward(false),
    bForward(false),
    bHistory(false),
    bBookm(false),
    selectedURL()
{
}

// --------------------------------------------------------------------------

FCBrowserSourceFactory::FCBrowserSourceFactory()
    : QMimeSourceFactory()
{
  FCBrowserFactoryData::Instance();
}

FCBrowserSourceFactory::~FCBrowserSourceFactory()
{
  FCBrowserFactoryData::Destruct();
}

const QMimeSource* FCBrowserSourceFactory::data(const QString& abs_name) const
{
  if (canConvertToHTML(abs_name))
  {
    TDocType type=Html;
    FCTextBrowserPrivate d;
    d.getType(abs_name, type);

    QString path = abs_name;
    int pos = path.findRev('/'); if (pos == -1) pos = path.findRev('\\');

#ifdef FC_OS_WIN32
    path = path.left(pos);
    path += '\\';
#else
    path = path.left(pos+1);
#endif

    if (FCBrowserFactoryData::Instance().mPaths.find(path) == FCBrowserFactoryData::Instance().mPaths.end())
    {
      FCBrowserFactoryData::Instance().mPaths.append(path);
      QString root = QString("FCDoc:/%1/").arg(path);
      FCBrowserFactoryData::Instance().mRoots.append(root);
      GetDocumentationManager().AddProvider(new FCDocProviderDirectory(root.latin1(),path.latin1()));
    }

    return new FCDocumentationSource("text/html;charset=iso8859-1", abs_name, type);
  }
  else
    return QMimeSourceFactory::data(abs_name);
}

QString FCBrowserSourceFactory::makeAbsolute(const QString& abs_or_rel_name, const QString& context) const
{
  return QMimeSourceFactory::makeAbsolute(abs_or_rel_name, context);
}

void FCBrowserSourceFactory::setText( const QString& abs_name, const QString& text )
{
  QMimeSourceFactory::setText(abs_name, text);
}

void FCBrowserSourceFactory::setImage( const QString& abs_name, const QImage& im )
{
  QMimeSourceFactory::setImage(abs_name, im);
}

void FCBrowserSourceFactory::setPixmap( const QString& abs_name, const QPixmap& pm )
{
  QMimeSourceFactory::setPixmap(abs_name, pm);
}

void FCBrowserSourceFactory::setData( const QString& abs_name, QMimeSource* data )
{
  QMimeSourceFactory::setData(abs_name, data);
}

void FCBrowserSourceFactory::setFilePath( const QStringList& s)
{
  QMimeSourceFactory::setFilePath(s);
}

QStringList FCBrowserSourceFactory::filePath() const
{
  return QMimeSourceFactory::filePath();
}

void FCBrowserSourceFactory::setExtensionType( const QString& ext, const char* mimetype )
{
  QMimeSourceFactory::setExtensionType(ext, mimetype);
}

bool FCBrowserSourceFactory::canConvertToHTML (const QString& url)
{
  TDocType type=Html;
  FCTextBrowserPrivate d;
  return d.getType(url, type);
}

// --------------------------------------------------------------------------

FCTextBrowser::FCTextBrowser(QWidget * parent, const char * name)
    : QTextBrowser(parent, name)
{
  WhatsThis::setHelpView( this );
  d = new FCTextBrowserPrivate;

  //  setMimeSourceFactory(new FCBrowserSourceFactory);

  setHScrollBarMode(QScrollView::AlwaysOff);
  setVScrollBarMode(QScrollView::AlwaysOff);
  mimeSourceFactory()->setExtensionType("HTML", "text/html;charset=iso8859-1");
  mimeSourceFactory()->setExtensionType("HTM", "text/html;charset=iso8859-1");
  mimeSourceFactory()->setExtensionType("FCParam", "text/xml;charset=UTF-8");

  setAcceptDrops( TRUE );
  viewport()->setAcceptDrops( TRUE );
}

FCTextBrowser::~FCTextBrowser()
{
  delete d;
}

void FCTextBrowser::setSource (const QString & name)
{
  QString source = name;
  QString mark;
  int hash = name.find('#');
  if ( hash != -1)
  {
    source  = name.left( hash );
    mark = name.mid( hash+1 );
  }

  QString url = mimeSourceFactory()->makeAbsolute( source, context() );
  QString txt;

  if (!source.isEmpty())
  {
    const QMimeSource* mime = mimeSourceFactory()->data(source, context());
    if (mime == NULL)
    {
      int type = Tools::getURLType(source);
      if ( type == 1 )
      {
        QString msg = tr("File %1 does not exist.\n").arg(source);
        bool bMute = GuiConsoleObserver::bMute;
        GuiConsoleObserver::bMute = true;
        Base::Console().Error(msg.latin1());
        GuiConsoleObserver::bMute = bMute;
        setText(tr("File not found"));
        return;
      }
      else if ( type == 2 )
      {
        QString msg = tr("Can't load '%1'.\nDo you want to start your favourite external browser instead?").arg(source);
        if (QMessageBox::information(this, "FreeCAD", msg, tr("Yes"), tr("No"), QString::null, 0) == 0)
          emit startExtBrowser(name);
        return;
      }
    }
    else
    {
      if (QTextDrag::decode(mime, txt) == false)
      {
        QString msg = tr("Can't decode '%1'").arg(source);
        QMessageBox::information(this, "FreeCAD", msg);
        return;
      }
    }
  }

  if ( !mark.isEmpty() )
  {
    url += "#";
    url += mark;
  }

  if (d->bdStack.isEmpty() || d->bdStack.top() != url)
    d->bdStack.push(url);

  int bdStackCount = (int)d->bdStack.count();
  if ( d->bdStack.top() == url )
    bdStackCount--;

  if (d->tMode == FCTextBrowserPrivate::None)
    d->fdStack.clear();
  int fdStackCount = (int)d->fdStack.count();
  if ( d->fdStack.top() == url )
    fdStackCount--;

  QTextBrowser::setSource(name);

  emit backwardAvailable( bdStackCount > 0 );
  emit forwardAvailable( fdStackCount > 0 );
}

void FCTextBrowser::setText (const QString & contents, const QString & context)
{
  QTextBrowser::setText(contents, context);
}

void FCTextBrowser::backward()
{
  if ( d->bdStack.count() <= 1)
    return;

  d->fdStack.push( d->bdStack.pop() );

  d->tMode = FCTextBrowserPrivate::Backward;
  setSource( d->bdStack.pop() );
  d->tMode = FCTextBrowserPrivate::None;
  emit forwardAvailable( true );
}

void FCTextBrowser::forward()
{
  if ( d->fdStack.isEmpty() )
    return;

  d->tMode = FCTextBrowserPrivate::Forward;
  setSource( d->fdStack.pop() );
  d->tMode = FCTextBrowserPrivate::None;
  emit forwardAvailable( !d->fdStack.isEmpty() );
}

void FCTextBrowser::setMinWidthToReach (int width)
{
  d->minWidth = width;
}

void FCTextBrowser::viewportResizeEvent (QResizeEvent* e)
{
  if (e->size().width() > d->minWidth)
    emit minWidthReached(true);
  else
    minWidthReached(false);

  QTextBrowser::viewportResizeEvent(e);
}

void FCTextBrowser::viewportMousePressEvent (QMouseEvent * e)
{
  if (e->button() == RightButton)
  {
    // show the popup menu
    // (you have to connect this signal with a slot in your class)
    emit showPopupMenu();
  }
  else
    QTextBrowser::viewportMousePressEvent(e);
}

void FCTextBrowser::contentsDropEvent(QDropEvent  * e)
{
  if (QUriDrag::canDecode(e))
  {
    QStringList fn;
    QUriDrag::decodeLocalFiles(e, fn);
    QString file = fn.first();
    QFileInfo info(file);
    if (info.exists() && info.isFile())
      setSource(file);
  }
  else if (QTextDrag::canDecode(e))
  {
    QString text;
    QTextDrag::decode(e, text);
    if (QStyleSheet::mightBeRichText(text))
      setText(text);
  }
}

void FCTextBrowser::contentsDragEnterEvent  (QDragEnterEvent * e)
{
  bool can = QUriDrag::canDecode(e) || QTextDrag::canDecode(e);
  if ( !can )
    e->ignore();
}

void FCTextBrowser::contentsDragMoveEvent( QDragMoveEvent *e )
{
  bool can = QUriDrag::canDecode(e) || QTextDrag::canDecode(e);
  if ( !can )
    e->ignore();
}

//// FCHtmlViewValidator //////////////////////////////////////////////////////

FCHtmlViewValidator::FCHtmlViewValidator ( QWidget * parent, const char * name )
    : QValidator(parent, name)
{
}

FCHtmlViewValidator::~FCHtmlViewValidator ()
{
}

QValidator::State FCHtmlViewValidator::validate ( QString & txt, int & i) const
{
  if (txt.isEmpty())
    return Intermediate;
  else
    return Acceptable;
}

void FCHtmlViewValidator::fixup ( QString & txt) const
{
}

//// FCHtmlComboBox //////////////////////////////////////////////////////

FCHtmlComboBox::FCHtmlComboBox ( QWidget * parent, const char * name)
    : QComboBox(parent, name)
{
  setValidator(new FCHtmlViewValidator(this));
  connect(lineEdit(), SIGNAL(returnPressed()), this, SLOT(slotKeyPressReturn));
}

FCHtmlComboBox::FCHtmlComboBox ( bool rw, QWidget * parent, const char * name)
    : QComboBox(rw, parent, name)
{
  setValidator(new FCHtmlViewValidator(this));
  connect(lineEdit(), SIGNAL(returnPressed()), this, SLOT(slotKeyPressReturn()));
}

void FCHtmlComboBox::slotKeyPressReturn()
{
  emit returnPressed(currentText());
}

//// FCHtmlView //////////////////////////////////////////////////////

/*
 *  Constructs a FCHtmlView which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
FCHtmlView::FCHtmlView( const QString& home_,  QWidget* parent,  const char* name, WFlags fl )
  : DockWindow( 0L, parent, name, fl ), WindowParameter( name ), pclPathCombo( 0 )
{
  d = new FCHtmlViewPrivate;
  d->m_Process.Attach(this);

  //initialize
  init();

  d->m_strCaption = tr("FreeCAD Help - ");
  d->m_FCdoc = QString::null/*"FCdoc://"*/;
  d->m_FCext = "FCext://";
  d->m_FCscript = QString::null/*"FCScript://"*/;

  // find the home directory of the online manual
  d->m_strDocDir = GetDocDirectory() + PATHSEP;

  QString home = d->m_strDocDir;

  // this is the complete URL of the start page
  //
  // check if home_ is absolute or relative
  QDir dir (home_);
  if (dir.isRelative())
    home.append(home_);
  else
    home = home_;

  // read the old history and bookmark entries
  ReadHistory();
  ReadBookmarks();

  // create the actual browser
  //
  pclBrowser = new FCTextBrowser(this, "FCHelpViewer");
  pclBrowser->setMinWidthToReach(240);
  // set the path where the factory is looking for if you set a new source
  pclBrowser->mimeSourceFactory()->setFilePath( d->m_strDocDir );
  pclBrowser->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  connect(pclBrowser, SIGNAL(textChanged()), this, SLOT(TextChanged()));
  connect(pclBrowser, SIGNAL(startExtBrowser(QString)), this, SLOT(StartExtBrowser(QString)));

  // set the start page now
  if (!home.isEmpty())
    pclBrowser->setSource(home);

  // the browser's functionality
  connect( pclBrowser, SIGNAL(backwardAvailable(bool)), this, SLOT(SetBackwardAvailable(bool)));
  connect( pclBrowser, SIGNAL(forwardAvailable (bool)), this, SLOT(SetForwardAvailable (bool)));

  // create the button group for the layout
  pclButtonGrp = new QButtonGroup(this, "ButtonGroup");
  pclButtonGrp->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  pclButtonGrp->setProperty("title", tr( "" ));
  pclButtonGrp->setColumnLayout(0, Qt::Vertical);
  pclButtonGrp->layout()->setSpacing(0);
  pclButtonGrp->layout()->setMargin (0);
  pclButtonGrpLayout = new QHBoxLayout(pclButtonGrp->layout());
  pclButtonGrpLayout->setAlignment(Qt::AlignTop);
  pclButtonGrpLayout->setSpacing(1);
  pclButtonGrpLayout->setMargin (1);

  // create the buttons
  //
  //
  // the 'Backward' button
  pclButtonBack = new QToolButton(pclButtonGrp, tr("Backward"));
  pclButtonBack->setProperty("minimumSize", QSize(25, 25));
  pclButtonBack->setProperty("text", tr( "..." ) );
  pclButtonBack->setProperty("pixmap", QPixmap(back_pixmap));
  pclButtonBack->setAutoRaise(true);
  QToolTip::add(pclButtonBack, tr("Previous"));

  // the 'Forward' button
  pclButtonForward = new QToolButton( pclButtonGrp, tr("Forward") );
  pclButtonForward->setProperty( "minimumSize", QSize( 25, 25 ) );
  pclButtonForward->setProperty( "text", tr( "..." ) );
  pclButtonForward->setProperty( "pixmap", QPixmap(forward_pixmap) );
  pclButtonForward->setAutoRaise(true);
  QToolTip::add(pclButtonForward, tr("Next"));

  // the 'Home' button
  pclButtonHome = new QToolButton( pclButtonGrp, tr("Home") );
  pclButtonHome->setProperty( "minimumSize", QSize( 25, 25 ) );
  pclButtonHome->setProperty( "text", tr( "..." ) );
  pclButtonHome->setProperty( "pixmap", QPixmap(home_pixmap) );
  pclButtonHome->setAutoRaise(true);
  QToolTip::add(pclButtonHome, tr("Home"));

  // the 'Open' button
  pclButtonOpen = new QToolButton( pclButtonGrp, tr("Open") );
  pclButtonOpen->setProperty( "minimumSize", QSize( 25, 25 ) );
  pclButtonOpen->setProperty( "text", tr( "..." ) );
  pclButtonOpen->setProperty( "pixmap", QPixmap(open_pixmap) );
  pclButtonOpen->setAutoRaise(true);
  QToolTip::add(pclButtonOpen, tr("Open"));

  // the 'Path' combo box
  pclPathCombo = new FCHtmlComboBox( true, pclButtonGrp, "Paths" );
  pclPathCombo->setDuplicatesEnabled(false);
  pclPathCombo->setAutoCompletion(true);
  pclPathCombo->setProperty( "minimumSize", QSize( 25, 25 ) );
  pclPathCombo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
  pclPathCombo->hide();

  // the buttons' functionality
  pclButtonForward->setEnabled( false );
  pclButtonBack->setEnabled( false );
  connect(pclButtonBack,    SIGNAL(clicked()),                     pclBrowser,       SLOT(backward()));
  connect(pclBrowser,       SIGNAL(backwardAvailable(bool) ),      pclButtonBack,    SLOT(setEnabled(bool) ) );
  connect(pclBrowser,       SIGNAL(minWidthReached  (bool)),       this, SLOT(onMinWidthReached(bool)));
  connect(pclButtonForward, SIGNAL(clicked()),                     pclBrowser,       SLOT(forward()));
  connect(pclBrowser,       SIGNAL(forwardAvailable(bool) ),       pclButtonForward, SLOT(setEnabled(bool) ) );
  connect(pclButtonHome,    SIGNAL(clicked()),                     pclBrowser,       SLOT(home()));
  connect(pclButtonOpen,    SIGNAL(clicked()),                     this,             SLOT(OpenFile()));
  connect(pclPathCombo,     SIGNAL(activated( const QString & ) ), this, SLOT( PathSelected( const QString & ) ) );
  connect(pclPathCombo,     SIGNAL(returnPressed(QString)),        this, SLOT( StartScriptOrBrowser(QString)));

  // make the layout of the browser
  //
  //
  pclFormLayout = new QGridLayout( this );
  pclFormLayout->setSpacing( 1 );
  pclFormLayout->setMargin ( 1 );

  // add the three buttons and the combo box
  pclButtonGrpLayout->addWidget( pclButtonBack );
  pclButtonGrpLayout->addWidget( pclButtonForward );
  pclButtonGrpLayout->addWidget( pclButtonHome );
  pclButtonGrpLayout->addWidget( pclButtonOpen );
  QSpacerItem* spacer = new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  pclButtonGrpLayout->addItem( spacer );
  pclButtonGrpLayout->addWidget( pclPathCombo );

  // add the button group with its elements and the browser to the layout
  pclFormLayout->addWidget( pclButtonGrp, 0, 0 );
  pclFormLayout->addWidget( pclBrowser,   1, 0 );

  // insert the start page to the history
  pclPathCombo->insertItem( GetRelativeURL(home) );

  // create instances of the popup menus
  //
  pclPopup   = new QPopupMenu(0L);
  pclHistory = new QPopupMenu(0L);
  pclBookm   = new QPopupMenu(0L);
  // insert the history & bookmarks to the menu
  SetMaxHistory(5);
  CreateHistoryPopup ();
  CreateBookmarkPopup();

  connect(pclBrowser, SIGNAL(showPopupMenu()), this, SLOT(ShowPopupMenu()));
  connect(pclPopup, SIGNAL( aboutToShow()), this, SLOT( PopupMenuAboutToShow()));
  connect( pclHistory, SIGNAL( activated( int ) ), this, SLOT( HistChosen( int ) ) );
  connect( pclBookm, SIGNAL( activated( int ) ),   this, SLOT( BookmChosen( int ) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
FCHtmlView::~FCHtmlView()
{
  // no need to delete child widgets, Qt does it all for us
  d->m_Process.Detach(this);
  if (d->bHistory)
    SaveHistory();
  if (d->bBookm)
    SaveBookmarks();
  App::GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->Detach(this);

  delete d;
}

void FCHtmlView::init()
{
  try{
    // attach the help viewer to its preferences
    d->aStrGroupPath = "User parameter:BaseApp/Preferences/HelpViewer";
    App::GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->Attach(this);
    App::GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->Notify("EnableBookmarks");
    App::GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->Notify("EnableHistory");
    App::GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->Notify("NbOfHistoryItems");
    App::GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->Notify("NbOfBookmarks");
  }catch(const Base::Exception& rclE)
  {
    QMessageBox::warning(ApplicationWindow::Instance, "Wrong parameter", rclE.what());
  }
}

void FCHtmlView::SetEnableHistory  (bool b)
{
  d->bHistory = b;
}

void FCHtmlView::SetEnableBookmarks(bool b)
{
  d->bBookm = b;
}

void FCHtmlView::RefreshPage()
{
  pclBrowser->reload();
}

void FCHtmlView::ShowPopupMenu()
{
  pclPopup->exec(QCursor::pos());
}

void FCHtmlView::PopupMenuAboutToShow()
{
  // create and show up your context menu
  //
  pclPopup->clear();
  if (pclBrowser->hasSelectedText())
  {
    pclPopup->insertItem(tr("Copy"), pclBrowser, SLOT(copy()));
  }
  else
  {
    int iBack = pclPopup->insertItem(QPixmap(back_pixmap), tr("Previous"), pclBrowser, SLOT(backward()));
    pclPopup->setItemEnabled(iBack, d->bBackward);
    int iForw = pclPopup->insertItem(QPixmap(forward_pixmap), tr("Forward"), pclBrowser, SLOT(forward()));
    pclPopup->setItemEnabled(iForw, d->bForward);
    pclPopup->insertItem(QPixmap(home_pixmap), tr("Home"), pclBrowser, SLOT(home()));
    pclPopup->insertSeparator();
    pclPopup->insertItem(tr("Refresh"), this, SLOT(RefreshPage()));
    pclPopup->insertSeparator();
    if (d->bHistory == true)
      pclPopup->insertItem(tr("History"), pclHistory);
    if (d->bBookm == true)
    {
      CreateBookmarkPopup();
      pclPopup->insertItem(tr("Bookmarks"), pclBookm);
    }
    if (d->bHistory == true || d->bBookm == true)
      pclPopup->insertSeparator();
  }

  pclPopup->insertItem(tr("Select all"), pclBrowser, SLOT(selectAll()));
}

void FCHtmlView::SetBackwardAvailable( bool b)
{
  d->bBackward = b;
  pclButtonBack->setEnabled(b);
}

void FCHtmlView::SetForwardAvailable( bool b)
{
  d->bForward = b;
  pclButtonForward->setEnabled(b);
}

QString FCHtmlView::GetDocDirectory()
{
  QString home(App::GetApplication().GetHomePath());
  QString path = getWindowParameter()->GetASCII("OnlineDocDir", "/doc/free-cad.sourceforge.net/").c_str();

  QDir dir (home + path);

  if (dir.exists() == false)
  {
    QString msg = tr("Couldn't find the path for the Online help.\n");
    bool bMute = GuiConsoleObserver::bMute;
    GuiConsoleObserver::bMute = true;
    Base::Console().Error(msg.latin1());
    GuiConsoleObserver::bMute = bMute;
  }

  return dir.path();
}

QString FCHtmlView::GetScriptDirectory()
{
  QString path = getWindowParameter()->GetASCII("ScriptDir", "../src/Tools").c_str();

  QDir dir (path);
  dir.convertToAbs();
  path = dir.path();
  path.append("/");

  return path;
}

QString FCHtmlView::GetBrowserDirectory()
{
  //  QString browser = GetWindowParameter()->GetASCII("External Browser", "").c_str();
  QString browser = App::GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->GetASCII("LineEditBrowser", "").c_str();
  if (browser.isEmpty())
  {
    QMessageBox::information(this, tr("External browser"), tr("Please search for an external browser."));
    browser = QFileDialog::getOpenFileName();

    if (browser.isEmpty())
      QMessageBox::warning(this, tr("External browser"), tr("No external browser found."));
    else
      //      GetWindowParameter()->SetASCII("External Browser", browser.latin1());
      App::GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->SetASCII("LineEditBrowser", browser.latin1());
  }

  return browser;
}

QString FCHtmlView::GetRelativeURL (const QString& rcAbsPath) const
{
  QString cRelPath = rcAbsPath;
  QString sCurrDir = cRelPath.lower();

  // is this a pathname inside the online manual
  if (sCurrDir.startsWith(d->m_strDocDir.lower()) == true)
  {
    QString relativeURL = cRelPath.mid(d->m_strDocDir.length());
    relativeURL.prepend( d->m_FCdoc );
    cRelPath = relativeURL;
  }

  return cRelPath;
}

QString FCHtmlView::GetAbsoluteURL (const QString& rcRelPath) const
{
  QString cAbsPath = rcRelPath;
  QString sCurrDir = cAbsPath.lower();

  // generate an absolute URL from a relative one
  if (sCurrDir.startsWith(d->m_FCdoc.lower()) == true)
  {
    QString absoluteURL = cAbsPath.mid(d->m_FCdoc.length());
    absoluteURL.prepend(d->m_strDocDir);
    cAbsPath = absoluteURL;
  }

  return cAbsPath;
}

void FCHtmlView::TextChanged()
{
  // insert new item into the combo box
  //
  //
  if ( pclBrowser->documentTitle().isNull() )
  {
    setCaption( d->m_strCaption + pclBrowser->context() );
    d->selectedURL = pclBrowser->context();
  }
  else
  {
    setCaption( d->m_strCaption + pclBrowser->documentTitle() ) ;
    d->selectedURL = pclBrowser->documentTitle();
  }

  d->selectedURL = GetRelativeURL(d->selectedURL);

  if ( !d->selectedURL.isEmpty() && pclPathCombo )
  {
    bool exists = FALSE;
    int i;
    for ( i = 0; i < pclPathCombo->count(); ++i )
    {
      if ( pclPathCombo->text( i ) == d->selectedURL )
      {
        exists = TRUE;
        break;
      }
    }
    if ( !exists )
    {
      pclPathCombo->insertItem( d->selectedURL, 0 );
      pclPathCombo->setCurrentItem( 0 );
      if (d->bHistory)
        d->mHistory[pclHistory->insertItem(d->selectedURL, d->mHistory.size())] = d->selectedURL;
    }
    else
      pclPathCombo->setCurrentItem( i );

    d->selectedURL = QString::null;
  }
}

void FCHtmlView::OpenFile()
{
#ifndef QT_NO_FILEDIALOG
  QString fn = QFileDialog::getOpenFileName( QString::null, QString::null, this );
  if ( !fn.isEmpty() )
    pclBrowser->setSource( fn );
#endif
}

void FCHtmlView::AddToPath (const QString& path)
{
  // add new path if it is not inserted yet
  QString sCurrDir = path;
  sCurrDir.truncate(sCurrDir.findRev('/'));

  QStringList sl = pclBrowser->mimeSourceFactory()->filePath();
  if (sl.contains(sCurrDir) == false)
  {
    pclBrowser->mimeSourceFactory()->addFilePath(sCurrDir);
  }
}

void FCHtmlView::StartScriptOrBrowser(QString path)
{
  QString lpath  = path.lower();

  // start a script
  if (lpath.startsWith(d->m_FCscript.lower()) == true)
  {
    StartScript(path, d->m_FCscript);
  }
  // start an external browser
  else if (lpath.startsWith(d->m_FCext.lower()) == true)
  {
    StartBrowser(path, d->m_FCext);
  }
}

void FCHtmlView::StartExtBrowser(QString url)
{
  StartBrowser(url, "");
}

void FCHtmlView::StartBrowser(QString path, QString protocol)
{
  QString url = path.mid(protocol.length());

  QString browser = GetBrowserDirectory();
  if (browser.isEmpty())
    return;
#if 0
  // split into absolute path and filename
  QString sPath = browser;
  sPath   = sPath.left(sPath.findRev("/") + 1);
  browser = browser.mid(browser.findRev("/") + 1);

  // append the path of your favorite browser to global path
  d->m_Process.appendToPath(sPath.latin1());
#endif
  // create the command to execute
  d->m_Process.setExecutable(browser.latin1());
  d->m_Process << url.latin1();

  if (!d->m_Process.start())
    // delete the invalid entry
    App::GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->SetASCII("LineEditBrowser", "" );
}

void FCHtmlView::StartScript(QString path, QString protocol)
{
  QString currPath = QDir::currentDirPath();
  _chdir(GetScriptDirectory().latin1());

  path = path.mid(protocol.length());

  QDir dir (path);
  dir.convertToAbs();
  path = dir.path();

  // split into absolute path and filename
  QString script = path;
  path   = path.left(path.findRev("/") + 1);
  script = script.mid(script.findRev("/") + 1);


  _chdir(path.latin1());

  Gui::Process proc(QString("python")); 
  proc << script;
  if (!proc.start())
  {
    QString msg = tr("Sorry, cannot run file '%1'.").arg(script);
    QMessageBox::critical(this, "Script", msg);
  }
  else
  {
    QString msg = tr("'%1' done successfully.").arg(script);
    QMessageBox::information(this, "Script", msg);
  }

  // go to the former path
  _chdir(currPath.latin1());
}

void FCHtmlView::PathSelected( const QString & path )
{
  QString lpath  = path.lower();

  // if you want to start a script or a browser do nothing here
  // NOTE: to start a script or browser you must press Return/Enter
  if (lpath.startsWith(d->m_FCscript.lower()) || lpath.startsWith(d->m_FCext.lower()))
    return;

  // add new path if it is not inserted yet
  AddToPath(GetAbsoluteURL(path));

  // set new source
  pclBrowser->setSource( GetAbsoluteURL(path) );
  TextChanged();

  // insert to the history
  bool exists = FALSE;
  for ( std::map<int, QString>::iterator it = d->mHistory.begin(); it != d->mHistory.end(); ++it )
  {
    if ( it->second == path )
    {
      exists = TRUE;
      break;
    }
  }

  if ( !exists )
  {
    if (d->bHistory)
      d->mHistory[pclHistory->insertItem(path, d->mHistory.size())] = path;
  }
}

bool FCHtmlView::SetMaxHistory (long lCnt)
{
  FCParameterGrp::handle hHistGrp = getWindowParameter()->GetGroup("History");

  hHistGrp->SetInt("Max History items", lCnt);

  return true;
}

bool FCHtmlView::SetMaxBookmarks (long lCnt)
{
  FCParameterGrp::handle hBookmGrp = getWindowParameter()->GetGroup("Bookmarks");

  hBookmGrp->SetInt("Max Bookmark items", lCnt);

  return true;
}

void FCHtmlView::ReadHistory()
{
  // get all stored history items
  FCParameterGrp::handle hHistGrp = getWindowParameter()->GetGroup("History");
  std::vector<std::string> hist = hHistGrp->GetASCIIs("History");

  int i=0;
  for (std::vector<std::string>::iterator it = hist.begin(); it != hist.end(); ++it, i++)
  {
    d->mHistory[i] = it->c_str();
  }
}

void FCHtmlView::ReadBookmarks()
{
  // get all stored bookmark items
  FCParameterGrp::handle hBookmGrp = getWindowParameter()->GetGroup("Bookmarks");
  std::vector<std::string> bookm = hBookmGrp->GetASCIIs("Bookmark");

  int i=0;
  for (std::vector<std::string>::iterator it = bookm.begin(); it != bookm.end(); ++it, i++)
  {
    d->mBookmarks[i] = it->c_str();
  }
}

void FCHtmlView::SaveHistory()
{
  // write the history items into file
  FCParameterGrp::handle hHistGrp = getWindowParameter()->GetGroup("History");
  while ( int(d->mHistory.size()) > d->iMaxHist )
    d->mHistory.erase( d->mHistory.begin() );

  long i=0;
  for (std::map<int, QString>::iterator it = d->mHistory.begin(); it != d->mHistory.end(); ++it, i++)
  {
    char szBuf[200];
    sprintf(szBuf, "History %ld", i);
    hHistGrp->SetASCII(szBuf, it->second.latin1());
  }
}

void FCHtmlView::SaveBookmarks()
{
  // write the bookmark items into file
  FCParameterGrp::handle hBookmGrp = getWindowParameter()->GetGroup("Bookmarks");
  while ( int(d->mBookmarks.size()) > d->iMaxBookm )
    d->mBookmarks.erase( d->mBookmarks.begin() );

  long i=0;
  for (std::map<int, QString>::iterator it = d->mBookmarks.begin(); it != d->mBookmarks.end(); ++it, i++)
  {
    char szBuf[200];
    sprintf(szBuf, "Bookmark %ld", i);
    hBookmGrp->SetASCII(szBuf, it->second.latin1());
  }
}

void FCHtmlView::HistChosen( int i )
{
  if ( d->mHistory.find( i ) != d->mHistory.end() )
    pclBrowser->setSource( GetAbsoluteURL( d->mHistory[ i ] ));
}

void FCHtmlView::BookmChosen( int i )
{
  if ( d->mBookmarks.find( i ) != d->mBookmarks.end() )
  {
    QString sBookm = d->mBookmarks[ i ];
    pclBrowser->setSource( sBookm );
  }
}

void FCHtmlView::AddBookmark()
{
  if (d->bBookm)
  {
    bool bFound = false;
    QString txt = caption();
    QString path = pclPathCombo->currentText();
    path = GetAbsoluteURL(path);

    for ( std::map<int, QString>::iterator it = d->mBookmarks.begin(); it != d->mBookmarks.end(); ++it )
    {
      if (it->second == path)
      {
        bFound = true;
        break;
      }
    }

    if (bFound == false)
    {
      d->mBookmarks[pclBookm->insertItem(txt, d->mBookmarks.size())] = path;
    }
  }
}

void FCHtmlView::CreateBookmarkPopup()
{
  pclBookm->clear();
  std::map<int, QString> tmp = d->mBookmarks;
  d->mBookmarks.clear();

  pclBookm->insertItem( tr( "Add Bookmark" ), this, SLOT( AddBookmark() ) );
  if (tmp.size() > 0)
    pclBookm->insertItem( tr( "Update Bookmarks" ), this, SLOT( CheckBookmarks() ) );
  pclBookm->insertSeparator();

  int i = 0;
  for (std::map<int, QString>::iterator it = tmp.begin(); it != tmp.end(); ++it)
  {
    QString txt = d->m_strCaption + it->second;
    int iPos = pclBookm->insertItem(d->m_strCaption + it->second, i++);
    d->mBookmarks[iPos] = it->second;
  }
}

void FCHtmlView::CreateHistoryPopup()
{
  pclHistory->clear();
  std::map<int, QString> tmp = d->mHistory;
  d->mHistory.clear();

  for (std::map<int, QString>::iterator it = tmp.begin(); it != tmp.end(); ++it)
  {
    d->mHistory[pclHistory->insertItem(it->second, d->mHistory.size())] = it->second;
    AddToPath(GetAbsoluteURL(it->second));
  }
}

void FCHtmlView::CheckBookmarks()
{
  if (d->mBookmarks.size() == 0)
    return;

  int iButton = QMessageBox::information(this, "FreeCAD", tr("All unavailable bookmarks will be deleted\n"
                                         "Continue ?"), tr("Yes"), tr("No"), QString::null, 1);

  if (iButton != 0) // not Ok pressed
    return;

  std::map<int, QString> mChecked;
  for (std::map<int, QString>::iterator it = d->mBookmarks.begin(); it != d->mBookmarks.end(); ++it)
  {
    const QMimeSource * mime = pclBrowser->mimeSourceFactory()->data(it->second);
    if (mime == NULL)
    {
      QString msg = tr("%1 is unavailable.\n Do you want to delete it?").arg(it->second.latin1());
      iButton = QMessageBox::information(this, "FreeCAD", msg, tr("Yes"), tr("No"), QString::null, 0);

      if (iButton != 0)
      {
        // bookmark is not OK but take it anyway
        mChecked[it->first] = it->second;
      }
    }
    else // bookmark is OK
      mChecked[it->first] = it->second;
  }

  d->mBookmarks.clear();
  d->mBookmarks = mChecked;
  CreateBookmarkPopup();

  QMessageBox::information(this, "FreeCAD", tr("All bookmarks are uptodate"));
}

void FCHtmlView::OnChange(FCSubject<FCParameterGrp::MessageType> &rCaller,FCParameterGrp::MessageType sReason)
{
  FCParameterGrp& rclGrp = ((FCParameterGrp&)rCaller);

  if (strcmp(sReason, "EnableBookmarks") == 0)
    d->bBookm = rclGrp.GetBool(sReason, false);
  else if (strcmp(sReason, "EnableHistory") == 0)
    d->bHistory = rclGrp.GetBool(sReason, false);
  else if (strcmp(sReason, "NbOfHistoryItems") == 0)
  {
    d->iMaxHist = rclGrp.GetInt(sReason, 20);
    if (!d->bHistory) d->iMaxHist = 0;
  }
  else if (strcmp(sReason, "NbOfBookmarks") == 0)
  {
    d->iMaxBookm = rclGrp.GetInt(sReason, 20);
    if (!d->bBookm) d->iMaxBookm = 0;
  }
}

void FCHtmlView::OnChange (FCSubject<Gui::Process::MessageType> &rCaller,Gui::Process::MessageType rcReason)
{
  if (&d->m_Process != &rCaller)
    return;

  // observe incoming signals
  switch (rcReason)
  {
  case Gui::Process::processStarted:
    break;
  case Gui::Process::processFailed:
    {
      QMessageBox::critical(this, "Browser",
#ifdef FC_OS_WIN32
        Gui::Process::systemWarning( GetLastError(), d->m_Process.executable().latin1()) );
#else
      QObject::tr("Cannot start '%1'").arg(d->m_Process.executable().latin1()));
#endif
    } break;
  case Gui::Process::processExited:
    break;
  case Gui::Process::processKilled:
    break;
  case Gui::Process::receivedStdout:
    break;
  case Gui::Process::receivedStderr:
    break;
  case Gui::Process::wroteStdin:
    break;
  case Gui::Process::launchFinished:
    break;
  }
}

void FCHtmlView::onMinWidthReached (bool show)
{
  if (show)
    pclPathCombo->show();
  else
    pclPathCombo->hide();
}

#include "moc_HtmlView.cpp"

