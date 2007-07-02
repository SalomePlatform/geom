/********************************************************************************
** Form generated from reading ui file 'DlgRef_1Spin_QTD.ui'
**
** Created: Fri Jun 29 14:44:21 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef DLGREF_1SPIN_QTD_H
#define DLGREF_1SPIN_QTD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_DlgRef_1Spin_QTD
{
public:
    QGridLayout *gridLayout;
    QGroupBox *GroupBox1;
    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;
    QSpacerItem *spacerItem;
    QSpinBox *SpinBox1;
    QLabel *TextLabel1;

    void setupUi(QWidget *DlgRef_1Spin_QTD)
    {
    DlgRef_1Spin_QTD->setObjectName(QString::fromUtf8("DlgRef_1Spin_QTD"));
    gridLayout = new QGridLayout(DlgRef_1Spin_QTD);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    GroupBox1 = new QGroupBox(DlgRef_1Spin_QTD);
    GroupBox1->setObjectName(QString::fromUtf8("GroupBox1"));
    gridLayout1 = new QGridLayout(GroupBox1);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(11);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    gridLayout2 = new QGridLayout();
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(0);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    spacerItem = new QSpacerItem(0, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout2->addItem(spacerItem, 3, 1, 1, 1);

    SpinBox1 = new QSpinBox(GroupBox1);
    SpinBox1->setObjectName(QString::fromUtf8("SpinBox1"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(SpinBox1->sizePolicy().hasHeightForWidth());
    SpinBox1->setSizePolicy(sizePolicy);

    gridLayout2->addWidget(SpinBox1, 0, 1, 1, 1);

    TextLabel1 = new QLabel(GroupBox1);
    TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(TextLabel1->sizePolicy().hasHeightForWidth());
    TextLabel1->setSizePolicy(sizePolicy1);
    TextLabel1->setWordWrap(false);

    gridLayout2->addWidget(TextLabel1, 0, 0, 1, 1);


    gridLayout1->addLayout(gridLayout2, 0, 0, 1, 1);


    gridLayout->addWidget(GroupBox1, 0, 0, 1, 1);


    retranslateUi(DlgRef_1Spin_QTD);

    QSize size(124, 55);
    size = size.expandedTo(DlgRef_1Spin_QTD->minimumSizeHint());
    DlgRef_1Spin_QTD->resize(size);


    QMetaObject::connectSlotsByName(DlgRef_1Spin_QTD);
    } // setupUi

    void retranslateUi(QWidget *DlgRef_1Spin_QTD)
    {
    DlgRef_1Spin_QTD->setWindowTitle(QApplication::translate("DlgRef_1Spin_QTD", "DlgRef_1Spin_QTD", 0, QApplication::UnicodeUTF8));
    GroupBox1->setTitle(QString());
    TextLabel1->setText(QApplication::translate("DlgRef_1Spin_QTD", "TL1", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(DlgRef_1Spin_QTD);
    } // retranslateUi

};

namespace Ui {
    class DlgRef_1Spin_QTD: public Ui_DlgRef_1Spin_QTD {};
} // namespace Ui

#endif // DLGREF_1SPIN_QTD_H
