/********************************************************************************
** Form generated from reading ui file 'DlgRef_1Sel1Check1List_QTD.ui'
**
** Created: Fri Jun 29 12:50:56 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef DLGREF_1SEL1CHECK1LIST_QTD_H
#define DLGREF_1SEL1CHECK1LIST_QTD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_DlgRef_1Sel1Check1List_QTD
{
public:
    QGridLayout *gridLayout;
    QGroupBox *GroupBox1;
    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;
    QSpacerItem *spacerItem;
    QRadioButton *CheckButton1;
    QLabel *TextLabel2;
    QComboBox *ComboBox1;
    QLineEdit *LineEdit1;
    QLabel *TextLabel1;
    QPushButton *PushButton1;

    void setupUi(QWidget *DlgRef_1Sel1Check1List_QTD)
    {
    DlgRef_1Sel1Check1List_QTD->setObjectName(QString::fromUtf8("DlgRef_1Sel1Check1List_QTD"));
    gridLayout = new QGridLayout(DlgRef_1Sel1Check1List_QTD);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    GroupBox1 = new QGroupBox(DlgRef_1Sel1Check1List_QTD);
    GroupBox1->setObjectName(QString::fromUtf8("GroupBox1"));
    gridLayout1 = new QGridLayout(GroupBox1);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(11);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    gridLayout2 = new QGridLayout();
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(0);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    spacerItem = new QSpacerItem(0, 166, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout2->addItem(spacerItem, 3, 2, 1, 1);

    CheckButton1 = new QRadioButton(GroupBox1);
    CheckButton1->setObjectName(QString::fromUtf8("CheckButton1"));

    gridLayout2->addWidget(CheckButton1, 2, 0, 1, 3);

    TextLabel2 = new QLabel(GroupBox1);
    TextLabel2->setObjectName(QString::fromUtf8("TextLabel2"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(TextLabel2->sizePolicy().hasHeightForWidth());
    TextLabel2->setSizePolicy(sizePolicy);
    TextLabel2->setWordWrap(false);

    gridLayout2->addWidget(TextLabel2, 1, 0, 1, 2);

    ComboBox1 = new QComboBox(GroupBox1);
    ComboBox1->setObjectName(QString::fromUtf8("ComboBox1"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(ComboBox1->sizePolicy().hasHeightForWidth());
    ComboBox1->setSizePolicy(sizePolicy1);

    gridLayout2->addWidget(ComboBox1, 1, 2, 1, 1);

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

    PushButton1 = new QPushButton(GroupBox1);
    PushButton1->setObjectName(QString::fromUtf8("PushButton1"));
    QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(PushButton1->sizePolicy().hasHeightForWidth());
    PushButton1->setSizePolicy(sizePolicy3);

    gridLayout2->addWidget(PushButton1, 0, 1, 1, 1);


    gridLayout1->addLayout(gridLayout2, 0, 0, 1, 1);


    gridLayout->addWidget(GroupBox1, 0, 0, 1, 1);


    retranslateUi(DlgRef_1Sel1Check1List_QTD);

    QSize size(129, 104);
    size = size.expandedTo(DlgRef_1Sel1Check1List_QTD->minimumSizeHint());
    DlgRef_1Sel1Check1List_QTD->resize(size);


    QMetaObject::connectSlotsByName(DlgRef_1Sel1Check1List_QTD);
    } // setupUi

    void retranslateUi(QWidget *DlgRef_1Sel1Check1List_QTD)
    {
    DlgRef_1Sel1Check1List_QTD->setWindowTitle(QApplication::translate("DlgRef_1Sel1Check1List_QTD", "DlgRef_1Sel1Check1List_QTD", 0, QApplication::UnicodeUTF8));
    GroupBox1->setTitle(QString());
    CheckButton1->setText(QString());
    TextLabel2->setText(QApplication::translate("DlgRef_1Sel1Check1List_QTD", "TL2", 0, QApplication::UnicodeUTF8));
    TextLabel1->setText(QApplication::translate("DlgRef_1Sel1Check1List_QTD", "TL1", 0, QApplication::UnicodeUTF8));
    PushButton1->setText(QString());
    Q_UNUSED(DlgRef_1Sel1Check1List_QTD);
    } // retranslateUi

};

namespace Ui {
    class DlgRef_1Sel1Check1List_QTD: public Ui_DlgRef_1Sel1Check1List_QTD {};
} // namespace Ui

#endif // DLGREF_1SEL1CHECK1LIST_QTD_H
