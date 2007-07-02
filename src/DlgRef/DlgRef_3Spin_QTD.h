/********************************************************************************
** Form generated from reading ui file 'DlgRef_3Spin_QTD.ui'
**
** Created: Fri Jun 29 14:47:42 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef DLGREF_3SPIN_QTD_H
#define DLGREF_3SPIN_QTD_H

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

class Ui_DlgRef_3Spin_QTD
{
public:
    QGridLayout *gridLayout;
    QGroupBox *GroupBox1;
    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;
    QSpinBox *SpinBox2;
    QSpacerItem *spacerItem;
    QLabel *TextLabel3;
    QSpinBox *SpinBox1;
    QSpinBox *SpinBox3;
    QLabel *TextLabel1;
    QLabel *TextLabel2;

    void setupUi(QWidget *DlgRef_3Spin_QTD)
    {
    DlgRef_3Spin_QTD->setObjectName(QString::fromUtf8("DlgRef_3Spin_QTD"));
    gridLayout = new QGridLayout(DlgRef_3Spin_QTD);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    GroupBox1 = new QGroupBox(DlgRef_3Spin_QTD);
    GroupBox1->setObjectName(QString::fromUtf8("GroupBox1"));
    gridLayout1 = new QGridLayout(GroupBox1);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(11);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    gridLayout2 = new QGridLayout();
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(0);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    SpinBox2 = new QSpinBox(GroupBox1);
    SpinBox2->setObjectName(QString::fromUtf8("SpinBox2"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(SpinBox2->sizePolicy().hasHeightForWidth());
    SpinBox2->setSizePolicy(sizePolicy);

    gridLayout2->addWidget(SpinBox2, 1, 1, 1, 1);

    spacerItem = new QSpacerItem(0, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout2->addItem(spacerItem, 3, 1, 1, 1);

    TextLabel3 = new QLabel(GroupBox1);
    TextLabel3->setObjectName(QString::fromUtf8("TextLabel3"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(TextLabel3->sizePolicy().hasHeightForWidth());
    TextLabel3->setSizePolicy(sizePolicy1);
    TextLabel3->setWordWrap(false);

    gridLayout2->addWidget(TextLabel3, 2, 0, 1, 1);

    SpinBox1 = new QSpinBox(GroupBox1);
    SpinBox1->setObjectName(QString::fromUtf8("SpinBox1"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(SpinBox1->sizePolicy().hasHeightForWidth());
    SpinBox1->setSizePolicy(sizePolicy2);

    gridLayout2->addWidget(SpinBox1, 0, 1, 1, 1);

    SpinBox3 = new QSpinBox(GroupBox1);
    SpinBox3->setObjectName(QString::fromUtf8("SpinBox3"));
    QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(SpinBox3->sizePolicy().hasHeightForWidth());
    SpinBox3->setSizePolicy(sizePolicy3);

    gridLayout2->addWidget(SpinBox3, 2, 1, 1, 1);

    TextLabel1 = new QLabel(GroupBox1);
    TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));
    QSizePolicy sizePolicy4(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(TextLabel1->sizePolicy().hasHeightForWidth());
    TextLabel1->setSizePolicy(sizePolicy4);
    TextLabel1->setWordWrap(false);

    gridLayout2->addWidget(TextLabel1, 0, 0, 1, 1);

    TextLabel2 = new QLabel(GroupBox1);
    TextLabel2->setObjectName(QString::fromUtf8("TextLabel2"));
    QSizePolicy sizePolicy5(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy5.setHorizontalStretch(0);
    sizePolicy5.setVerticalStretch(0);
    sizePolicy5.setHeightForWidth(TextLabel2->sizePolicy().hasHeightForWidth());
    TextLabel2->setSizePolicy(sizePolicy5);
    TextLabel2->setWordWrap(false);

    gridLayout2->addWidget(TextLabel2, 1, 0, 1, 1);


    gridLayout1->addLayout(gridLayout2, 0, 0, 1, 1);


    gridLayout->addWidget(GroupBox1, 0, 0, 1, 1);


    retranslateUi(DlgRef_3Spin_QTD);

    QSize size(124, 111);
    size = size.expandedTo(DlgRef_3Spin_QTD->minimumSizeHint());
    DlgRef_3Spin_QTD->resize(size);


    QMetaObject::connectSlotsByName(DlgRef_3Spin_QTD);
    } // setupUi

    void retranslateUi(QWidget *DlgRef_3Spin_QTD)
    {
    DlgRef_3Spin_QTD->setWindowTitle(QApplication::translate("DlgRef_3Spin_QTD", "DlgRef_3Spin_QTD", 0, QApplication::UnicodeUTF8));
    GroupBox1->setTitle(QString());
    TextLabel3->setText(QApplication::translate("DlgRef_3Spin_QTD", "TL3", 0, QApplication::UnicodeUTF8));
    TextLabel1->setText(QApplication::translate("DlgRef_3Spin_QTD", "TL1", 0, QApplication::UnicodeUTF8));
    TextLabel2->setText(QApplication::translate("DlgRef_3Spin_QTD", "TL2", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(DlgRef_3Spin_QTD);
    } // retranslateUi

};

namespace Ui {
    class DlgRef_3Spin_QTD: public Ui_DlgRef_3Spin_QTD {};
} // namespace Ui

#endif // DLGREF_3SPIN_QTD_H
