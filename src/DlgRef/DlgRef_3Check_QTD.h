/********************************************************************************
** Form generated from reading ui file 'DlgRef_3Check_QTD.ui'
**
** Created: Fri Jul 6 10:09:03 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef DLGREF_3CHECK_QTD_H
#define DLGREF_3CHECK_QTD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_DlgRef_3Check_QTD
{
public:
    QGridLayout *gridLayout;
    QGroupBox *GroupBox1;
    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;
    QRadioButton *RadioButton3;
    QRadioButton *RadioButton1;
    QRadioButton *RadioButton2;
    QSpacerItem *spacerItem;

    void setupUi(QWidget *DlgRef_3Check_QTD)
    {
    DlgRef_3Check_QTD->setObjectName(QString::fromUtf8("DlgRef_3Check_QTD"));
    gridLayout = new QGridLayout(DlgRef_3Check_QTD);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    GroupBox1 = new QGroupBox(DlgRef_3Check_QTD);
    GroupBox1->setObjectName(QString::fromUtf8("GroupBox1"));
    gridLayout1 = new QGridLayout(GroupBox1);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(11);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    gridLayout2 = new QGridLayout();
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(0);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    RadioButton3 = new QRadioButton(GroupBox1);
    RadioButton3->setObjectName(QString::fromUtf8("RadioButton3"));

    gridLayout2->addWidget(RadioButton3, 2, 0, 1, 1);

    RadioButton1 = new QRadioButton(GroupBox1);
    RadioButton1->setObjectName(QString::fromUtf8("RadioButton1"));

    gridLayout2->addWidget(RadioButton1, 0, 0, 1, 1);

    RadioButton2 = new QRadioButton(GroupBox1);
    RadioButton2->setObjectName(QString::fromUtf8("RadioButton2"));

    gridLayout2->addWidget(RadioButton2, 1, 0, 1, 1);

    spacerItem = new QSpacerItem(0, 91, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout2->addItem(spacerItem, 3, 0, 1, 1);


    gridLayout1->addLayout(gridLayout2, 0, 0, 1, 1);


    gridLayout->addWidget(GroupBox1, 0, 0, 1, 1);

    QWidget::setTabOrder(RadioButton1, RadioButton2);
    QWidget::setTabOrder(RadioButton2, RadioButton3);

    retranslateUi(DlgRef_3Check_QTD);

    QSize size(135, 108);
    size = size.expandedTo(DlgRef_3Check_QTD->minimumSizeHint());
    DlgRef_3Check_QTD->resize(size);


    QMetaObject::connectSlotsByName(DlgRef_3Check_QTD);
    } // setupUi

    void retranslateUi(QWidget *DlgRef_3Check_QTD)
    {
    DlgRef_3Check_QTD->setWindowTitle(QApplication::translate("DlgRef_3Check_QTD", "DlgRef_3Check_QTD", 0, QApplication::UnicodeUTF8));
    GroupBox1->setTitle(QString());
    RadioButton3->setText(QApplication::translate("DlgRef_3Check_QTD", "RadioButton3", 0, QApplication::UnicodeUTF8));
    RadioButton1->setText(QApplication::translate("DlgRef_3Check_QTD", "RadioButton1", 0, QApplication::UnicodeUTF8));
    RadioButton2->setText(QApplication::translate("DlgRef_3Check_QTD", "RadioButton2", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(DlgRef_3Check_QTD);
    } // retranslateUi

};

namespace Ui {
    class DlgRef_3Check_QTD: public Ui_DlgRef_3Check_QTD {};
} // namespace Ui

#endif // DLGREF_3CHECK_QTD_H
