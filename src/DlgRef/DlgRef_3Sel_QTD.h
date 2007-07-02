/********************************************************************************
** Form generated from reading ui file 'DlgRef_3Sel_QTD.ui'
**
** Created: Fri Jun 29 14:36:31 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef DLGREF_3SEL_QTD_H
#define DLGREF_3SEL_QTD_H

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

class Ui_DlgRef_3Sel_QTD
{
public:
    QGridLayout *gridLayout;
    QGroupBox *GroupBox1;
    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;
    QLineEdit *LineEdit3;
    QPushButton *PushButton1;
    QLineEdit *LineEdit2;
    QLabel *TextLabel1;
    QLabel *TextLabel3;
    QLineEdit *LineEdit1;
    QSpacerItem *spacerItem;
    QLabel *TextLabel2;
    QPushButton *PushButton3;
    QPushButton *PushButton2;

    void setupUi(QWidget *DlgRef_3Sel_QTD)
    {
    DlgRef_3Sel_QTD->setObjectName(QString::fromUtf8("DlgRef_3Sel_QTD"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(7));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(DlgRef_3Sel_QTD->sizePolicy().hasHeightForWidth());
    DlgRef_3Sel_QTD->setSizePolicy(sizePolicy);
    gridLayout = new QGridLayout(DlgRef_3Sel_QTD);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    GroupBox1 = new QGroupBox(DlgRef_3Sel_QTD);
    GroupBox1->setObjectName(QString::fromUtf8("GroupBox1"));
    gridLayout1 = new QGridLayout(GroupBox1);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(11);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    gridLayout2 = new QGridLayout();
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(0);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    LineEdit3 = new QLineEdit(GroupBox1);
    LineEdit3->setObjectName(QString::fromUtf8("LineEdit3"));

    gridLayout2->addWidget(LineEdit3, 2, 2, 1, 1);

    PushButton1 = new QPushButton(GroupBox1);
    PushButton1->setObjectName(QString::fromUtf8("PushButton1"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(PushButton1->sizePolicy().hasHeightForWidth());
    PushButton1->setSizePolicy(sizePolicy1);

    gridLayout2->addWidget(PushButton1, 0, 1, 1, 1);

    LineEdit2 = new QLineEdit(GroupBox1);
    LineEdit2->setObjectName(QString::fromUtf8("LineEdit2"));

    gridLayout2->addWidget(LineEdit2, 1, 2, 1, 1);

    TextLabel1 = new QLabel(GroupBox1);
    TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(TextLabel1->sizePolicy().hasHeightForWidth());
    TextLabel1->setSizePolicy(sizePolicy2);
    TextLabel1->setWordWrap(false);

    gridLayout2->addWidget(TextLabel1, 0, 0, 1, 1);

    TextLabel3 = new QLabel(GroupBox1);
    TextLabel3->setObjectName(QString::fromUtf8("TextLabel3"));
    QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(TextLabel3->sizePolicy().hasHeightForWidth());
    TextLabel3->setSizePolicy(sizePolicy3);
    TextLabel3->setWordWrap(false);

    gridLayout2->addWidget(TextLabel3, 2, 0, 1, 1);

    LineEdit1 = new QLineEdit(GroupBox1);
    LineEdit1->setObjectName(QString::fromUtf8("LineEdit1"));

    gridLayout2->addWidget(LineEdit1, 0, 2, 1, 1);

    spacerItem = new QSpacerItem(0, 159, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout2->addItem(spacerItem, 3, 2, 1, 1);

    TextLabel2 = new QLabel(GroupBox1);
    TextLabel2->setObjectName(QString::fromUtf8("TextLabel2"));
    QSizePolicy sizePolicy4(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(TextLabel2->sizePolicy().hasHeightForWidth());
    TextLabel2->setSizePolicy(sizePolicy4);
    TextLabel2->setWordWrap(false);

    gridLayout2->addWidget(TextLabel2, 1, 0, 1, 1);

    PushButton3 = new QPushButton(GroupBox1);
    PushButton3->setObjectName(QString::fromUtf8("PushButton3"));
    QSizePolicy sizePolicy5(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy5.setHorizontalStretch(0);
    sizePolicy5.setVerticalStretch(0);
    sizePolicy5.setHeightForWidth(PushButton3->sizePolicy().hasHeightForWidth());
    PushButton3->setSizePolicy(sizePolicy5);

    gridLayout2->addWidget(PushButton3, 2, 1, 1, 1);

    PushButton2 = new QPushButton(GroupBox1);
    PushButton2->setObjectName(QString::fromUtf8("PushButton2"));
    QSizePolicy sizePolicy6(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy6.setHorizontalStretch(0);
    sizePolicy6.setVerticalStretch(0);
    sizePolicy6.setHeightForWidth(PushButton2->sizePolicy().hasHeightForWidth());
    PushButton2->setSizePolicy(sizePolicy6);

    gridLayout2->addWidget(PushButton2, 1, 1, 1, 1);


    gridLayout1->addLayout(gridLayout2, 0, 0, 1, 1);


    gridLayout->addWidget(GroupBox1, 0, 0, 1, 1);


    retranslateUi(DlgRef_3Sel_QTD);

    QSize size(129, 117);
    size = size.expandedTo(DlgRef_3Sel_QTD->minimumSizeHint());
    DlgRef_3Sel_QTD->resize(size);


    QMetaObject::connectSlotsByName(DlgRef_3Sel_QTD);
    } // setupUi

    void retranslateUi(QWidget *DlgRef_3Sel_QTD)
    {
    DlgRef_3Sel_QTD->setWindowTitle(QApplication::translate("DlgRef_3Sel_QTD", "DlgRef_3Sel_QTD", 0, QApplication::UnicodeUTF8));
    GroupBox1->setTitle(QString());
    PushButton1->setText(QString());
    TextLabel1->setText(QApplication::translate("DlgRef_3Sel_QTD", "TL1", 0, QApplication::UnicodeUTF8));
    TextLabel3->setText(QApplication::translate("DlgRef_3Sel_QTD", "TL3", 0, QApplication::UnicodeUTF8));
    TextLabel2->setText(QApplication::translate("DlgRef_3Sel_QTD", "TL2", 0, QApplication::UnicodeUTF8));
    PushButton3->setText(QString());
    PushButton2->setText(QString());
    Q_UNUSED(DlgRef_3Sel_QTD);
    } // retranslateUi

};

namespace Ui {
    class DlgRef_3Sel_QTD: public Ui_DlgRef_3Sel_QTD {};
} // namespace Ui

#endif // DLGREF_3SEL_QTD_H
