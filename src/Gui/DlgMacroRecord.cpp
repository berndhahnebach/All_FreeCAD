/****************************************************************************
** Form implementation generated from reading ui file '.\DlgMacroRecord.ui'
**
** Created: Sa 13. Mai 12:35:56 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgMacroRecord.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"39 37 133 2",
"Qt c None",
"#S c #100808",
"#J c #101010",
"#L c #181810",
"#s c #181818",
"#Y c #212118",
".6 c #212121",
"#1 c #291010",
"#Z c #292929",
"#w c #292931",
"#A c #310000",
"#D c #312929",
".o c #313129",
"#f c #313131",
"## c #313139",
".0 c #313939",
"#. c #390000",
"#m c #393931",
".F c #393939",
"a# c #393942",
"#W c #394239",
".Z c #420000",
".C c #423939",
"#O c #423942",
".U c #424239",
".v c #424242",
"#Q c #42424a",
".5 c #4a0000",
"#P c #4a4242",
"#g c #4a424a",
"#c c #4a4a42",
".w c #4a4a4a",
".j c #4a4a52",
"#M c #4a5229",
".K c #4a5a39",
"#K c #52524a",
".i c #525252",
".R c #5a3942",
".h c #5a5a52",
".g c #5a5a5a",
"#6 c #5a5a63",
"#t c #5a6331",
"#G c #5a6339",
"#R c #5a6352",
".J c #636321",
".V c #636331",
".p c #636363",
".A c #63636b",
"#B c #636b5a",
".2 c #636b63",
"#2 c #6b0000",
"#H c #6b3931",
"#j c #6b6b31",
"#F c #6b6b5a",
".k c #6b6b63",
".E c #6b6b6b",
".7 c #6b7331",
".O c #6b736b",
"#8 c #6b7373",
".L c #6b737b",
"#x c #733108",
"#I c #733910",
"#v c #733931",
".9 c #733939",
".Q c #734208",
"#q c #734210",
".X c #734231",
".f c #737373",
".r c #73737b",
".M c #737b73",
".u c #737b7b",
"#r c #7b0008",
"#E c #7b3929",
"#N c #7b4239",
"#T c #7b7373",
"#U c #7b737b",
"#y c #7b7b73",
".l c #7b7b7b",
"#9 c #7b7b84",
".P c #7b8418",
"#7 c #7b847b",
"#k c #7b8c63",
"#z c #7b8c6b",
".Y c #840000",
"#l c #844239",
"#X c #844a31",
".c c #84847b",
".H c #848484",
"#C c #848c10",
".3 c #848c18",
".8 c #848c63",
".W c #848c73",
".q c #848c84",
".4 c #8c0000",
"#e c #8c0008",
".G c #8c8484",
".d c #8c8c84",
"#b c #8c8c8c",
"a. c #8c8c94",
"#u c #8c947b",
"#i c #949484",
".n c #94948c",
"#V c #949494",
"#d c #94949c",
".N c #949ca5",
"#p c #9c949c",
".b c #9c9c94",
".s c #9c9c9c",
".1 c #9c9ca5",
".S c #9ca59c",
"#a c #a5a59c",
".D c #a5a5a5",
".x c #a5ada5",
".T c #adadad",
".I c #adb5ad",
"#o c #b5b5ad",
".z c #b5b5b5",
"#5 c #b5bdb5",
"aa c #bdb5ad",
"ab c #bdbdad",
".e c #bdbdb5",
".t c #bdbdbd",
".m c #bdc6b5",
".y c #c6bdbd",
".B c #c6c6bd",
"#0 c #c6c6c6",
"#n c #c6cec6",
"#h c #d6d6d6",
".a c #ded6d6",
".# c #deded6",
"#4 c #dedede",
"#3 c #e7e7e7",
"ac c #eff7ef",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.#Qt.aQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.b.c.c.c.c.d.e.#QtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.b.f.g.h.i.i.j.k.l.mQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.n.o.p.q.f.r.s.t.u.v.f.tQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.k.w.w.w.w.w.i.x.y.z.A.p.eQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.B.C.j.q.D.t.f.E.eQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.F.w.G.H.D.I.f.l.eQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.n.J.K.L.H.H.s.t.g.MQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.N.O.P.Q.R.u.G.S.T.F.U.mQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.s.V.W.X.Y.Z.0.g.T.s.v.v.c.eQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.1.2.3.Q.4.5.6.F.F.g.r.H.x.i.dQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.n.7.8.9.4#.###aQtQt.w.i#b.p#c#aQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt#d.O.P.Q#e.Z#f#aQtQtQt.f.w.i#g.b#hQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQt#i#j#k#l.Y.Z#m.sQtQtQtQt#n.E#c#b.B#a#oQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQt#p.O.P#q#r.Z.0.xQtQtQtQtQtQtQtQtQt.f#s.lQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQt.b#t#u#v.4.Z#w#aQtQtQtQtQtQtQtQtQtQt.E#f.E.BQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQt#d.2.3#x.Y#..F.xQtQtQtQtQtQtQtQtQtQtQtQtQt#y.E.zQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQt#i#j#z.X.4#A#f#aQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.l.bQtQtQt",
"QtQtQtQtQtQtQtQtQtQt.N#B#C#q.4#.#D.sQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt#nQtQtQtQt",
"QtQtQtQtQtQtQtQtQt#i#j#z#E.Y.Z.F.DQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQt.1#F#C#q#r.Z#D.DQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQt#hQtQt.b#G#u#H.4.Z#f.x.#.aQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQt#a#a#d#B#C#I#e.Z#J.E.s.b.b.b.b.b.b.b.b.b.b.b.b.b.b.b.b.b.b.b.b.b.zQtQt",
"QtQtQt.B#K#L#M#u#N.Y.Z#w#O#P.v.v.v.w#Q#P.v#P.v.C.F.v#P.v#P.v#P.v.v.v.v.v#bQtQt",
"QtQtQtQt.H#R.P#I#e#A#S.g.l.l.l#T.f.f.l#U.f.f.H.T.z#V.f.f.f.l.l.f.f.l.D.z.BQtQt",
"QtQtQt.e#W#y#X.4.5#Y#J.F.g.v#Z.v.g.w#Z.v.g#c#Z#V#0.g#O.i.w#Z.F.g.w#Z.GQtQtQtQt",
"QtQtQt.B.v#1#2#..F#b.F#a#3.T#f.T#3.z.v#V#3.z##.H.t.v.s#3#o.F.l#3#0.v.f#0QtQtQt",
"QtQtQtQt.T.v#S#f#V.D.F.T#4.D.F#5#h.I.w.s#4.I.F.H#5.v.T#4.T.v.H#h.t.w.f#0QtQtQt",
"QtQtQtQtQt.s#w.g.l.g#s.E.l.i#Y.f.l.g.6.p.l#6#Y.H#0.v.p#7.g#Z.i.l.k.6.fQt#nQtQt",
"QtQtQtQtQt.t.p.w.v.v.v.v#Q.v#W.v#Q.v#O.v.v.v#Q.S#0#8.v.v#Q.C.v.v.v.v#VQtQtQtQt",
"QtQtQtQt#V#b.H.u.l.u.l#T.l.l.l.l.l.l.l.l.l.l#9#b#b.H.l.l.l.l.l.l.l.l.Ga..TQtQt",
"QtQt#h.e#K#f.C.U.U.U.U.U.U.U.U.U.U.U.U.U.U.va#.F#m.C.v.U.U.U.U.U.U.v.F.F#b#hQt",
"QtQtQtQtaa.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.eab.e.BQtQt",
"QtQtQtQtacacacacacacacacacacacacacacacacacacacacacacacacacacacacacacacacQtQtQt"};

static const char* const image1_data[] = { 
"40 36 111 2",
"Qt c None",
"#D c #101010",
"#E c #181018",
"#z c #181818",
"#x c #182118",
"#u c #211821",
".E c #212121",
"#N c #292129",
".H c #292929",
"#y c #293129",
"#J c #312931",
".r c #313131",
".k c #313929",
"#A c #393139",
".s c #393931",
"#d c #393939",
"#v c #393942",
"#F c #394239",
"#t c #423942",
".5 c #424239",
".S c #424242",
"#G c #424a42",
"#R c #4a4242",
"#S c #4a424a",
"#P c #4a4a42",
".V c #4a4a4a",
"#c c #4a4a52",
"#a c #4a524a",
"#b c #4a5252",
"#s c #524a4a",
"#I c #524a52",
".g c #525210",
".Q c #52524a",
".6 c #525252",
"#M c #525a52",
"#L c #5a525a",
".L c #5a5a52",
".G c #5a5a5a",
".I c #5a5a63",
".D c #635a5a",
".q c #63635a",
".K c #636363",
".c c #6b6b4a",
".F c #6b6b63",
".J c #6b6b6b",
"## c #6b736b",
".j c #736b52",
".Y c #736b6b",
".f c #737308",
".P c #73736b",
".l c #737373",
".t c #7b7b73",
".R c #7b7b7b",
"#H c #7b847b",
"#w c #847b84",
".d c #848473",
".W c #84847b",
".T c #848484",
"#K c #84848c",
".U c #8c8484",
"#r c #8c848c",
".w c #8c8c84",
".O c #8c8c8c",
"#e c #8c948c",
"#q c #949494",
"#p c #949c94",
"#C c #9c949c",
".h c #9c9c94",
".N c #9c9c9c",
"#j c #9ca59c",
".m c #a5a59c",
".M c #a5a5a5",
".8 c #a5ad9c",
".9 c #a5ada5",
".7 c #ada5a5",
"#B c #ada5ad",
".v c #adada5",
".y c #adadad",
".z c #adb5a5",
"#m c #adb5ad",
"#. c #b5adad",
"#o c #b5adb5",
".x c #b5b5a5",
".i c #b5b5ad",
".p c #b5b5b5",
".# c #bdb5b5",
".b c #bdbdad",
".n c #bdbdb5",
".o c #bdbdbd",
".1 c #bdc6b5",
"#n c #bdc6bd",
".2 c #c6c6b5",
".u c #c6c6bd",
".a c #c6c6c6",
".e c #c6cebd",
".X c #cecebd",
".A c #cecec6",
"#l c #cecece",
".3 c #ced6ce",
".0 c #d6cece",
".Z c #d6d6c6",
".C c #d6d6ce",
"#g c #d6d6d6",
"#h c #d6ded6",
"#f c #d6dede",
"#i c #deded6",
".B c #dedede",
"#O c #dee7de",
".4 c #e7e7de",
"#k c #e7e7e7",
"#Q c #efe7ef",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.#.aQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.b.c.d.eQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.d.f.g.hQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.i.j.k.l.m.m.nQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.o.p.q.r.s.tQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.u.i.v.q.wQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQt.n.x.a.n.y.u.o.i.a.n.v.u.o.z.u.n.y.A.B.C.q.tQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQt.n.m.D.E.F.G.H.I.J.E.K.L.E.K.q.E.K.G.E.F.M.N.D.OQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQt.n.P.Q.R.G.S.T.K.S.R.q.S.U.K.S.R.J.S.T.G.S.V.V.W.XQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQt.Y.F.u.C.Z.n.C.C.u.A.C.n.C.0.1.C.0.2.C.3.n.n.n.Z.4QtQtQtQtQtQtQtQtQt",
"QtQtQtQtQt.i.5.6.i.7.v.v.7.v.v.v.8.v.v.v.v.v.9.v.9.v.v.v.v.9.m.m.m.m.v.uQtQtQtQt",
"QtQtQtQt#.##.S.V#a.V.V.V.V.V.V#a.V#a.V.V#a.V#b.V.V.V.V#c.V.V.Q.V.V.V#d.K.nQtQtQt",
"QtQtQtQt.L#e.B#f#g.B#g#h#g#h#g#g#g#g.B#h#g#i#g#g#h#h#g#h#h#g.B#g#g#g#j#d.RQtQtQt",
"QtQtQtQt.V.N#k#l#l.p.a#l#m.o#g.p.a#l.y.a#l.p.o#l.y#n#l.p.o#l#o.a.3#o.O#d.PQtQtQt",
"QtQtQtQt.Q.N#k#h#g.K#p#g.6.O.B.K#q#g.V#q.B.G#r#g.6#q#g.G.T.B.6#q#g.G.V#d.PQtQtQt",
"QtQtQtQt#s.N#k.N.R.E.S#r.E#t.T.r.S.T.E.S.O.H#d.T#u.S#r.H#d.O.E.S.R.H#v.S.PQtQtQt",
"QtQtQtQt.V.N#k#w.V.S#d#x.H#d.E.E#d.E#y#d#z.H#d.E.r#d#z.H#d.E.H.S#A#z.S.S.PQtQtQt",
"QtQtQtQt#a#p#k#l#g.B.a#d.T#n.S.J#n.S.O.a#d.R.a.S.O.a#d.T.a.6.T#l.y.S#d.S.PQtQtQt",
"QtQtQtQt#a.N#k#B#C.N#q.r.K#q#d.V#q.r.K#C.r.6#p#d.K.N.r.G#C.S.G.N.O.r.S.S.PQtQtQt",
"QtQtQtQt.V.N#k.R.S#u.H#d#D.H#d#z.H#d#D.H#d#E.E#d#D.H#F#E.E.V.V#G#d#E.S.S.PQtQtQt",
"QtQtQtQt.Q.N.4#g#h.G#H#l#t.R#l#I#H.a#d.R#l.V.l.a#d.T#l.S.J#k#k#k.a.S#t.S.PQtQtQt",
"QtQtQtQt.V.N#k#j#H#J.V.O.H.S.O#d#G.R.E.S.T#J.S.R.E.V.O.r.S.O.O.T.T.r.S.S.lQtQtQt",
"QtQtQtQt.V#p#k#K.G.G.V#z.r.V.E#y#L.G.G.G.G#M.6.G.G.V#x#d.V#N#d.G.V#z.S.S.PQtQtQt",
"QtQtQtQt.Q.h#k#l.B#k#l.S.O#l.V.J#k.B.B.B#g.B.B.B#O#l.S.T#l.6.T#k.a.S.S.S.PQtQtQt",
"QtQtQtQt#P#j#Q.N.T.T.T.H.V.T.r.S.O.T.T.T.T.T#H.T.T.T#y.V#r#d.6.O#w.r.S#v.PQtQtQt",
"QtQtQtQt#P.R#l.J.H.H.H.r#y.r#J#y.H.H.H.H.H.H#J.H.H.r.H.H.H.r.H.H#y.r.G#d.lQtQtQt",
"QtQtQtQt.W.V.Q.V#R#S.V.V.V#S.V.V.V.S.V.V.V.V#G.S.S.V.V.V.V.V.V#S.V.V#s.Q#qQtQtQt",
"QtQtQtQtQt.W.Q.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.L.hQtQtQtQt",
"QtQtQtQtQtQtQt#kQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt"};


/*
 *  Constructs a Gui::Dialog::DlgMacroRecord as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Gui::Dialog::DlgMacroRecord::DlgMacroRecord( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ),
      image0( (const char **) image0_data ),
      image1( (const char **) image1_data )
{
    if ( !name )
	setName( "Gui::Dialog::DlgMacroRecord" );
    DlgMacroRecordLayout = new QHBoxLayout( this, 11, 6, "DlgMacroRecordLayout"); 

    Layout9 = new QVBoxLayout( 0, 0, 6, "Layout9"); 

    TextLabel1 = new QLabel( this, "TextLabel1" );
    Layout9->addWidget( TextLabel1 );

    Layout8 = new QHBoxLayout( 0, 0, 6, "Layout8"); 

    LineEditPath = new QLineEdit( this, "LineEditPath" );
    LineEditPath->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, LineEditPath->sizePolicy().hasHeightForWidth() ) );
    LineEditPath->setFrameShape( QLineEdit::LineEditPanel );
    LineEditPath->setFrameShadow( QLineEdit::Sunken );
    Layout8->addWidget( LineEditPath );
    Layout9->addLayout( Layout8 );

    GroupBox4 = new QGroupBox( this, "GroupBox4" );
    GroupBox4->setColumnLayout(0, Qt::Vertical );
    GroupBox4->layout()->setSpacing( 6 );
    GroupBox4->layout()->setMargin( 11 );
    GroupBox4Layout = new QGridLayout( GroupBox4->layout() );
    GroupBox4Layout->setAlignment( Qt::AlignTop );

    PushButton9 = new QPushButton( GroupBox4, "PushButton9" );
    PushButton9->setEnabled( TRUE );
    PushButton9->setPixmap( image0 );

    GroupBox4Layout->addWidget( PushButton9, 0, 0 );

    PushButton10 = new QPushButton( GroupBox4, "PushButton10" );
    PushButton10->setPixmap( image1 );

    GroupBox4Layout->addWidget( PushButton10, 0, 2 );

    PushButton9_2 = new QPushButton( GroupBox4, "PushButton9_2" );
    PushButton9_2->setPixmap( image0 );

    GroupBox4Layout->addWidget( PushButton9_2, 0, 1 );

    TextLabel4 = new QLabel( GroupBox4, "TextLabel4" );
    TextLabel4->setAlignment( int( QLabel::AlignCenter ) );

    GroupBox4Layout->addWidget( TextLabel4, 1, 0 );

    TextLabel5 = new QLabel( GroupBox4, "TextLabel5" );
    TextLabel5->setAlignment( int( QLabel::AlignCenter ) );

    GroupBox4Layout->addWidget( TextLabel5, 1, 1 );

    TextLabel6 = new QLabel( GroupBox4, "TextLabel6" );
    TextLabel6->setAlignment( int( QLabel::AlignCenter ) );

    GroupBox4Layout->addWidget( TextLabel6, 1, 2 );
    Layout9->addWidget( GroupBox4 );
    DlgMacroRecordLayout->addLayout( Layout9 );

    Layout5 = new QVBoxLayout( 0, 0, 6, "Layout5"); 

    PushButtonStart = new QPushButton( this, "PushButtonStart" );
    PushButtonStart->setDefault( TRUE );
    Layout5->addWidget( PushButtonStart );

    PushButtonStop = new QPushButton( this, "PushButtonStop" );
    Layout5->addWidget( PushButtonStop );
    QSpacerItem* spacer = new QSpacerItem( 20, 50, QSizePolicy::Minimum, QSizePolicy::Fixed );
    Layout5->addItem( spacer );

    PushButtonCancel = new QPushButton( this, "PushButtonCancel" );
    Layout5->addWidget( PushButtonCancel );
    DlgMacroRecordLayout->addLayout( Layout5 );
    languageChange();
    resize( QSize(316, 171).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( PushButton9, SIGNAL( clicked() ), this, SLOT( onTieToolBar() ) );
    connect( PushButton9_2, SIGNAL( clicked() ), this, SLOT( onTieCommandBar() ) );
    connect( PushButton10, SIGNAL( clicked() ), this, SLOT( onTieKeyboard() ) );
    connect( PushButtonStart, SIGNAL( clicked() ), this, SLOT( start() ) );
    connect( PushButtonStop, SIGNAL( clicked() ), this, SLOT( stop() ) );
    connect( PushButtonCancel, SIGNAL( clicked() ), this, SLOT( cancel() ) );

    // tab order
    setTabOrder( LineEditPath, PushButton9 );
    setTabOrder( PushButton9, PushButton9_2 );
    setTabOrder( PushButton9_2, PushButton10 );
    setTabOrder( PushButton10, PushButtonStart );
    setTabOrder( PushButtonStart, PushButtonStop );
    setTabOrder( PushButtonStop, PushButtonCancel );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgMacroRecord::~DlgMacroRecord()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgMacroRecord::languageChange()
{
    setCaption( tr( "Macro recording" ) );
    TextLabel1->setText( tr( "Macro name:" ) );
    GroupBox4->setTitle( tr( "Tie macro:" ) );
    PushButton9->setText( QString::null );
    PushButton10->setText( QString::null );
    PushButton9_2->setText( QString::null );
    TextLabel4->setText( tr( "Tool bar" ) );
    TextLabel5->setText( tr( "Command bar" ) );
    TextLabel6->setText( tr( "Keyboard" ) );
    PushButtonStart->setText( tr( "Start" ) );
    PushButtonStop->setText( tr( "Stop" ) );
    PushButtonCancel->setText( tr( "Cancel" ) );
}

void Gui::Dialog::DlgMacroRecord::cancel()
{
    qWarning( "Gui::Dialog::DlgMacroRecord::cancel(): Not implemented yet" );
}

void Gui::Dialog::DlgMacroRecord::onTieCommandBar()
{
    qWarning( "Gui::Dialog::DlgMacroRecord::onTieCommandBar(): Not implemented yet" );
}

void Gui::Dialog::DlgMacroRecord::onTieKeyboard()
{
    qWarning( "Gui::Dialog::DlgMacroRecord::onTieKeyboard(): Not implemented yet" );
}

void Gui::Dialog::DlgMacroRecord::onTieToolBar()
{
    qWarning( "Gui::Dialog::DlgMacroRecord::onTieToolBar(): Not implemented yet" );
}

void Gui::Dialog::DlgMacroRecord::start()
{
    qWarning( "Gui::Dialog::DlgMacroRecord::start(): Not implemented yet" );
}

void Gui::Dialog::DlgMacroRecord::stop()
{
    qWarning( "Gui::Dialog::DlgMacroRecord::stop(): Not implemented yet" );
}

void Gui::Dialog::DlgMacroRecord::onSaveMacro()
{
    qWarning( "Gui::Dialog::DlgMacroRecord::onSaveMacro(): Not implemented yet" );
}

