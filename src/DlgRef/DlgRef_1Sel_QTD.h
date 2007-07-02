/********************************************************************************
** Form generated from reading ui file 'DlgRef_1Sel_QTD.ui'
**
** Created: Fri Jun 29 12:03:33 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef DLGREF_1SEL_QTD_H
#define DLGREF_1SEL_QTD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_DlgRef_1Sel_QTD
{
public:
    QGridLayout *gridLayout;
    QGroupBox *GroupBox1;
    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;
    QPushButton *PushButton1;
    QSpacerItem *spacerItem;
    QLineEdit *LineEdit1;
    QLabel *TextLabel1;

    void setupUi(QWidget *DlgRef_1Sel_QTD)
    {
    DlgRef_1Sel_QTD->setObjectName(QString::fromUtf8("DlgRef_1Sel_QTD"));
    gridLayout = new QGridLayout(DlgRef_1Sel_QTD);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    GroupBox1 = new QGroupBox(DlgRef_1Sel_QTD);
    GroupBox1->setObjectName(QString::fromUtf8("GroupBox1"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(GroupBox1->sizePolicy().hasHeightForWidth());
    GroupBox1->setSizePolicy(sizePolicy);
    gridLayout1 = new QGridLayout(GroupBox1);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(11);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    gridLayout2 = new QGridLayout();
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(0);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    PushButton1 = new QPushButton(GroupBox1);
    PushButton1->setObjectName(QString::fromUtf8("PushButton1"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(PushButton1->sizePolicy().hasHeightForWidth());
    PushButton1->setSizePolicy(sizePolicy1);

    gridLayout2->addWidget(PushButton1, 0, 1, 1, 1);

    spacerItem = new QSpacerItem(0, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout2->addItem(spacerItem, 1, 2, 1, 1);

    LineEdit1 = new QLineEdit(GroupBox1);
    LineEdit1->setObjectName(QString::fromUtf8("LineEdit1"));

    gridLayout2->addWidget(LineEdit1, 0, 2, 1, 1);

    TextLabel1 = new QLabel(GroupBox1);
    TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(TextLabel1->sizePolicy().hasHeightForWidth());
    TextLabel1->setSizePolicy(sizePolicy2);
    TextLabel1->setWordWrap(false);

    gridLayout2->addWidget(TextLabel1, 0, 0, 1, 1);


    gridLayout1->addLayout(gridLayout2, 0, 0, 1, 1);


    gridLayout->addWidget(GroupBox1, 0, 0, 1, 1);


    retranslateUi(DlgRef_1Sel_QTD);

    QSize size(129, 57);
    size = size.expandedTo(DlgRef_1Sel_QTD->minimumSizeHint());
    DlgRef_1Sel_QTD->resize(size);


    QMetaObject::connectSlotsByName(DlgRef_1Sel_QTD);
    } // setupUi

    void retranslateUi(QWidget *DlgRef_1Sel_QTD)
    {
    DlgRef_1Sel_QTD->setWindowTitle(QApplication::translate("DlgRef_1Sel_QTD", "DlgRef_1Sel_QTD", 0, QApplication::UnicodeUTF8));
    GroupBox1->setTitle(QString());
    PushButton1->setText(QString());
    TextLabel1->setText(QApplication::translate("DlgRef_1Sel_QTD", "TL1", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(DlgRef_1Sel_QTD);
    } // retranslateUi

};

namespace Ui {
    class DlgRef_1Sel_QTD: public Ui_DlgRef_1Sel_QTD {};
} // namespace Ui

#endif // DLGREF_1SEL_QTD_H
