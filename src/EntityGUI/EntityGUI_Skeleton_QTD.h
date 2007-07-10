/********************************************************************************
** Form generated from reading ui file 'EntityGUI_Skeleton_QTD.ui'
**
** Created: Mon Jul 9 17:21:08 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef ENTITYGUI_SKELETON_QTD_H
#define ENTITYGUI_SKELETON_QTD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_EntityGUI_Skeleton_QTD
{
public:
    QGridLayout *gridLayout;
    QGridLayout *gridLayout1;
    QGroupBox *GroupVal;
    QGroupBox *GroupConstructors;
    QGridLayout *gridLayout2;
    QHBoxLayout *hboxLayout;
    QRadioButton *RadioButton1;
    QRadioButton *RadioButton2;
    QGroupBox *GroupDest;
    QGridLayout *gridLayout3;
    QGridLayout *gridLayout4;
    QGroupBox *GroupDest1;
    QGridLayout *gridLayout5;
    QGridLayout *gridLayout6;
    QRadioButton *RB_Dest2;
    QRadioButton *RB_Dest1;
    QGroupBox *GroupDest2;
    QGridLayout *gridLayout7;
    QGroupBox *GroupDest3;
    QGridLayout *gridLayout8;
    QGroupBox *GroupButtons;
    QHBoxLayout *hboxLayout1;
    QHBoxLayout *hboxLayout2;
    QPushButton *buttonEnd;
    QPushButton *buttonClose;
    QSpacerItem *spacerItem;
    QPushButton *buttonCancel;
    QPushButton *buttonHelp;

    void setupUi(QDialog *EntityGUI_Skeleton_QTD)
    {
    EntityGUI_Skeleton_QTD->setObjectName(QString::fromUtf8("EntityGUI_Skeleton_QTD"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(7));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(EntityGUI_Skeleton_QTD->sizePolicy().hasHeightForWidth());
    EntityGUI_Skeleton_QTD->setSizePolicy(sizePolicy);
    gridLayout = new QGridLayout(EntityGUI_Skeleton_QTD);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout1 = new QGridLayout();
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(0);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    GroupVal = new QGroupBox(EntityGUI_Skeleton_QTD);
    GroupVal->setObjectName(QString::fromUtf8("GroupVal"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(GroupVal->sizePolicy().hasHeightForWidth());
    GroupVal->setSizePolicy(sizePolicy1);

    gridLayout1->addWidget(GroupVal, 2, 0, 1, 1);

    GroupConstructors = new QGroupBox(EntityGUI_Skeleton_QTD);
    GroupConstructors->setObjectName(QString::fromUtf8("GroupConstructors"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(GroupConstructors->sizePolicy().hasHeightForWidth());
    GroupConstructors->setSizePolicy(sizePolicy2);
    gridLayout2 = new QGridLayout(GroupConstructors);
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(11);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    RadioButton1 = new QRadioButton(GroupConstructors);
    RadioButton1->setObjectName(QString::fromUtf8("RadioButton1"));

    hboxLayout->addWidget(RadioButton1);

    RadioButton2 = new QRadioButton(GroupConstructors);
    RadioButton2->setObjectName(QString::fromUtf8("RadioButton2"));

    hboxLayout->addWidget(RadioButton2);


    gridLayout2->addLayout(hboxLayout, 0, 0, 1, 1);


    gridLayout1->addWidget(GroupConstructors, 0, 0, 1, 1);

    GroupDest = new QGroupBox(EntityGUI_Skeleton_QTD);
    GroupDest->setObjectName(QString::fromUtf8("GroupDest"));
    QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(GroupDest->sizePolicy().hasHeightForWidth());
    GroupDest->setSizePolicy(sizePolicy3);
    gridLayout3 = new QGridLayout(GroupDest);
    gridLayout3->setSpacing(6);
    gridLayout3->setMargin(11);
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
    gridLayout4 = new QGridLayout();
    gridLayout4->setSpacing(6);
    gridLayout4->setMargin(0);
    gridLayout4->setObjectName(QString::fromUtf8("gridLayout4"));
    GroupDest1 = new QGroupBox(GroupDest);
    GroupDest1->setObjectName(QString::fromUtf8("GroupDest1"));
    QSizePolicy sizePolicy4(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(GroupDest1->sizePolicy().hasHeightForWidth());
    GroupDest1->setSizePolicy(sizePolicy4);
    gridLayout5 = new QGridLayout(GroupDest1);
    gridLayout5->setSpacing(6);
    gridLayout5->setMargin(11);
    gridLayout5->setObjectName(QString::fromUtf8("gridLayout5"));
    gridLayout6 = new QGridLayout();
    gridLayout6->setSpacing(6);
    gridLayout6->setMargin(0);
    gridLayout6->setObjectName(QString::fromUtf8("gridLayout6"));
    RB_Dest2 = new QRadioButton(GroupDest1);
    RB_Dest2->setObjectName(QString::fromUtf8("RB_Dest2"));

    gridLayout6->addWidget(RB_Dest2, 0, 1, 1, 1);

    RB_Dest1 = new QRadioButton(GroupDest1);
    RB_Dest1->setObjectName(QString::fromUtf8("RB_Dest1"));

    gridLayout6->addWidget(RB_Dest1, 0, 0, 1, 1);


    gridLayout5->addLayout(gridLayout6, 0, 0, 1, 1);


    gridLayout4->addWidget(GroupDest1, 0, 0, 1, 2);

    GroupDest2 = new QGroupBox(GroupDest);
    GroupDest2->setObjectName(QString::fromUtf8("GroupDest2"));
    QSizePolicy sizePolicy5(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
    sizePolicy5.setHorizontalStretch(0);
    sizePolicy5.setVerticalStretch(0);
    sizePolicy5.setHeightForWidth(GroupDest2->sizePolicy().hasHeightForWidth());
    GroupDest2->setSizePolicy(sizePolicy5);
    gridLayout7 = new QGridLayout(GroupDest2);
    gridLayout7->setSpacing(6);
    gridLayout7->setMargin(11);
    gridLayout7->setObjectName(QString::fromUtf8("gridLayout7"));

    gridLayout4->addWidget(GroupDest2, 1, 0, 1, 1);

    GroupDest3 = new QGroupBox(GroupDest);
    GroupDest3->setObjectName(QString::fromUtf8("GroupDest3"));
    QSizePolicy sizePolicy6(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
    sizePolicy6.setHorizontalStretch(0);
    sizePolicy6.setVerticalStretch(0);
    sizePolicy6.setHeightForWidth(GroupDest3->sizePolicy().hasHeightForWidth());
    GroupDest3->setSizePolicy(sizePolicy6);
    gridLayout8 = new QGridLayout(GroupDest3);
    gridLayout8->setSpacing(6);
    gridLayout8->setMargin(11);
    gridLayout8->setObjectName(QString::fromUtf8("gridLayout8"));

    gridLayout4->addWidget(GroupDest3, 1, 1, 1, 1);


    gridLayout3->addLayout(gridLayout4, 0, 0, 1, 1);


    gridLayout1->addWidget(GroupDest, 1, 0, 1, 1);

    GroupButtons = new QGroupBox(EntityGUI_Skeleton_QTD);
    GroupButtons->setObjectName(QString::fromUtf8("GroupButtons"));
    QSizePolicy sizePolicy7(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy7.setHorizontalStretch(0);
    sizePolicy7.setVerticalStretch(0);
    sizePolicy7.setHeightForWidth(GroupButtons->sizePolicy().hasHeightForWidth());
    GroupButtons->setSizePolicy(sizePolicy7);
    hboxLayout1 = new QHBoxLayout(GroupButtons);
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(11);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    hboxLayout2 = new QHBoxLayout();
    hboxLayout2->setSpacing(6);
    hboxLayout2->setMargin(0);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    buttonEnd = new QPushButton(GroupButtons);
    buttonEnd->setObjectName(QString::fromUtf8("buttonEnd"));

    hboxLayout2->addWidget(buttonEnd);

    buttonClose = new QPushButton(GroupButtons);
    buttonClose->setObjectName(QString::fromUtf8("buttonClose"));

    hboxLayout2->addWidget(buttonClose);

    spacerItem = new QSpacerItem(91, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout2->addItem(spacerItem);

    buttonCancel = new QPushButton(GroupButtons);
    buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));

    hboxLayout2->addWidget(buttonCancel);

    buttonHelp = new QPushButton(GroupButtons);
    buttonHelp->setObjectName(QString::fromUtf8("buttonHelp"));

    hboxLayout2->addWidget(buttonHelp);


    hboxLayout1->addLayout(hboxLayout2);


    gridLayout1->addWidget(GroupButtons, 3, 0, 1, 1);


    gridLayout->addLayout(gridLayout1, 0, 0, 1, 1);

    QWidget::setTabOrder(RadioButton1, RadioButton2);
    QWidget::setTabOrder(RadioButton2, RB_Dest1);
    QWidget::setTabOrder(RB_Dest1, RB_Dest2);
    QWidget::setTabOrder(RB_Dest2, buttonEnd);
    QWidget::setTabOrder(buttonEnd, buttonClose);
    QWidget::setTabOrder(buttonClose, buttonCancel);

    retranslateUi(EntityGUI_Skeleton_QTD);

    QSize size(317, 276);
    size = size.expandedTo(EntityGUI_Skeleton_QTD->minimumSizeHint());
    EntityGUI_Skeleton_QTD->resize(size);


    QMetaObject::connectSlotsByName(EntityGUI_Skeleton_QTD);
    } // setupUi

    void retranslateUi(QDialog *EntityGUI_Skeleton_QTD)
    {
    EntityGUI_Skeleton_QTD->setWindowTitle(QApplication::translate("EntityGUI_Skeleton_QTD", "EntityGUI_Skeleton_QTD", 0, QApplication::UnicodeUTF8));
    GroupVal->setTitle(QString());
    GroupConstructors->setTitle(QApplication::translate("EntityGUI_Skeleton_QTD", "Element Type", 0, QApplication::UnicodeUTF8));
    RadioButton1->setText(QApplication::translate("EntityGUI_Skeleton_QTD", "Segment", 0, QApplication::UnicodeUTF8));
    RadioButton2->setText(QApplication::translate("EntityGUI_Skeleton_QTD", "Arc", 0, QApplication::UnicodeUTF8));
    GroupDest->setTitle(QApplication::translate("EntityGUI_Skeleton_QTD", "Destination", 0, QApplication::UnicodeUTF8));
    GroupDest1->setTitle(QApplication::translate("EntityGUI_Skeleton_QTD", "Type", 0, QApplication::UnicodeUTF8));
    RB_Dest2->setText(QApplication::translate("EntityGUI_Skeleton_QTD", "Direction", 0, QApplication::UnicodeUTF8));
    RB_Dest1->setText(QApplication::translate("EntityGUI_Skeleton_QTD", "Point", 0, QApplication::UnicodeUTF8));
    GroupDest2->setTitle(QString());
    GroupDest3->setTitle(QString());
    GroupButtons->setTitle(QString());
    buttonEnd->setText(QApplication::translate("EntityGUI_Skeleton_QTD", "End Sketch", 0, QApplication::UnicodeUTF8));
    buttonClose->setText(QApplication::translate("EntityGUI_Skeleton_QTD", "Close Sketch", 0, QApplication::UnicodeUTF8));
    buttonCancel->setText(QApplication::translate("EntityGUI_Skeleton_QTD", "&Cancel", 0, QApplication::UnicodeUTF8));
    buttonHelp->setText(QApplication::translate("EntityGUI_Skeleton_QTD", "&Help", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(EntityGUI_Skeleton_QTD);
    } // retranslateUi

};

namespace Ui {
    class EntityGUI_Skeleton_QTD: public Ui_EntityGUI_Skeleton_QTD {};
} // namespace Ui

#endif // ENTITYGUI_SKELETON_QTD_H
