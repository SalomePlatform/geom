/********************************************************************************
** Form generated from reading ui file 'DlgRef_Skeleton_QTD.ui'
**
** Created: Fri Jun 29 11:48:51 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef DLGREF_SKELETON_QTD_H
#define DLGREF_SKELETON_QTD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_DlgRef_Skeleton_QTD
{
public:
    QGridLayout *gridLayout;
    QGridLayout *gridLayout1;
    QGroupBox *GroupMedium;
    QGroupBox *GroupButtons;
    QHBoxLayout *hboxLayout;
    QHBoxLayout *hboxLayout1;
    QPushButton *buttonOk;
    QPushButton *buttonApply;
    QSpacerItem *spacerItem;
    QPushButton *buttonCancel;
    QPushButton *buttonHelp;
    QGroupBox *GroupConstructors;
    QHBoxLayout *hboxLayout2;
    QHBoxLayout *hboxLayout3;
    QRadioButton *RadioButton1;
    QRadioButton *RadioButton2;
    QRadioButton *RadioButton3;
    QRadioButton *RadioButton4;
    QGroupBox *GroupBoxName;
    QGridLayout *gridLayout2;
    QGridLayout *gridLayout3;
    QLineEdit *ResultName;
    QLabel *NameLabel;

    void setupUi(QDialog *DlgRef_Skeleton_QTD)
    {
    DlgRef_Skeleton_QTD->setObjectName(QString::fromUtf8("DlgRef_Skeleton_QTD"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(7));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(DlgRef_Skeleton_QTD->sizePolicy().hasHeightForWidth());
    DlgRef_Skeleton_QTD->setSizePolicy(sizePolicy);
    DlgRef_Skeleton_QTD->setSizeGripEnabled(true);
    gridLayout = new QGridLayout(DlgRef_Skeleton_QTD);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout1 = new QGridLayout();
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(0);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    GroupMedium = new QGroupBox(DlgRef_Skeleton_QTD);
    GroupMedium->setObjectName(QString::fromUtf8("GroupMedium"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(GroupMedium->sizePolicy().hasHeightForWidth());
    GroupMedium->setSizePolicy(sizePolicy1);

    gridLayout1->addWidget(GroupMedium, 2, 0, 1, 1);

    GroupButtons = new QGroupBox(DlgRef_Skeleton_QTD);
    GroupButtons->setObjectName(QString::fromUtf8("GroupButtons"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(GroupButtons->sizePolicy().hasHeightForWidth());
    GroupButtons->setSizePolicy(sizePolicy2);
    hboxLayout = new QHBoxLayout(GroupButtons);
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(11);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    buttonOk = new QPushButton(GroupButtons);
    buttonOk->setObjectName(QString::fromUtf8("buttonOk"));

    hboxLayout1->addWidget(buttonOk);

    buttonApply = new QPushButton(GroupButtons);
    buttonApply->setObjectName(QString::fromUtf8("buttonApply"));

    hboxLayout1->addWidget(buttonApply);

    spacerItem = new QSpacerItem(91, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem);

    buttonCancel = new QPushButton(GroupButtons);
    buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));

    hboxLayout1->addWidget(buttonCancel);

    buttonHelp = new QPushButton(GroupButtons);
    buttonHelp->setObjectName(QString::fromUtf8("buttonHelp"));

    hboxLayout1->addWidget(buttonHelp);


    hboxLayout->addLayout(hboxLayout1);


    gridLayout1->addWidget(GroupButtons, 3, 0, 1, 1);

    GroupConstructors = new QGroupBox(DlgRef_Skeleton_QTD);
    GroupConstructors->setObjectName(QString::fromUtf8("GroupConstructors"));
    QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(GroupConstructors->sizePolicy().hasHeightForWidth());
    GroupConstructors->setSizePolicy(sizePolicy3);
    hboxLayout2 = new QHBoxLayout(GroupConstructors);
    hboxLayout2->setSpacing(6);
    hboxLayout2->setMargin(11);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    hboxLayout3 = new QHBoxLayout();
    hboxLayout3->setSpacing(6);
    hboxLayout3->setMargin(0);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    RadioButton1 = new QRadioButton(GroupConstructors);
    RadioButton1->setObjectName(QString::fromUtf8("RadioButton1"));

    hboxLayout3->addWidget(RadioButton1);

    RadioButton2 = new QRadioButton(GroupConstructors);
    RadioButton2->setObjectName(QString::fromUtf8("RadioButton2"));

    hboxLayout3->addWidget(RadioButton2);

    RadioButton3 = new QRadioButton(GroupConstructors);
    RadioButton3->setObjectName(QString::fromUtf8("RadioButton3"));

    hboxLayout3->addWidget(RadioButton3);

    RadioButton4 = new QRadioButton(GroupConstructors);
    RadioButton4->setObjectName(QString::fromUtf8("RadioButton4"));

    hboxLayout3->addWidget(RadioButton4);


    hboxLayout2->addLayout(hboxLayout3);


    gridLayout1->addWidget(GroupConstructors, 0, 0, 1, 1);

    GroupBoxName = new QGroupBox(DlgRef_Skeleton_QTD);
    GroupBoxName->setObjectName(QString::fromUtf8("GroupBoxName"));
    gridLayout2 = new QGridLayout(GroupBoxName);
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(11);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    gridLayout3 = new QGridLayout();
    gridLayout3->setSpacing(6);
    gridLayout3->setMargin(0);
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
    ResultName = new QLineEdit(GroupBoxName);
    ResultName->setObjectName(QString::fromUtf8("ResultName"));

    gridLayout3->addWidget(ResultName, 0, 1, 1, 1);

    NameLabel = new QLabel(GroupBoxName);
    NameLabel->setObjectName(QString::fromUtf8("NameLabel"));
    NameLabel->setWordWrap(false);

    gridLayout3->addWidget(NameLabel, 0, 0, 1, 1);


    gridLayout2->addLayout(gridLayout3, 0, 0, 1, 1);


    gridLayout1->addWidget(GroupBoxName, 1, 0, 1, 1);


    gridLayout->addLayout(gridLayout1, 0, 0, 1, 1);


    retranslateUi(DlgRef_Skeleton_QTD);

    QSize size(307, 378);
    size = size.expandedTo(DlgRef_Skeleton_QTD->minimumSizeHint());
    DlgRef_Skeleton_QTD->resize(size);


    QMetaObject::connectSlotsByName(DlgRef_Skeleton_QTD);
    } // setupUi

    void retranslateUi(QDialog *DlgRef_Skeleton_QTD)
    {
    DlgRef_Skeleton_QTD->setWindowTitle(QApplication::translate("DlgRef_Skeleton_QTD", "DlgRef_Skeleton_QTD", 0, QApplication::UnicodeUTF8));
    GroupMedium->setTitle(QString());
    GroupButtons->setTitle(QString());
    buttonOk->setText(QApplication::translate("DlgRef_Skeleton_QTD", "&Ok", 0, QApplication::UnicodeUTF8));
    buttonApply->setText(QApplication::translate("DlgRef_Skeleton_QTD", "&Apply", 0, QApplication::UnicodeUTF8));
    buttonCancel->setText(QApplication::translate("DlgRef_Skeleton_QTD", "&Cancel", 0, QApplication::UnicodeUTF8));
    buttonHelp->setText(QApplication::translate("DlgRef_Skeleton_QTD", "&Help", 0, QApplication::UnicodeUTF8));
    GroupConstructors->setTitle(QString());
    RadioButton1->setText(QString());
    RadioButton2->setText(QString());
    RadioButton3->setText(QString());
    RadioButton4->setText(QString());
    GroupBoxName->setTitle(QApplication::translate("DlgRef_Skeleton_QTD", "Result name", 0, QApplication::UnicodeUTF8));
    NameLabel->setText(QApplication::translate("DlgRef_Skeleton_QTD", "Name", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(DlgRef_Skeleton_QTD);
    } // retranslateUi

};

namespace Ui {
    class DlgRef_Skeleton_QTD: public Ui_DlgRef_Skeleton_QTD {};
} // namespace Ui

#endif // DLGREF_SKELETON_QTD_H
