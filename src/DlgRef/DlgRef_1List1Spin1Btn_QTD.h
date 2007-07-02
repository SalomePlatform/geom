/********************************************************************************
** Form generated from reading ui file 'DlgRef_1List1Spin1Btn_QTD.ui'
**
** Created: Mon Jul 2 08:49:49 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef DLGREF_1LIST1SPIN1BTN_QTD_H
#define DLGREF_1LIST1SPIN1BTN_QTD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_DlgRef_1List1Spin1Btn_QTD
{
public:
    QGridLayout *gridLayout;
    QGroupBox *GroupBox1;
    QGridLayout *gridLayout1;
    QSpacerItem *spacerItem;
    QListView *ListView1;
    QLabel *TextLabel1;
    QSpinBox *SpinBox1;
    QPushButton *PushButton1;

    void setupUi(QWidget *DlgRef_1List1Spin1Btn_QTD)
    {
    DlgRef_1List1Spin1Btn_QTD->setObjectName(QString::fromUtf8("DlgRef_1List1Spin1Btn_QTD"));
    gridLayout = new QGridLayout(DlgRef_1List1Spin1Btn_QTD);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    GroupBox1 = new QGroupBox(DlgRef_1List1Spin1Btn_QTD);
    GroupBox1->setObjectName(QString::fromUtf8("GroupBox1"));
    gridLayout1 = new QGridLayout(GroupBox1);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(11);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    spacerItem = new QSpacerItem(0, 23, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout1->addItem(spacerItem, 3, 1, 1, 1);

    ListView1 = new QListView(GroupBox1);
    ListView1->setObjectName(QString::fromUtf8("ListView1"));

    gridLayout1->addWidget(ListView1, 0, 0, 4, 1);

    TextLabel1 = new QLabel(GroupBox1);
    TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));
    TextLabel1->setWordWrap(false);

    gridLayout1->addWidget(TextLabel1, 0, 1, 1, 1);

    SpinBox1 = new QSpinBox(GroupBox1);
    SpinBox1->setObjectName(QString::fromUtf8("SpinBox1"));

    gridLayout1->addWidget(SpinBox1, 1, 1, 1, 1);

    PushButton1 = new QPushButton(GroupBox1);
    PushButton1->setObjectName(QString::fromUtf8("PushButton1"));

    gridLayout1->addWidget(PushButton1, 2, 1, 1, 1);


    gridLayout->addWidget(GroupBox1, 0, 0, 1, 1);


    retranslateUi(DlgRef_1List1Spin1Btn_QTD);

    QSize size(204, 148);
    size = size.expandedTo(DlgRef_1List1Spin1Btn_QTD->minimumSizeHint());
    DlgRef_1List1Spin1Btn_QTD->resize(size);


    QMetaObject::connectSlotsByName(DlgRef_1List1Spin1Btn_QTD);
    } // setupUi

    void retranslateUi(QWidget *DlgRef_1List1Spin1Btn_QTD)
    {
    DlgRef_1List1Spin1Btn_QTD->setWindowTitle(QApplication::translate("DlgRef_1List1Spin1Btn_QTD", "DlgRef_3Sel4Spin2Check_QTD", 0, QApplication::UnicodeUTF8));
    GroupBox1->setTitle(QString());
    TextLabel1->setText(QApplication::translate("DlgRef_1List1Spin1Btn_QTD", "TL1", 0, QApplication::UnicodeUTF8));
    PushButton1->setText(QString());
    Q_UNUSED(DlgRef_1List1Spin1Btn_QTD);
    } // retranslateUi

};

namespace Ui {
    class DlgRef_1List1Spin1Btn_QTD: public Ui_DlgRef_1List1Spin1Btn_QTD {};
} // namespace Ui

#endif // DLGREF_1LIST1SPIN1BTN_QTD_H
